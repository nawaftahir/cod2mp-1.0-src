#include "../qcommon/qcommon.h"
#include "../qcommon/cmd.h"
#include "dvar.h"

const char *CL_GetUsernameForLocalClient(int controllerIndex);

void Dvar_ForEach(void (*callback)(const char *dvarName))
{
	dvar_t *dvar;

	for ( dvar = sortedDvars; dvar; dvar = dvar->next )
		callback(dvar->name);
}

static void Dvar_GetCombinedString(char *combined, int first)
{
	int i;
	int c;
	int l;
	int len;

	c = Cmd_Argc();
	*combined = 0;
	l = 0;

	for ( i = first; i < c; i++ )
	{
		len = strlen(Cmd_Argv(i) + 1);

		if ( l + len >= MAXPRINTMSG - 2 )
			break;

		I_strncat(combined, MAXPRINTMSG, Cmd_Argv(i));

		if ( i != c - 1 )
			I_strncat(combined, MAXPRINTMSG, " ");

		l += len;
	}
}

qboolean Dvar_Command()
{
	dvar_t *dvar;
	char dvar_value[MAXPRINTMSG];

	dvar = Dvar_FindVar(Cmd_Argv(0));

	if ( !dvar )
		return qfalse;

	if ( Cmd_Argc() == 1 )
	{
		Com_Printf("\"%s\" is: \"%s^7\" default: \"%s^7\"\n", dvar->name, Dvar_DisplayableValue(dvar), Dvar_DisplayableResetValue(dvar));
		if ( Dvar_HasLatchedValue(dvar) )
		{
			Com_Printf("latched: \"%s\"\n", Dvar_DisplayableLatchedValue(dvar));
		}
		Dvar_PrintDomain((DvarType)dvar->type, dvar->domain);
		return qtrue;
	}
	else
	{
		Dvar_GetCombinedString(dvar_value, 1);
		Dvar_SetCommand(Cmd_Argv(0), dvar_value);
	}

	return qtrue;
}

static bool Dvar_ToggleSimple(dvar_t *dvar)
{
	switch (dvar->type)
	{
	case DVAR_TYPE_BOOL:
		Dvar_SetBoolFromSource(dvar, !dvar->current.boolean, DVAR_SOURCE_EXTERNAL);
		return true;

	case DVAR_TYPE_INT:
		if (dvar->domain.integer.min <= 0 && dvar->domain.integer.max >= 1)
		{
			if (dvar->current.integer)
				Dvar_SetIntFromSource(dvar, 0, DVAR_SOURCE_EXTERNAL);
			else
				Dvar_SetIntFromSource(dvar, 1, DVAR_SOURCE_EXTERNAL);
		}
		else if (dvar->current.integer == dvar->domain.integer.min)
			Dvar_SetIntFromSource(dvar, dvar->domain.integer.max, DVAR_SOURCE_EXTERNAL);
		else
			Dvar_SetIntFromSource(dvar, dvar->domain.integer.min, DVAR_SOURCE_EXTERNAL);
		return true;

	case DVAR_TYPE_FLOAT:
		if (dvar->domain.decimal.min <= 0.0f && dvar->domain.decimal.max >= 1.0f)
		{
			if (dvar->current.decimal != 0.0f)
				Dvar_SetFloatFromSource(dvar, 0.0f, DVAR_SOURCE_EXTERNAL);
			else
				Dvar_SetFloatFromSource(dvar, 1.0f, DVAR_SOURCE_EXTERNAL);
		}
		else if (dvar->current.decimal == dvar->domain.decimal.min)
			Dvar_SetFloatFromSource(dvar, dvar->domain.decimal.max, DVAR_SOURCE_EXTERNAL);
		else
			Dvar_SetFloatFromSource(dvar, dvar->domain.decimal.min, DVAR_SOURCE_EXTERNAL);
		return true;

	case DVAR_TYPE_VEC2:
	case DVAR_TYPE_VEC3:
	case DVAR_TYPE_VEC4:
	case DVAR_TYPE_STRING:
	case DVAR_TYPE_COLOR:
		Com_Printf("'toggle' with no arguments makes no sense for dvar '%s'\n", dvar->name);
		return false;

	case DVAR_TYPE_ENUM:
		if (dvar->domain.enumeration.stringCount)
		{
			Dvar_SetIntFromSource(dvar, (dvar->current.integer + 1) % dvar->domain.enumeration.stringCount, DVAR_SOURCE_EXTERNAL);
		}
		return true;

	default:
		return false;
	}
}

static bool Dvar_ToggleInternal()
{
	const char* dvarName;
	dvar_t* dvar;
	const char* string;
	int argIndex;
	const char* argString;
	const char* enumString;

	if (Cmd_Argc() < 2)
	{
		Com_Printf("USAGE: %s <variable> <optional value sequence>\n", Cmd_Argv(0));
		return false;
	}

	dvarName = Cmd_Argv(1);
	dvar = Dvar_FindVar(dvarName);
	if (!dvar)
	{
		Com_Printf("toggle failed: dvar '%s' not found.\n", dvarName);
		return false;
	}

	if (Cmd_Argc() == 2)
	{
		return Dvar_ToggleSimple(dvar);
	}

	string = Dvar_DisplayableValue(dvar);
	for (argIndex = 2; argIndex + 1 < Cmd_Argc(); ++argIndex)
	{
		argString = Cmd_Argv(argIndex);
		if (dvar->type == DVAR_TYPE_ENUM)
		{
			enumString = Dvar_IndexStringToEnumString(dvar, argString);
			if (strlen(enumString))
			{
				argString = (char*)enumString;
			}
		}
		if (!stricmp(string, argString))
		{
			Dvar_SetCommand(dvarName, Cmd_Argv(argIndex + 1));
			return true;
		}
	}

	argString = Cmd_Argv(2);
	if (dvar->type == DVAR_TYPE_ENUM)
	{
		enumString = Dvar_IndexStringToEnumString(dvar, argString);
		if (strlen(enumString))
		{
			argString = (char*)enumString;
		}
	}
	Dvar_SetCommand(dvarName, argString);
	return true;
}

static void Dvar_Toggle_f(void)
{
	Dvar_ToggleInternal();
}

void Dvar_TogglePrint_f(void)
{
	const char *cmd;
	dvar_t *dvar;
	const char *value;

	if (!Dvar_ToggleInternal())
	{
		return;
	}

	cmd = Cmd_Argv(1);
	dvar = Dvar_FindVar(cmd);
	value = Dvar_DisplayableValue(dvar);
	Com_Printf("%s toggled to %s\n", cmd, value);
}

void Dvar_Set_f(void)
{
	int argc;
	char combined[MAXPRINTMSG];
	const char *dvarName;

	argc = Cmd_Argc();

	if ( argc < 3 )
	{
		Com_Printf("USAGE: set <variable> <value>\n");
		return;
	}

	dvarName = Cmd_Argv(1);

	if ( !Dvar_IsValidName(dvarName) )
	{
		Com_Printf("invalid variable name: %s\n", Cmd_Argv(1));
		return;
	}

	Dvar_GetCombinedString(combined, 2);
	Dvar_SetCommand(Cmd_Argv(1), combined);
}

static void Dvar_RegisterBool_f()
{
	int argc;
	const dvar_t *dvar;
	const char *dvarName;
	bool value;

	argc = Cmd_Argc();

	if ( argc != 3 )
	{
		Com_Printf("USAGE: %s <name> <default>\n", Cmd_Argv(0));
	}
	else
	{
		dvarName = Cmd_Argv(1);
		value = atoi(Cmd_Argv(2)) != 0;
		dvar = Dvar_FindVar(dvarName);

		if ( !dvar || dvar->type == DVAR_TYPE_STRING && dvar->flags & DVAR_EXTERNAL )
		{
			Dvar_RegisterBool(dvarName, value, DVAR_EXTERNAL);
		}
		else if ( dvar->type )
		{
			Com_Printf("dvar '%s' is not a boolean dvar\n", dvar->name);
		}
	}
}

static void Dvar_RegisterInt_f()
{
	int argc;
	const dvar_t *dvar;
	const char *dvarName;
	int value;
	int min;
	int max;

	argc = Cmd_Argc();

	if ( argc != 5 )
	{
		Com_Printf("USAGE: %s <name> <default> <min> <max>\n", Cmd_Argv(0));
		return;
	}

	dvarName = Cmd_Argv(1);
	value = atoi(Cmd_Argv(2));
	min = atoi(Cmd_Argv(3));
	max = atoi(Cmd_Argv(4));

	if ( min > max )
	{
		Com_Printf("dvar %s: min %i should not be greater than max %i\n", dvarName, min, max);
		return;
	}

	dvar = Dvar_FindVar(dvarName);

	if ( !dvar || dvar->type == DVAR_TYPE_STRING && dvar->flags & DVAR_EXTERNAL )
	{
		Dvar_RegisterInt(dvarName, value, min, max, DVAR_EXTERNAL);
	}
	else if ( dvar->type != DVAR_TYPE_INT && dvar->type != DVAR_TYPE_ENUM )
	{
		Com_Printf("dvar '%s' is not an integer dvar\n", dvar->name);
	}
}

static void Dvar_RegisterFloat_f()
{
	int argc;
	const dvar_t *dvar;
	const char *dvarName;
	float value;
	float min;
	float max;

	argc = Cmd_Argc();

	if ( argc != 5 )
	{
		Com_Printf("USAGE: %s <name> <default> <min> <max>\n", Cmd_Argv(0));
		return;
	}

	dvarName = Cmd_Argv(1);
	value = atof(Cmd_Argv(2));
	min = atof(Cmd_Argv(3));
	max = atof(Cmd_Argv(4));

	if ( min > max )
	{
		Com_Printf("dvar %s: min %g should not be greater than max %g\n", dvarName, min, max);
		return;
	}

	dvar = Dvar_FindVar(dvarName);

	if ( !dvar || dvar->type == DVAR_TYPE_STRING && dvar->flags & DVAR_EXTERNAL )
	{
		Dvar_RegisterFloat(dvarName, value, min, max, DVAR_EXTERNAL);
	}
	else if ( dvar->type != DVAR_TYPE_FLOAT )
	{
		Com_Printf("dvar '%s' is not an integer dvar\n", dvar->name);
	}
}

void Dvar_SetU_f(void)
{
	dvar_t *dvar;

	if ( Cmd_Argc() < 3 )
	{
		Com_Printf("USAGE: setu <variable> <value>\n");
		return;
	}

	Dvar_Set_f();
	dvar = Dvar_FindVar(Cmd_Argv(1));

	if ( !dvar )
		return;

	Dvar_AddFlags(dvar, DVAR_USERINFO);
}

void Dvar_SetS_f(void)
{
	dvar_t *dvar;

	if ( Cmd_Argc() < 3 )
	{
		Com_Printf("USAGE: sets <variable> <value>\n");
		return;
	}

	Dvar_Set_f();
	dvar = Dvar_FindVar(Cmd_Argv(1));

	if ( !dvar )
		return;

	Dvar_AddFlags(dvar, DVAR_SERVERINFO);
}

void Dvar_SetA_f(void)
{
	dvar_t *dvar;

	if ( Cmd_Argc() < 3 )
	{
		Com_Printf("USAGE: seta <variable> <value>\n");
		return;
	}

	Dvar_Set_f();
	dvar = Dvar_FindVar(Cmd_Argv(1));

	if ( !dvar )
		return;

	Dvar_AddFlags(dvar, DVAR_ARCHIVE);
}

static void Dvar_SetFromDvar_f(void)
{
	dvar_t *dvar;

	if (Cmd_Argc() != 3)
	{
		Com_Printf("USAGE: setfromdvar <dest_dvar> <source_dvar>\n");
		return;
	}

	dvar = Dvar_FindVar(Cmd_Argv(2));

	if (!dvar)
	{
		Com_Printf("dvar '%s' doesn't exist\n", Cmd_Argv(2));
		return;
	}

	Dvar_SetCommand(Cmd_Argv(1), Dvar_DisplayableValue(dvar));
}

static void Dvar_Reset_f(void)
{
	dvar_t *dvar;

	if (Cmd_Argc() != 2)
	{
		Com_Printf("USAGE: reset <variable>\n");
		return;
	}

	dvar = Dvar_FindVar(Cmd_Argv(1));

	if (!dvar)
		return;

	Dvar_Reset(dvar, DVAR_SOURCE_EXTERNAL);
}

void Dvar_WriteVariables(fileHandle_t f)
{
	dvar_t *dvar;

	for (dvar = sortedDvars; dvar; dvar = dvar->next)
	{
		if (!I_stricmp(dvar->name, "cl_cdkey"))
		{
			continue;
		}

		if (!(dvar->flags & DVAR_ARCHIVE))
		{
			continue;
		}

		FS_Printf(f, "seta %s \"%s\"\n", dvar->name, Dvar_DisplayableLatchedValue(dvar));
	}
}

void Dvar_WriteDefaults(fileHandle_t f)
{
	dvar_t *dvar;

	for (dvar = sortedDvars; dvar; dvar = dvar->next)
	{
		if (!I_stricmp(dvar->name, "cl_cdkey"))
		{
			continue;
		}

		if (dvar->flags & (DVAR_ROM | DVAR_CHEAT | DVAR_EXTERNAL))
		{
			continue;
		}

		FS_Printf(f, "set %s \"%s\"\n", dvar->name, Dvar_DisplayableResetValue(dvar));
	}
}

static void Dvar_List_f(void)
{
	dvar_t *dvar;
	const char *match;

	if ( Cmd_Argc() > 1 )
		match = Cmd_Argv(1);
	else
		match = NULL;

	for ( dvar = sortedDvars; dvar; dvar = dvar->next )
	{
		if (match && !Com_Filter(match, dvar->name, qfalse))
			continue;

		if (dvar->flags & (DVAR_SERVERINFO | DVAR_SERVERINFO_NOUPDATE))
			Com_Printf("S");
		else
			Com_Printf(" ");

		if (dvar->flags & DVAR_USERINFO)
			Com_Printf("U");
		else
			Com_Printf(" ");

		if (dvar->flags & DVAR_ROM)
			Com_Printf("R");
		else
			Com_Printf(" ");

		if (dvar->flags & DVAR_INIT)
			Com_Printf("I");
		else
			Com_Printf(" ");

		if (dvar->flags & DVAR_ARCHIVE)
			Com_Printf("A");
		else
			Com_Printf(" ");

		if (dvar->flags & DVAR_LATCH)
			Com_Printf("L");
		else
			Com_Printf(" ");

		if (dvar->flags & DVAR_CHEAT)
			Com_Printf("C");
		else
			Com_Printf(" ");

		Com_Printf(" %s \"%s\"\n", dvar->name, Dvar_DisplayableValue(dvar));
	}

	Com_Printf("\n%i total dvars\n", dvarCount);
}

void Com_DvarDump(print_msg_type_t channel)
{
	dvar_t *dvar;
	int count;
	const char *match;
	char summary[8192];

	if ( Cmd_Argc() > 1 )
		match = Cmd_Argv(1);
	else
		match = NULL;

	if (channel != PMSG_CONSOLE || (com_logfile && com_logfile->current.integer))
	{
		count = 0;

		Com_PrintMessage(channel, "=============================== DVAR DUMP ========================================\n");

		for (dvar = sortedDvars; dvar; dvar = dvar->next, count++)
		{
			if (!match || Com_Filter(match, dvar->name, 0))
			{
				if (Dvar_HasLatchedValue(dvar))
					Com_sprintf(summary, sizeof(summary), "      %s \"%s\" -- latched \"%s\"\n", dvar->name,
					            Dvar_DisplayableValue(dvar), Dvar_DisplayableLatchedValue(dvar));
				else
					Com_sprintf(summary, sizeof(summary), "      %s \"%s\"\n", dvar->name, Dvar_DisplayableValue(dvar));

				Com_PrintMessage(channel, summary);
			}
		}

		Com_sprintf(summary, sizeof(summary), "\n%i total dvars\n%i dvar indexes\n", count, dvarCount);
		Com_PrintMessage(channel, summary);
		Com_PrintMessage(channel, "=============================== END DVAR DUMP =====================================\n");
	}
}

static void Dvar_Dump_f(void)
{
	Com_DvarDump(PMSG_CONSOLE);
}

void SV_SetConfig(int start, int max, int bit)
{
	dvar_t *dvar;

	for ( dvar = sortedDvars; dvar; dvar = dvar->next )
	{
		if ( dvar->flags & bit )
		{
			SV_SetConfigValueForKey(start, max, dvar->name, Dvar_DisplayableValue(dvar));
		}
	}
}

char *Dvar_InfoString(int bit)
{
	static char info[MAX_INFO_STRING];
	dvar_t *dvar;

	info[0] = 0;

	for ( dvar = sortedDvars; dvar; dvar = dvar->next )
	{
		if ( dvar->flags & bit )
		{
			Info_SetValueForKey( info, dvar->name, Dvar_DisplayableValue(dvar) );
		}
	}

	if ( bit & DVAR_USERINFO )
	{
		Info_SetValueForKey( info, "name", CL_GetUsernameForLocalClient(0) );
	}

	return info;
}

char *Dvar_InfoString_Big(int bit)
{
	static char info[BIG_INFO_STRING];
	dvar_t *dvar;

	info[0] = 0;

	for ( dvar = sortedDvars; dvar; dvar = dvar->next )
	{
		if ( dvar->flags & bit )
		{
			Info_SetValueForKey_Big( info, dvar->name, Dvar_DisplayableValue(dvar) );
		}
	}

	return info;
}

void Dvar_AddCommands()
{
	Cmd_AddCommand("toggle", Dvar_Toggle_f);
	Cmd_AddCommand("togglep", Dvar_TogglePrint_f);
	Cmd_AddCommand("set", Dvar_Set_f);
	Cmd_AddCommand("sets", Dvar_SetS_f);
	Cmd_AddCommand("seta", Dvar_SetA_f);
	Cmd_AddCommand("setfromdvar", Dvar_SetFromDvar_f);
	Cmd_AddCommand("reset", Dvar_Reset_f);
	Cmd_AddCommand("dvarlist", Dvar_List_f);
	Cmd_AddCommand("dvardump", Dvar_Dump_f);
	Cmd_AddCommand("dvar_bool", Dvar_RegisterBool_f);
	Cmd_AddCommand("dvar_int", Dvar_RegisterInt_f);
	Cmd_AddCommand("dvar_float", Dvar_RegisterFloat_f);
	Cmd_AddCommand("setu", Dvar_SetU_f);
}

