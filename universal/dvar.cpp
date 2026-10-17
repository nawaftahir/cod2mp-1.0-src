#include "../qcommon/qcommon.h"
#include "com_memory.h"
#include "dvar.h"

dvar_t *sortedDvars;
static dvar_t *dvar_cheats;
int dvar_modifiedFlags;
static dvar_t dvarPool[MAX_DVARS];
int dvarCount;

static const char *s_dvarOffString = "off";
static const char *s_dvarOnString = "on";

static dvar_t *dvarHashTable[FILE_HASH_SIZE];
static float dvarVectorPool[12];
static unsigned int dvarVectorPoolIndex;
static bool isDvarSystemActive;
static bool isLoadingAutoExec;

static const char digitStrings[10][2] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

void Dvar_AddFlags(dvar_t *dvar, int flags);

void Dvar_SetInAutoExec(bool inAutoExec)
{
	isLoadingAutoExec = inAutoExec;
}

bool Dvar_IsSystemActive()
{
	return isDvarSystemActive;
}

static int generateHashValue(const char *fname)
{
	int i;
	int hash;
	int letter;

	if ( !fname )
		Com_Error(ERR_DROP, "\x15" "null name in generateHashValue");

	hash = 0;
	i = 0;

	while ( fname[i] != '\0' )
	{
		letter = tolower(fname[i]);
		hash += letter * (i + 119);
		i++;
	}

	hash &= FILE_HASH_SIZE - 1;
	return hash;
}

bool Dvar_IsValidName(const char *dvarName)
{
	int index;
	char nameChar;

	if ( !dvarName )
		return false;

	for ( index = 0; dvarName[index]; ++index )
	{
		nameChar = dvarName[index];
		if ( !isalnum(nameChar) && nameChar != '_' )
			return false;
	}

	return true;
}

// temporary vectors for string conversions; the ring wraps when full
static float *Dvar_AllocVector(unsigned int componentCount)
{
	float *vector;

	if ( dvarVectorPoolIndex + componentCount > 12 )
		dvarVectorPoolIndex = 0;

	vector = &dvarVectorPool[dvarVectorPoolIndex];
	dvarVectorPoolIndex += componentCount;
	return vector;
}

// current, latched and reset share one allocation
static void Dvar_AllocVectors(dvar_t *dvar)
{
	dvar->current.vector = (float *)Z_Malloc(3 * dvar->type * sizeof(float));
	dvar->latched.vector = dvar->current.vector + dvar->type;
	dvar->reset.vector = dvar->latched.vector + dvar->type;
}

static void Dvar_FreeVectors(dvar_t *dvar)
{
	Z_Free(dvar->current.vector);
}

static const char *Dvar_AllocNameString(const char *name)
{
	return CopyString(name);
}

static void Dvar_FreeNameString(const char *name)
{
	Z_Free((void *)name);
}

// the empty string, single digits and "on"/"off" are shared constants
static const char *Dvar_InternString(const char *string)
{
	unsigned int length;

	if ( !*string )
		return "";

	length = I_strlen(string);

	if ( !string[1] )
	{
		if ( *string >= '0' && *string <= '9' )
			return digitStrings[*string - '0'];
	}
	else if ( *string == 'o' )
	{
		if ( length == 3 && string[1] == 'f' && string[2] == 'f' && !string[3] )
			return s_dvarOffString;

		if ( length == 2 && string[1] == 'n' && !string[2] )
			return s_dvarOnString;
	}

	return CopyString(string);
}

static void Dvar_FreeInternString(const char *string)
{
	if ( !*string )
		return;

	if ( !string[1] && *string >= '0' && *string <= '9' )
		return;

	if ( string == s_dvarOffString || string == s_dvarOnString )
		return;

	Z_Free((void *)string);
}

static void Dvar_FreeCurrentString(dvar_t *dvar)
{
	if ( dvar->current.string != dvar->latched.string && dvar->current.string != dvar->reset.string )
		Dvar_FreeInternString(dvar->current.string);

	dvar->current.string = NULL;
}

static void Dvar_FreeLatchedString(dvar_t *dvar)
{
	if ( dvar->latched.string != dvar->current.string && dvar->latched.string != dvar->reset.string )
		Dvar_FreeInternString(dvar->latched.string);

	dvar->latched.string = NULL;
}

static void Dvar_FreeResetString(dvar_t *dvar)
{
	if ( dvar->reset.string != dvar->current.string && dvar->reset.string != dvar->latched.string )
		Dvar_FreeInternString(dvar->reset.string);

	dvar->reset.string = NULL;
}

static void Dvar_SetCurrentStringValue(dvar_t *dvar, const char *string)
{
	if ( dvar->latched.string && (string == dvar->latched.string || !strcmp(string, dvar->latched.string)) )
		dvar->current.string = dvar->latched.string;
	else if ( dvar->reset.string && (string == dvar->reset.string || !strcmp(string, dvar->reset.string)) )
		dvar->current.string = dvar->reset.string;
	else
		dvar->current.string = Dvar_InternString(string);
}

static void Dvar_SetLatchedStringValue(dvar_t *dvar, const char *string)
{
	if ( dvar->current.string && (string == dvar->current.string || !strcmp(string, dvar->current.string)) )
		dvar->latched.string = dvar->current.string;
	else if ( dvar->reset.string && (string == dvar->reset.string || !strcmp(string, dvar->reset.string)) )
		dvar->latched.string = dvar->reset.string;
	else
		dvar->latched.string = Dvar_InternString(string);
}

static void Dvar_SetResetStringValue(dvar_t *dvar, const char *string)
{
	if ( dvar->current.string && (string == dvar->current.string || !strcmp(string, dvar->current.string)) )
		dvar->reset.string = dvar->current.string;
	else if ( dvar->latched.string && (string == dvar->latched.string || !strcmp(string, dvar->latched.string)) )
		dvar->reset.string = dvar->latched.string;
	else
		dvar->reset.string = Dvar_InternString(string);
}

const char *Dvar_EnumToString(const dvar_t *dvar)
{
	if ( !dvar->domain.enumeration.stringCount )
		return "";

	return dvar->domain.enumeration.strings[dvar->current.integer];
}

const char *Dvar_IndexStringToEnumString(const dvar_t *dvar, const char *indexString)
{
	int index;
	int indexStringLen;
	int i;

	if ( !dvar->domain.enumeration.stringCount )
		return "";

	indexStringLen = I_strlen(indexString);
	for ( i = 0; i < indexStringLen; ++i )
	{
		if ( !isdigit(indexString[i]) )
			return "";
	}

	index = atoi(indexString);
	return dvar->domain.enumeration.strings[index];
}

static const char *Dvar_ValueToString(const dvar_t *dvar, DvarValue value)
{
	switch ( dvar->type )
	{
	case DVAR_TYPE_BOOL:
		return value.boolean ? "1" : "0";
	case DVAR_TYPE_INT:
		return va("%i", value.integer);
	case DVAR_TYPE_FLOAT:
		return va("%g", value.decimal);
	case DVAR_TYPE_VEC2:
		return va("%g %g", value.vector[0], value.vector[1]);
	case DVAR_TYPE_VEC3:
		return va("%g %g %g", value.vector[0], value.vector[1], value.vector[2]);
	case DVAR_TYPE_VEC4:
		return va("%g %g %g %g", value.vector[0], value.vector[1], value.vector[2], value.vector[3]);
	case DVAR_TYPE_COLOR:
		return va("%g %g %g %g",
		          value.color[0] * (1.0f / 255.0f), value.color[1] * (1.0f / 255.0f),
		          value.color[2] * (1.0f / 255.0f), value.color[3] * (1.0f / 255.0f));
	case DVAR_TYPE_ENUM:
		if ( !dvar->domain.enumeration.stringCount )
			return "";
		return dvar->domain.enumeration.strings[value.integer];
	case DVAR_TYPE_STRING:
		return va("%s", value.string);
	default:
		return "";
	}
}

static bool Dvar_StringToBool(const char *string)
{
	return atoi(string) != 0;
}

static int Dvar_StringToInt(const char *string)
{
	return atoi(string);
}

static float Dvar_StringToFloat(const char *string)
{
	return atof(string);
}

static float *Dvar_StringToVec2(const char *string)
{
	float *vector;

	vector = Dvar_AllocVector(2);
	Vector2Clear(vector);
	sscanf(string, "%g %g", &vector[0], &vector[1]);
	return vector;
}

static float *Dvar_StringToVec3(const char *string)
{
	float *vector;

	vector = Dvar_AllocVector(3);
	VectorClear(vector);
	sscanf(string, "%g %g %g", &vector[0], &vector[1], &vector[2]);
	return vector;
}

static float *Dvar_StringToVec4(const char *string)
{
	float *vector;

	vector = Dvar_AllocVector(4);
	Vector4Clear(vector);
	sscanf(string, "%g %g %g %g", &vector[0], &vector[1], &vector[2], &vector[3]);
	return vector;
}

// accepts a name, an index, or an unambiguous name prefix
int Dvar_StringToEnum(const DvarLimits *domain, const char *string)
{
	int stringIndex;
	const char *digit;
	int len;

	for ( stringIndex = 0; stringIndex < domain->enumeration.stringCount; ++stringIndex )
	{
		if ( !strcasecmp(string, domain->enumeration.strings[stringIndex]) )
			return stringIndex;
	}

	stringIndex = 0;
	for ( digit = string; *digit; ++digit )
	{
		if ( *digit < '0' || *digit > '9' )
			return DVAR_INVALID_ENUM_INDEX;
		stringIndex = 10 * stringIndex + *digit - '0';
	}

	if ( stringIndex >= 0 && stringIndex < domain->enumeration.stringCount )
		return stringIndex;

	len = I_strlen(string);
	for ( stringIndex = 0; stringIndex < domain->enumeration.stringCount; ++stringIndex )
	{
		if ( !strncasecmp(string, domain->enumeration.strings[stringIndex], len) )
			return stringIndex;
	}

	return DVAR_INVALID_ENUM_INDEX;
}

static void Dvar_StringToColor(const char *string, unsigned char *color)
{
	vec4_t colorVec;

	Vector4Clear(colorVec);
	sscanf(string, "%g %g %g %g", &colorVec[0], &colorVec[1], &colorVec[2], &colorVec[3]);

	color[0] = I_fround(I_fmax(0.0f, I_fmin(1.0f, colorVec[0])) * 255.0f);
	color[1] = I_fround(I_fmax(0.0f, I_fmin(1.0f, colorVec[1])) * 255.0f);
	color[2] = I_fround(I_fmax(0.0f, I_fmin(1.0f, colorVec[2])) * 255.0f);
	color[3] = I_fround(I_fmax(0.0f, I_fmin(1.0f, colorVec[3])) * 255.0f);
}

static DvarValue Dvar_StringToValue(unsigned char type, DvarLimits domain, const char *string)
{
	DvarValue value;

	switch ( type )
	{
	case DVAR_TYPE_BOOL:
		value.boolean = Dvar_StringToBool(string);
		break;
	case DVAR_TYPE_INT:
		value.integer = Dvar_StringToInt(string);
		break;
	case DVAR_TYPE_FLOAT:
		value.decimal = Dvar_StringToFloat(string);
		break;
	case DVAR_TYPE_VEC2:
		value.vector = Dvar_StringToVec2(string);
		break;
	case DVAR_TYPE_VEC3:
		value.vector = Dvar_StringToVec3(string);
		break;
	case DVAR_TYPE_VEC4:
		value.vector = Dvar_StringToVec4(string);
		break;
	case DVAR_TYPE_ENUM:
		value.integer = Dvar_StringToEnum(&domain, string);
		break;
	case DVAR_TYPE_STRING:
		value.string = string;
		break;
	case DVAR_TYPE_COLOR:
		Dvar_StringToColor(string, value.color);
		break;
	default:
		value.integer = 0;
		break;
	}

	return value;
}

const char *Dvar_DisplayableValue(const dvar_t *dvar)
{
	const char *value;

	value = Dvar_ValueToString(dvar, dvar->current);
	return value;
}

const char *Dvar_DisplayableResetValue(const dvar_t *dvar)
{
	const char *value;

	value = Dvar_ValueToString(dvar, dvar->reset);
	return value;
}

const char *Dvar_DisplayableLatchedValue(const dvar_t *dvar)
{
	const char *value;

	value = Dvar_ValueToString(dvar, dvar->latched);
	return value;
}

static void Dvar_ClampVectorToDomain(float *vector, int components, float min, float max)
{
	int channel;

	for ( channel = 0; channel < components; channel++ )
	{
		if ( vector[channel] < min )
			vector[channel] = min;
		else if ( vector[channel] > max )
			vector[channel] = max;
	}
}

static bool Dvar_VectorInDomain(const float *vector, int components, float min, float max)
{
	int channel;

	for ( channel = 0; channel < components; channel++ )
	{
		if ( vector[channel] < min )
			return false;
		if ( vector[channel] > max )
			return false;
	}

	return true;
}

static DvarValue Dvar_ClampValueToDomain(unsigned char type, DvarValue value, const DvarValue resetValue, const DvarLimits domain)
{
	switch ( type )
	{
	case DVAR_TYPE_BOOL:
		value.boolean = value.boolean != 0;
		break;
	case DVAR_TYPE_INT:
		if ( value.integer < domain.integer.min )
			value.integer = domain.integer.min;
		else if ( value.integer > domain.integer.max )
			value.integer = domain.integer.max;
		break;
	case DVAR_TYPE_FLOAT:
		if ( value.decimal < domain.decimal.min )
			value.decimal = domain.decimal.min;
		else if ( value.decimal > domain.decimal.max )
			value.decimal = domain.decimal.max;
		break;
	case DVAR_TYPE_VEC2:
		Dvar_ClampVectorToDomain(value.vector, 2, domain.decimal.min, domain.decimal.max);
		break;
	case DVAR_TYPE_VEC3:
		Dvar_ClampVectorToDomain(value.vector, 3, domain.decimal.min, domain.decimal.max);
		break;
	case DVAR_TYPE_VEC4:
		Dvar_ClampVectorToDomain(value.vector, 4, domain.decimal.min, domain.decimal.max);
		break;
	case DVAR_TYPE_ENUM:
		if ( value.integer < 0 || value.integer >= domain.enumeration.stringCount )
			value.integer = resetValue.integer;
		break;
	case DVAR_TYPE_STRING:
	case DVAR_TYPE_COLOR:
		break;
	}

	return value;
}

static bool Dvar_ValueInDomain(unsigned char type, DvarValue value, DvarLimits domain)
{
	switch ( type )
	{
	case DVAR_TYPE_BOOL:
		return true;
	case DVAR_TYPE_INT:
		if ( value.integer < domain.integer.min )
			return false;
		if ( value.integer > domain.integer.max )
			return false;
		return true;
	case DVAR_TYPE_FLOAT:
		if ( value.decimal < domain.decimal.min )
			return false;
		if ( value.decimal > domain.decimal.max )
			return false;
		return true;
	case DVAR_TYPE_ENUM:
		return (value.integer >= 0 && value.integer < domain.enumeration.stringCount) || value.integer == 0;
	case DVAR_TYPE_VEC2:
		return Dvar_VectorInDomain(value.vector, 2, domain.decimal.min, domain.decimal.max);
	case DVAR_TYPE_VEC3:
		return Dvar_VectorInDomain(value.vector, 3, domain.decimal.min, domain.decimal.max);
	case DVAR_TYPE_VEC4:
		return Dvar_VectorInDomain(value.vector, 4, domain.decimal.min, domain.decimal.max);
	case DVAR_TYPE_STRING:
		return true;
	case DVAR_TYPE_COLOR:
		return true;
	default:
		return false;
	}
}

static void Dvar_VectorDomainToString(int components, DvarLimits domain, char *outBuffer, int outBufferLen)
{
	if ( domain.decimal.min != -FLT_MAX )
	{
		if ( domain.decimal.max != FLT_MAX )
			snprintf(outBuffer, outBufferLen, "Domain is any %iD vector with components from %g to %g", components, domain.decimal.min, domain.decimal.max);
		else
			snprintf(outBuffer, outBufferLen, "Domain is any %iD vector with components %g or bigger", components, domain.decimal.min);
	}
	else
	{
		if ( domain.decimal.max != FLT_MAX )
			snprintf(outBuffer, outBufferLen, "Domain is any %iD vector with components %g or smaller", components, domain.decimal.max);
		else
			snprintf(outBuffer, outBufferLen, "Domain is any %iD vector", components);
	}
}

static const char *Dvar_DomainToString_Internal(unsigned char type, DvarLimits domain, char *outBuffer, int outBufferLen, int *outLineCount)
{
	char *outBufferEnd;
	int charsWritten;
	int stringIndex;

	outBufferEnd = &outBuffer[outBufferLen];
	if ( outLineCount )
		*outLineCount = 0;

	switch ( type )
	{
	case DVAR_TYPE_BOOL:
		snprintf(outBuffer, outBufferLen, "Domain is 0 or 1");
		break;
	case DVAR_TYPE_INT:
		if ( domain.integer.min != INT_MIN )
		{
			if ( domain.integer.max != INT_MAX )
				snprintf(outBuffer, outBufferLen, "Domain is any integer from %i to %i", domain.integer.min, domain.integer.max);
			else
				snprintf(outBuffer, outBufferLen, "Domain is any integer %i or bigger", domain.integer.min);
		}
		else
		{
			if ( domain.integer.max != INT_MAX )
				snprintf(outBuffer, outBufferLen, "Domain is any integer %i or smaller", domain.integer.max);
			else
				snprintf(outBuffer, outBufferLen, "Domain is any integer");
		}
		break;
	case DVAR_TYPE_FLOAT:
		if ( domain.decimal.min != -FLT_MAX )
		{
			if ( domain.decimal.max != FLT_MAX )
				snprintf(outBuffer, outBufferLen, "Domain is any number from %g to %g", domain.decimal.min, domain.decimal.max);
			else
				snprintf(outBuffer, outBufferLen, "Domain is any number %g or bigger", domain.decimal.min);
		}
		else
		{
			if ( domain.decimal.max != FLT_MAX )
				snprintf(outBuffer, outBufferLen, "Domain is any number %g or smaller", domain.decimal.max);
			else
				snprintf(outBuffer, outBufferLen, "Domain is any number");
		}
		break;
	case DVAR_TYPE_VEC2:
		Dvar_VectorDomainToString(2, domain, outBuffer, outBufferLen);
		break;
	case DVAR_TYPE_VEC3:
		Dvar_VectorDomainToString(3, domain, outBuffer, outBufferLen);
		break;
	case DVAR_TYPE_VEC4:
		Dvar_VectorDomainToString(4, domain, outBuffer, outBufferLen);
		break;
	case DVAR_TYPE_STRING:
		snprintf(outBuffer, outBufferLen, "Domain is any text");
		break;
	case DVAR_TYPE_ENUM:
		charsWritten = snprintf(outBuffer, outBufferEnd - outBuffer, "Domain is one of the following:");
		if ( charsWritten < 0 )
			break;

		outBuffer += charsWritten;
		for ( stringIndex = 0; stringIndex < domain.enumeration.stringCount; ++stringIndex )
		{
			charsWritten = snprintf(outBuffer, outBufferEnd - outBuffer, "\n  %2i: %s", stringIndex, domain.enumeration.strings[stringIndex]);
			if ( charsWritten < 0 )
				break;

			if ( outLineCount )
				++*outLineCount;

			outBuffer += charsWritten;
		}
		break;
	case DVAR_TYPE_COLOR:
		snprintf(outBuffer, outBufferLen, "Domain is any 4-component color, in RGBA format");
		break;
	default:
		*outBuffer = 0;
		break;
	}

	outBufferEnd[-1] = 0;
	return outBuffer;
}

static const char *Dvar_DomainToString(unsigned char type, DvarLimits domain, char *outBuffer, int outBufferLen)
{
	return Dvar_DomainToString_Internal(type, domain, outBuffer, outBufferLen, NULL);
}

const char *Dvar_DomainToString_GetLines(unsigned char type, DvarLimits domain, char *outBuffer, int outBufferLen, int *outLineCount)
{
	return Dvar_DomainToString_Internal(type, domain, outBuffer, outBufferLen, outLineCount);
}

void Dvar_PrintDomain(unsigned char type, DvarLimits domain)
{
	char domainBuffer[1024];

	Com_Printf("  %s\n", Dvar_DomainToString(type, domain, domainBuffer, sizeof(domainBuffer)));
}

static bool Dvar_ValuesEqual(unsigned char type, DvarValue val0, DvarValue val1)
{
	switch ( type )
	{
	case DVAR_TYPE_BOOL:
		return val0.boolean == val1.boolean;
	case DVAR_TYPE_INT:
		return val0.integer == val1.integer;
	case DVAR_TYPE_FLOAT:
		return val0.decimal == val1.decimal;
	case DVAR_TYPE_VEC2:
		return Vector2Compare(val0.vector, val1.vector);
	case DVAR_TYPE_VEC3:
		return VectorCompare(val0.vector, val1.vector);
	case DVAR_TYPE_VEC4:
		return Vector4Compare(val0.vector, val1.vector);
	case DVAR_TYPE_COLOR:
		return Byte4Compare(val0.color, val1.color);
	case DVAR_TYPE_ENUM:
		return val0.integer == val1.integer;
	case DVAR_TYPE_STRING:
		return strcmp(val0.string, val1.string) == 0;
	default:
		return false;
	}
}

void Dvar_SetVariant(dvar_t *dvar, DvarValue value, DvarSetSource source);

static void Dvar_SetLatchedValue(dvar_t *dvar, DvarValue value)
{
	switch ( dvar->type )
	{
	case DVAR_TYPE_STRING:
		Dvar_FreeLatchedString(dvar);
		Dvar_SetLatchedStringValue(dvar, value.string);
		break;
	case DVAR_TYPE_VEC2:
		Vector2Copy(value.vector, dvar->latched.vector);
		break;
	case DVAR_TYPE_VEC3:
		VectorCopy(value.vector, dvar->latched.vector);
		break;
	case DVAR_TYPE_VEC4:
		VectorCopy4(value.vector, dvar->latched.vector);
		break;
	default:
		dvar->latched = value;
		break;
	}
}

bool Dvar_HasLatchedValue(const dvar_t *dvar)
{
	return !Dvar_ValuesEqual(dvar->type, dvar->current, dvar->latched);
}

bool Dvar_IsAtDefaultValue(const dvar_t *dvar)
{
	return Dvar_ValuesEqual(dvar->type, dvar->current, dvar->reset);
}

void Dvar_SetVariant(dvar_t *dvar, DvarValue value, DvarSetSource source)
{
	Com_PrintMessage(PMSG_LOGFILE, va("      dvar set %s %s\n", dvar->name, Dvar_ValueToString(dvar, value)));

	if ( !Dvar_ValueInDomain(dvar->type, value, dvar->domain) )
	{
		Com_Printf("'%s' is not a valid value for dvar '%s'\n", Dvar_ValueToString(dvar, value), dvar->name);
		Dvar_PrintDomain(dvar->type, dvar->domain);
		if ( dvar->type == DVAR_TYPE_ENUM )
			Dvar_SetVariant(dvar, dvar->reset, source);
		return;
	}

	if ( source == DVAR_SOURCE_EXTERNAL || source == DVAR_SOURCE_SCRIPT )
	{
		if ( dvar->flags & DVAR_ROM )
		{
			Com_Printf("%s is read only.\n", dvar->name);
			return;
		}

		if ( dvar->flags & DVAR_INIT )
		{
			Com_Printf("%s is write protected.\n", dvar->name);
			return;
		}

		if ( source == DVAR_SOURCE_EXTERNAL && (dvar->flags & DVAR_CHEAT) && !dvar_cheats->current.boolean )
		{
			Com_Printf("%s is cheat protected.\n", dvar->name);
			return;
		}

		if ( dvar->flags & DVAR_LATCH )
		{
			Dvar_SetLatchedValue(dvar, value);
			if ( !Dvar_ValuesEqual(dvar->type, dvar->latched, dvar->current) )
				Com_Printf("%s will be changed upon restarting.\n", dvar->name);
			return;
		}
	}

	if ( Dvar_ValuesEqual(dvar->type, dvar->current, value) )
	{
		Dvar_SetLatchedValue(dvar, dvar->current);
		return;
	}

	dvar_modifiedFlags |= dvar->flags;

	switch ( dvar->type )
	{
	case DVAR_TYPE_STRING:
		Dvar_FreeCurrentString(dvar);
		Dvar_SetCurrentStringValue(dvar, value.string);
		Dvar_FreeLatchedString(dvar);
		dvar->latched.string = dvar->current.string;
		break;
	case DVAR_TYPE_VEC2:
		Vector2Copy(value.vector, dvar->current.vector);
		Vector2Copy(value.vector, dvar->latched.vector);
		break;
	case DVAR_TYPE_VEC3:
		VectorCopy(value.vector, dvar->current.vector);
		VectorCopy(value.vector, dvar->latched.vector);
		break;
	case DVAR_TYPE_VEC4:
		VectorCopy4(value.vector, dvar->current.vector);
		VectorCopy4(value.vector, dvar->latched.vector);
		break;
	default:
		dvar->current = value;
		dvar->latched = value;
		break;
	}

	dvar->modified = true;
}

static dvar_t *Dvar_FindMalleableVar(const char *dvarName)
{
	dvar_t *dvar;
	int hash;

	hash = generateHashValue(dvarName);

	for ( dvar = dvarHashTable[hash]; dvar; dvar = dvar->hashNext )
	{
		if ( !I_stricmp(dvarName, dvar->name) )
			return dvar;
	}

	return NULL;
}

dvar_t *Dvar_FindVar(const char *dvarName)
{
	return Dvar_FindMalleableVar(dvarName);
}

void Dvar_ClearModified(dvar_t *dvar)
{
	dvar->modified = false;
}

void Dvar_SetModified(dvar_t *dvar)
{
	dvar->modified = true;
}

void Dvar_UpdateEnumDomain(const dvar_t *dvar, const char **stringTable)
{
	int stringCount;
	dvar_t *var;

	for ( stringCount = 0; stringTable[stringCount]; stringCount++ )
		;

	var = (dvar_t *)dvar;
	var->domain.enumeration.stringCount = stringCount;
	var->domain.enumeration.strings = stringTable;
	var->current = Dvar_ClampValueToDomain(dvar->type, dvar->current, dvar->reset, dvar->domain);
	var->latched = dvar->current;
}

bool Dvar_GetBool(const char *dvarName)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);
	if ( !dvar )
		return false;

	if ( dvar->type == DVAR_TYPE_BOOL )
		return dvar->current.boolean;

	return Dvar_StringToBool(dvar->current.string);
}

int Dvar_GetInt(const char *dvarName)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);
	if ( !dvar )
		return 0;

	if ( dvar->type == DVAR_TYPE_INT || dvar->type == DVAR_TYPE_ENUM )
		return dvar->current.integer;

	return Dvar_StringToInt(dvar->current.string);
}

float Dvar_GetFloat(const char *dvarName)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);
	if ( !dvar )
		return 0;

	if ( dvar->type == DVAR_TYPE_FLOAT )
		return dvar->current.decimal;

	return Dvar_StringToFloat(dvar->current.string);
}

const float *Dvar_GetVec2(const char *dvarName)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);
	if ( !dvar )
		return vec2_origin;

	if ( dvar->type == DVAR_TYPE_VEC2 )
		return dvar->current.vector;

	return Dvar_StringToVec2(dvar->current.string);
}

const float *Dvar_GetVec3(const char *dvarName)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);
	if ( !dvar )
		return vec3_origin;

	if ( dvar->type == DVAR_TYPE_VEC3 )
		return dvar->current.vector;

	return Dvar_StringToVec3(dvar->current.string);
}

const float *Dvar_GetVec4(const char *dvarName)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);
	if ( !dvar )
		return vec4_origin;

	if ( dvar->type == DVAR_TYPE_VEC4 )
		return dvar->current.vector;

	return Dvar_StringToVec4(dvar->current.string);
}

const char *Dvar_GetString(const char *dvarName)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);
	if ( !dvar )
		return "";

	if ( dvar->type == DVAR_TYPE_ENUM )
		return Dvar_EnumToString(dvar);

	return dvar->current.string;
}

const char *Dvar_GetVariantString(const char *dvarName)
{
	dvar_t *dvar;
	const char *value;

	dvar = Dvar_FindVar(dvarName);
	if ( !dvar )
		return "";

	value = Dvar_ValueToString(dvar, dvar->current);
	return value;
}

void Dvar_GetUnpackedColor(const dvar_t *dvar, float *expandedColor)
{
	unsigned char color[4];

	if ( dvar->type == DVAR_TYPE_COLOR )
		Byte4Copy(dvar->current.color, color);
	else
		Dvar_StringToColor(dvar->current.string, color);

	expandedColor[0] = color[0] * (1.0f / 255.0f);
	expandedColor[1] = color[1] * (1.0f / 255.0f);
	expandedColor[2] = color[2] * (1.0f / 255.0f);
	expandedColor[3] = color[3] * (1.0f / 255.0f);
}

void Dvar_GetUnpackedColorByName(const char *dvarName, float *expandedColor)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);
	if ( dvar )
		Dvar_GetUnpackedColor(dvar, expandedColor);
	else
		VectorCopy4(colorWhite, expandedColor);
}

void Dvar_Shutdown()
{
	dvar_t *dvar;

	for ( dvar = sortedDvars; dvar; dvar = dvar->next )
	{
		if ( dvar->type == DVAR_TYPE_STRING )
		{
			Dvar_FreeCurrentString(dvar);
			Dvar_FreeResetString(dvar);
			Dvar_FreeLatchedString(dvar);
		}
		else if ( dvar->type == DVAR_TYPE_VEC2 || dvar->type == DVAR_TYPE_VEC3 || dvar->type == DVAR_TYPE_VEC4 )
		{
			Dvar_FreeVectors(dvar);
		}

		if ( dvar->flags & DVAR_EXTERNAL )
			Dvar_FreeNameString(dvar->name);
	}

	dvarCount = 0;
	sortedDvars = NULL;
	dvar_cheats = NULL;
	dvar_modifiedFlags = 0;
	isDvarSystemActive = false;
	memset(dvarHashTable, 0, sizeof(dvarHashTable));
}

// turns a registered dvar back into an external string dvar
static void Dvar_PerformUnregistration(dvar_t *dvar)
{
	float *vector;

	if ( !(dvar->flags & DVAR_EXTERNAL) )
	{
		dvar->flags |= DVAR_EXTERNAL;
		dvar->name = Dvar_AllocNameString(dvar->name);
	}

	if ( dvar->type == DVAR_TYPE_STRING )
		return;

	if ( dvar->type == DVAR_TYPE_VEC2 || dvar->type == DVAR_TYPE_VEC3 || dvar->type == DVAR_TYPE_VEC4 )
		vector = dvar->current.vector;
	else
		vector = NULL;

	dvar->current.string = Dvar_InternString(Dvar_DisplayableLatchedValue(dvar));
	dvar->latched.string = dvar->current.string;
	Dvar_SetResetStringValue(dvar, Dvar_DisplayableResetValue(dvar));
	dvar->type = DVAR_TYPE_STRING;

	if ( vector )
		Z_Free(vector);
}

void Dvar_ClearFlags(const dvar_t *dvar, int flags)
{
	dvar_t *var;

	if ( dvar->flags & DVAR_EXTERNAL )
		return;

	var = (dvar_t *)dvar;
	var->flags &= ~flags;
	if ( var->flags & (DVAR_CHANGEABLE_RESET | 0x2000 | DVAR_EXTERNAL) )
		return;

	Dvar_PerformUnregistration(var);
}

void Dvar_ClearFlagsFromAll(int flags)
{
	dvar_t *dvar;

	for ( dvar = sortedDvars; dvar; dvar = dvar->next )
	{
		if ( dvar->flags & flags )
			Dvar_ClearFlags(dvar, flags);
	}
}

static void Dvar_UpdateResetValue(dvar_t *dvar, DvarValue value)
{
	switch ( dvar->type )
	{
	case DVAR_TYPE_STRING:
		Dvar_FreeResetString(dvar);
		Dvar_SetResetStringValue(dvar, value.string);
		break;
	case DVAR_TYPE_VEC2:
		Vector2Copy(value.vector, dvar->reset.vector);
		break;
	case DVAR_TYPE_VEC3:
		VectorCopy(value.vector, dvar->reset.vector);
		break;
	case DVAR_TYPE_VEC4:
		VectorCopy4(value.vector, dvar->reset.vector);
		break;
	default:
		dvar->reset = value;
		break;
	}
}

void Dvar_ChangeResetValue(dvar_t *dvar, DvarValue value)
{
	Dvar_UpdateResetValue(dvar, value);
}

static void Dvar_UpdateValue(dvar_t *dvar, DvarValue value)
{
	switch ( dvar->type )
	{
	case DVAR_TYPE_STRING:
		if ( value.string != dvar->current.string )
		{
			Dvar_FreeCurrentString(dvar);
			Dvar_SetCurrentStringValue(dvar, value.string);
		}
		dvar->latched.string = value.string;
		break;
	case DVAR_TYPE_VEC2:
		Vector2Copy(value.vector, dvar->current.vector);
		Vector2Copy(value.vector, dvar->latched.vector);
		break;
	case DVAR_TYPE_VEC3:
		VectorCopy(value.vector, dvar->current.vector);
		VectorCopy(value.vector, dvar->latched.vector);
		break;
	case DVAR_TYPE_VEC4:
		VectorCopy4(value.vector, dvar->current.vector);
		VectorCopy4(value.vector, dvar->latched.vector);
		break;
	default:
		dvar->current = value;
		dvar->latched = value;
		break;
	}
}

static void Dvar_MakeExplicitType(dvar_t *dvar, const char *dvarName, unsigned char type, unsigned short flags, DvarValue resetValue, DvarLimits domain)
{
	DvarValue castValue;

	dvar->type = type;
	dvar->domain = domain;
	if ( (flags & DVAR_ROM) || ((flags & DVAR_CHEAT) && dvar_cheats && !dvar_cheats->current.boolean) )
	{
		castValue = resetValue;
	}
	else
	{
		castValue = Dvar_StringToValue(dvar->type, dvar->domain, dvar->current.string);
		castValue = Dvar_ClampValueToDomain(type, castValue, resetValue, domain);
	}

	if ( dvar->type != DVAR_TYPE_STRING )
		Dvar_FreeCurrentString(dvar);

	Dvar_FreeLatchedString(dvar);
	Dvar_FreeResetString(dvar);

	if ( dvar->type == DVAR_TYPE_VEC2 || dvar->type == DVAR_TYPE_VEC3 || dvar->type == DVAR_TYPE_VEC4 )
		Dvar_AllocVectors(dvar);

	Dvar_UpdateResetValue(dvar, resetValue);
	Dvar_UpdateValue(dvar, castValue);

	dvar_modifiedFlags |= flags;
}

static DvarValue Dvar_GetReinterpretedResetValue(dvar_t *dvar, DvarValue value, unsigned char type, unsigned short flags, DvarLimits domain)
{
	return value;
}

static void Dvar_ReinterpretDvar(dvar_t *dvar, const char *dvarName, unsigned char type, unsigned short flags, DvarValue value, DvarLimits domain)
{
	DvarValue resetValue;

	if ( (dvar->flags & DVAR_EXTERNAL) && !(flags & DVAR_EXTERNAL) )
	{
		resetValue = Dvar_GetReinterpretedResetValue(dvar, value, type, flags, domain);
		Dvar_PerformUnregistration(dvar);
		Dvar_FreeNameString(dvar->name);
		dvar->name = dvarName;
		dvar->flags &= ~DVAR_EXTERNAL;
		Dvar_MakeExplicitType(dvar, dvarName, type, flags, resetValue, domain);
	}
}

static void Dvar_UpdateRegistration(dvar_t *dvar, const char *dvarName, unsigned char type, unsigned short flags, DvarValue resetValue, DvarLimits domain)
{
	if ( flags & DVAR_EXTERNAL )
		return;

	if ( (flags & DVAR_CHANGEABLE_RESET) && !(dvar->flags & DVAR_CHANGEABLE_RESET) )
	{
		dvar->name = dvarName;
		if ( dvar->type == DVAR_TYPE_ENUM )
			dvar->domain = domain;
	}
}

static void Dvar_MakeLatchedValueCurrent(dvar_t *dvar)
{
	Dvar_SetVariant(dvar, dvar->latched, DVAR_SOURCE_INTERNAL);
}

void Dvar_ClearLatchedValue(dvar_t *dvar)
{
	if ( !Dvar_HasLatchedValue(dvar) )
		return;

	Dvar_SetLatchedValue(dvar, dvar->current);
}

static void Dvar_Reregister(dvar_t *dvar, const char *dvarName, unsigned char type, unsigned short flags, DvarValue resetValue, DvarLimits domain)
{
	if ( (dvar->flags ^ flags) & (DVAR_CHANGEABLE_RESET | 0x2000 | DVAR_EXTERNAL) )
	{
		Dvar_ReinterpretDvar(dvar, dvarName, type, flags, resetValue, domain);
		Dvar_UpdateRegistration(dvar, dvarName, type, flags, resetValue, domain);
	}

	if ( dvar->flags & DVAR_EXTERNAL )
	{
		if ( dvar->type != type )
			Dvar_MakeExplicitType(dvar, dvarName, type, flags, resetValue, domain);
	}

	dvar->flags |= flags;
	if ( (dvar->flags & DVAR_CHEAT) && dvar_cheats && !dvar_cheats->current.boolean )
	{
		Dvar_SetVariant(dvar, dvar->reset, DVAR_SOURCE_INTERNAL);
		Dvar_SetLatchedValue(dvar, dvar->reset);
	}

	if ( dvar->flags & DVAR_LATCH )
		Dvar_MakeLatchedValueCurrent(dvar);
}

static dvar_t *Dvar_RegisterNew(const char *dvarName, unsigned char type, unsigned short flags, DvarValue value, DvarLimits domain)
{
	dvar_t *dvar;
	dvar_t **sorted;
	int hash;

	if ( dvarCount >= MAX_DVARS )
		Com_Error(ERR_FATAL, "Can't create dvar '%s': %i dvars already exist", dvarName, MAX_DVARS);

	dvar = &dvarPool[dvarCount];
	dvarCount++;
	dvar->type = type;

	if ( flags & DVAR_EXTERNAL )
		dvar->name = Dvar_AllocNameString(dvarName);
	else
		dvar->name = dvarName;

	switch ( type )
	{
	case DVAR_TYPE_STRING:
		dvar->current.string = Dvar_InternString(value.string);
		dvar->latched.string = dvar->current.string;
		dvar->reset.string = dvar->current.string;
		break;
	case DVAR_TYPE_VEC2:
		Dvar_AllocVectors(dvar);
		Vector2Copy(value.vector, dvar->current.vector);
		Vector2Copy(value.vector, dvar->latched.vector);
		Vector2Copy(value.vector, dvar->reset.vector);
		break;
	case DVAR_TYPE_VEC3:
		Dvar_AllocVectors(dvar);
		VectorCopy(value.vector, dvar->current.vector);
		VectorCopy(value.vector, dvar->latched.vector);
		VectorCopy(value.vector, dvar->reset.vector);
		break;
	case DVAR_TYPE_VEC4:
		Dvar_AllocVectors(dvar);
		VectorCopy4(value.vector, dvar->current.vector);
		VectorCopy4(value.vector, dvar->latched.vector);
		VectorCopy4(value.vector, dvar->reset.vector);
		break;
	default:
		dvar->current = value;
		dvar->latched = value;
		dvar->reset = value;
		break;
	}

	dvar->domain = domain;
	dvar->modified = false;

	for ( sorted = &sortedDvars; *sorted; sorted = &(*sorted)->next )
	{
		if ( strcasecmp(dvar->name, (*sorted)->name) < 0 )
			break;
	}
	dvar->next = *sorted;
	*sorted = dvar;

	dvar->flags = flags;
	hash = generateHashValue(dvarName);
	dvar->hashNext = dvarHashTable[hash];
	dvarHashTable[hash] = dvar;

	return dvar;
}

// Retail expands this into every Dvar_Register* wrapper.
static __forceinline dvar_t *Dvar_RegisterVariant(const char *dvarName, unsigned char type, unsigned short flags, DvarValue value, DvarLimits domain)
{
	dvar_t *dvar;

	dvar = Dvar_FindMalleableVar(dvarName);
	if ( dvar )
	{
		Dvar_Reregister(dvar, dvarName, type, flags, value, domain);
		return dvar;
	}

	return Dvar_RegisterNew(dvarName, type, flags, value, domain);
}

dvar_t *Dvar_RegisterBool(const char *dvarName, bool value, unsigned short flags)
{
	DvarValue dvarValue;
	DvarLimits dvarDomain;

	dvarValue.boolean = value;
	memset(&dvarDomain, 0, sizeof(dvarDomain));
	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_BOOL, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterInt(const char *dvarName, int value, int min, int max, unsigned short flags)
{
	DvarValue dvarValue;
	DvarLimits dvarDomain;

	dvarValue.integer = value;
	dvarDomain.integer.min = min;
	dvarDomain.integer.max = max;
	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_INT, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterFloat(const char *dvarName, float value, float min, float max, unsigned short flags)
{
	DvarValue dvarValue;
	DvarLimits dvarDomain;

	dvarValue.decimal = value;
	dvarDomain.decimal.min = min;
	dvarDomain.decimal.max = max;
	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_FLOAT, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterVec2(const char *dvarName, float x, float y, float min, float max, unsigned short flags)
{
	DvarValue dvarValue;
	DvarLimits dvarDomain;
	vec2_t vector;

	Vector2Set(vector, x, y);
	dvarValue.vector = vector;
	dvarDomain.decimal.min = min;
	dvarDomain.decimal.max = max;
	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_VEC2, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterVec3(const char *dvarName, float x, float y, float z, float min, float max, unsigned short flags)
{
	DvarValue dvarValue;
	DvarLimits dvarDomain;
	vec3_t vector;

	VectorSet(vector, x, y, z);
	dvarValue.vector = vector;
	dvarDomain.decimal.min = min;
	dvarDomain.decimal.max = max;
	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_VEC3, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterVec4(const char *dvarName, float x, float y, float z, float w, float min, float max, unsigned short flags)
{
	DvarValue dvarValue;
	DvarLimits dvarDomain;
	vec4_t vector;

	Vector4Set(vector, x, y, z, w);
	dvarValue.vector = vector;
	dvarDomain.decimal.min = min;
	dvarDomain.decimal.max = max;
	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_VEC4, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterString(const char *dvarName, const char *value, unsigned short flags)
{
	DvarValue dvarValue;
	DvarLimits dvarDomain;

	dvarValue.string = value;
	memset(&dvarDomain, 0, sizeof(dvarDomain));
	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_STRING, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterEnum(const char *dvarName, const char **valueList, int defaultIndex, unsigned short flags)
{
	DvarValue dvarValue;
	DvarLimits dvarDomain;

	dvarValue.integer = defaultIndex;
	dvarDomain.enumeration.strings = valueList;
	for ( dvarDomain.enumeration.stringCount = 0; valueList[dvarDomain.enumeration.stringCount]; dvarDomain.enumeration.stringCount++ )
		;
	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_ENUM, flags, dvarValue, dvarDomain);
}

dvar_t *Dvar_RegisterColor(const char *dvarName, float r, float g, float b, float a, unsigned short flags)
{
	DvarValue dvarValue;
	DvarLimits dvarDomain;

	dvarValue.color[0] = I_fround(I_fmax(0.0f, I_fmin(1.0f, r)) * 255.0f);
	dvarValue.color[1] = I_fround(I_fmax(0.0f, I_fmin(1.0f, g)) * 255.0f);
	dvarValue.color[2] = I_fround(I_fmax(0.0f, I_fmin(1.0f, b)) * 255.0f);
	dvarValue.color[3] = I_fround(I_fmax(0.0f, I_fmin(1.0f, a)) * 255.0f);
	memset(&dvarDomain, 0, sizeof(dvarDomain));
	return Dvar_RegisterVariant(dvarName, DVAR_TYPE_COLOR, flags, dvarValue, dvarDomain);
}

void Dvar_SetBoolFromSource(dvar_t *dvar, bool value, DvarSetSource source)
{
	DvarValue newValue;

	if ( dvar->type == DVAR_TYPE_BOOL )
		newValue.boolean = value;
	else
		newValue.string = value ? "1" : "0";

	Dvar_SetVariant(dvar, newValue, source);
}

void Dvar_SetIntFromSource(dvar_t *dvar, int value, DvarSetSource source)
{
	DvarValue newValue;
	char string[32];

	if ( dvar->type == DVAR_TYPE_INT || dvar->type == DVAR_TYPE_ENUM )
	{
		newValue.integer = value;
	}
	else
	{
		Com_sprintf(string, sizeof(string), "%i", value);
		newValue.string = string;
	}

	Dvar_SetVariant(dvar, newValue, source);
}

void Dvar_SetFloatFromSource(dvar_t *dvar, float value, DvarSetSource source)
{
	DvarValue newValue;
	char string[32];

	if ( dvar->type == DVAR_TYPE_FLOAT )
	{
		newValue.decimal = value;
	}
	else
	{
		Com_sprintf(string, sizeof(string), "%g", value);
		newValue.string = string;
	}

	Dvar_SetVariant(dvar, newValue, source);
}

// tests the vec4 type, so a vec2 dvar always takes the string path
void Dvar_SetVec2FromSource(dvar_t *dvar, float x, float y, DvarSetSource source)
{
	DvarValue newValue;
	vec2_t vector;
	char string[64];

	if ( dvar->type == DVAR_TYPE_VEC4 )
	{
		Vector2Set(vector, x, y);
		newValue.vector = vector;
	}
	else
	{
		Com_sprintf(string, sizeof(string), "%g %g", x, y);
		newValue.string = string;
	}

	Dvar_SetVariant(dvar, newValue, source);
}

void Dvar_SetVec3FromSource(dvar_t *dvar, float x, float y, float z, DvarSetSource source)
{
	DvarValue newValue;
	vec3_t vector;
	char string[96];

	if ( dvar->type == DVAR_TYPE_VEC3 )
	{
		VectorSet(vector, x, y, z);
		newValue.vector = vector;
	}
	else
	{
		Com_sprintf(string, sizeof(string), "%g %g %g", x, y, z);
		newValue.string = string;
	}

	Dvar_SetVariant(dvar, newValue, source);
}

void Dvar_SetVec4FromSource(dvar_t *dvar, float x, float y, float z, float w, DvarSetSource source)
{
	DvarValue newValue;
	vec4_t vector;
	char string[128];

	if ( dvar->type == DVAR_TYPE_VEC4 )
	{
		Vector4Set(vector, x, y, z, w);
		newValue.vector = vector;
	}
	else
	{
		Com_sprintf(string, sizeof(string), "%g %g %g %g", x, y, z, w);
		newValue.string = string;
	}

	Dvar_SetVariant(dvar, newValue, source);
}

void Dvar_SetStringFromSource(dvar_t *dvar, const char *string, DvarSetSource source)
{
	DvarValue newValue;
	char stringCopy[1024];

	if ( dvar->type == DVAR_TYPE_STRING )
	{
		I_strncpyz(stringCopy, string, sizeof(stringCopy));
		newValue.string = stringCopy;
	}
	else
	{
		newValue.integer = Dvar_StringToEnum(&dvar->domain, string);
	}

	Dvar_SetVariant(dvar, newValue, source);
}

void Dvar_SetColorFromSource(dvar_t *dvar, float r, float g, float b, float a, DvarSetSource source)
{
	DvarValue newValue;
	char string[128];

	if ( dvar->type == DVAR_TYPE_COLOR )
	{
		newValue.color[0] = I_fround(I_fmax(0.0f, I_fmin(1.0f, r)) * 255.0f);
		newValue.color[1] = I_fround(I_fmax(0.0f, I_fmin(1.0f, g)) * 255.0f);
		newValue.color[2] = I_fround(I_fmax(0.0f, I_fmin(1.0f, b)) * 255.0f);
		newValue.color[3] = I_fround(I_fmax(0.0f, I_fmin(1.0f, a)) * 255.0f);
	}
	else
	{
		Com_sprintf(string, sizeof(string), "%g %g %g %g", r, g, b, a);
		newValue.string = string;
	}

	Dvar_SetVariant(dvar, newValue, source);
}

void Dvar_SetBool(dvar_t *dvar, bool value)
{
	Dvar_SetBoolFromSource(dvar, value, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetInt(dvar_t *dvar, int value)
{
	Dvar_SetIntFromSource(dvar, value, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetFloat(dvar_t *dvar, float value)
{
	Dvar_SetFloatFromSource(dvar, value, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetVec2(dvar_t *dvar, float x, float y)
{
	Dvar_SetVec2FromSource(dvar, x, y, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetVec3(dvar_t *dvar, float x, float y, float z)
{
	Dvar_SetVec3FromSource(dvar, x, y, z, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetVec4(dvar_t *dvar, float x, float y, float z, float w)
{
	Dvar_SetVec4FromSource(dvar, x, y, z, w, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetString(dvar_t *dvar, const char *value)
{
	Dvar_SetStringFromSource(dvar, value, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetColor(dvar_t *dvar, float r, float g, float b, float a)
{
	Dvar_SetColorFromSource(dvar, r, g, b, a, DVAR_SOURCE_INTERNAL);
}

static void Dvar_SetFromStringFromSource(dvar_t *dvar, const char *string, DvarSetSource source)
{
	DvarValue newValue;
	char buf[1024];

	I_strncpyz(buf, string, sizeof(buf));
	newValue = Dvar_StringToValue(dvar->type, dvar->domain, buf);
	if ( dvar->type == DVAR_TYPE_ENUM && newValue.integer == DVAR_INVALID_ENUM_INDEX )
	{
		Com_Printf("'%s' is not a valid value for dvar '%s'\n", buf, dvar->name);
		Dvar_PrintDomain(dvar->type, dvar->domain);
		newValue = dvar->reset;
	}

	Dvar_SetVariant(dvar, newValue, source);
}

void Dvar_SetFromString(dvar_t *dvar, const char *string)
{
	Dvar_SetFromStringFromSource(dvar, string, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetBoolByName(const char *dvarName, bool value)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);
	if ( dvar )
		Dvar_SetBool(dvar, value);
	else
		Dvar_RegisterString(dvarName, value ? "1" : "0", DVAR_EXTERNAL);
}

void Dvar_SetIntByName(const char *dvarName, int value)
{
	dvar_t *dvar;
	char valueString[32];

	dvar = Dvar_FindVar(dvarName);
	if ( dvar )
	{
		Dvar_SetInt(dvar, value);
	}
	else
	{
		Com_sprintf(valueString, sizeof(valueString), "%i", value);
		Dvar_RegisterString(dvarName, valueString, DVAR_EXTERNAL);
	}
}

void Dvar_SetFloatByName(const char *dvarName, float value)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);
	if ( dvar )
		Dvar_SetFloat(dvar, value);
	else
		Dvar_RegisterString(dvarName, va("%g", value), DVAR_EXTERNAL);
}

void Dvar_SetVec2ByName(const char *dvarName, float x, float y)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);
	if ( dvar )
		Dvar_SetVec2(dvar, x, y);
	else
		Dvar_RegisterString(dvarName, va("%g %g", x, y), DVAR_EXTERNAL);
}

void Dvar_SetVec3ByName(const char *dvarName, float x, float y, float z)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);
	if ( dvar )
		Dvar_SetVec3(dvar, x, y, z);
	else
		Dvar_RegisterString(dvarName, va("%g %g %g", x, y, z), DVAR_EXTERNAL);
}

void Dvar_SetVec4ByName(const char *dvarName, float x, float y, float z, float w)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);
	if ( dvar )
		Dvar_SetVec4(dvar, x, y, z, w);
	else
		Dvar_RegisterString(dvarName, va("%g %g %g %g", x, y, z, w), DVAR_EXTERNAL);
}

void Dvar_SetStringByName(const char *dvarName, const char *value)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);
	if ( dvar )
		Dvar_SetString(dvar, value);
	else
		Dvar_RegisterString(dvarName, value, DVAR_EXTERNAL);
}

// the fallback passes byte components to a %g format unconverted
void Dvar_SetColorByName(const char *dvarName, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);
	if ( dvar )
		Dvar_SetColor(dvar, r, g, b, a);
	else
		Dvar_RegisterString(dvarName, va("%g %g %g %g", r, g, b, a), DVAR_EXTERNAL);
}

dvar_t *Dvar_SetFromStringByNameFromSource(const char *dvarName, const char *string, DvarSetSource source)
{
	dvar_t *dvar;

	dvar = Dvar_FindVar(dvarName);
	if ( !dvar )
		dvar = Dvar_RegisterString(dvarName, string, DVAR_EXTERNAL);
	else
		Dvar_SetFromStringFromSource(dvar, string, source);

	return dvar;
}

void Dvar_SetFromStringByName(const char *dvarName, const char *string)
{
	Dvar_SetFromStringByNameFromSource(dvarName, string, DVAR_SOURCE_INTERNAL);
}

void Dvar_SetCommand(const char *dvarName, const char *string)
{
	dvar_t *dvar;

	dvar = Dvar_SetFromStringByNameFromSource(dvarName, string, DVAR_SOURCE_EXTERNAL);
	if ( dvar && isLoadingAutoExec )
	{
		Dvar_AddFlags(dvar, DVAR_AUTOEXEC);
		Dvar_UpdateResetValue(dvar, dvar->current);
	}
}

void Dvar_AddFlags(dvar_t *dvar, int flags)
{
	dvar->flags |= flags;
}

void Dvar_Reset(dvar_t *dvar, DvarSetSource setSource)
{
	Dvar_SetVariant(dvar, dvar->reset, setSource);
}

void Dvar_SetCheatsToReset()
{
	dvar_t *dvar;

	for ( dvar = sortedDvars; dvar; dvar = dvar->next )
	{
		if ( dvar->flags & DVAR_CHEAT )
			Dvar_SetVariant(dvar, dvar->reset, DVAR_SOURCE_INTERNAL);
	}
}

void Dvar_Init()
{
	isDvarSystemActive = true;
	dvar_cheats = Dvar_RegisterBool("sv_cheats", false, DVAR_CHANGEABLE_RESET | DVAR_INIT | DVAR_SYSTEMINFO);
	Dvar_AddCommands();
}

// walks the registration chain from the first pool entry
void Dvar_ClearServerInfoNoUpdateFlags()
{
	dvar_t *dvar;

	for ( dvar = dvarPool; dvar; dvar = dvar->next )
		dvar->flags &= ~DVAR_SERVERINFO_NOUPDATE;
}

__declspec(noinline) bool Dvar_AnyLatchedValues()
{
	dvar_t *dvar;

	for ( dvar = sortedDvars; dvar; dvar = dvar->next )
	{
		if ( Dvar_HasLatchedValue(dvar) )
			return true;
	}

	return false;
}

// packs "name\0value\0" pairs; a NULL buffer only measures
int Dvar_SaveDvars(char *buffer, int flags)
{
	dvar_t *dvar;
	const char *value;
	unsigned int hash;
	int size;

	size = 0;
	for ( hash = 0; hash < FILE_HASH_SIZE; hash++ )
	{
		for ( dvar = dvarHashTable[hash]; dvar; dvar = dvar->hashNext )
		{
			if ( !(dvar->flags & flags) )
				continue;

			if ( buffer )
				strcpy(&buffer[size], dvar->name);
			size += I_strlen(dvar->name) + 1;

			value = Dvar_DisplayableValue(dvar);
			if ( buffer )
				strcpy(&buffer[size], value);
			size += I_strlen(value) + 1;
		}
	}

	return size;
}

void Dvar_LoadDvarsFromBuffer(const char *buffer, int size)
{
	int i;
	const char *dvarName;
	const char *string;
	DvarValue value;
	dvar_t *dvar;

	for ( i = 0; i < size; )
	{
		dvarName = &buffer[i];
		i += I_strlen(dvarName) + 1;
		string = &buffer[i];
		i += I_strlen(string) + 1;

		dvar = Dvar_FindMalleableVar(dvarName);
		if ( dvar )
		{
			value = Dvar_StringToValue(dvar->type, dvar->domain, string);
			Dvar_SetVariant(dvar, value, DVAR_SOURCE_INTERNAL);
		}
		else
		{
			Dvar_RegisterString(dvarName, string, DVAR_EXTERNAL);
		}
	}
}

void Dvar_ResetDvars(int filter, DvarSetSource setSource)
{
	dvar_t *dvar;

	for ( dvar = sortedDvars; dvar; dvar = dvar->next )
	{
		if ( filter & dvar->flags )
			Dvar_Reset(dvar, setSource);
	}
}

qboolean Com_SaveDvarsToBuffer(const char **dvarnames, int numDvars, char *buffer, int bufsize)
{
	int i;
	int written;
	dvar_t *dvar;
	const char *value;

	for ( i = 0; i < numDvars; i++ )
	{
		dvar = Dvar_FindVar(dvarnames[i]);
		value = Dvar_DisplayableValue(dvar);
		written = snprintf(buffer, bufsize, "%s \"%s\"\n", dvar->name, value);
		if ( written < 0 )
			return qfalse;

		buffer += written;
		bufsize -= written;
	}

	return qtrue;
}

qboolean Com_LoadDvarsFromBuffer(const char **dvarnames, int numDvars, const char *buffer, const char *filename)
{
	int i;
	const char *token;
	dvar_t *dvar;
	int numRead;
	char wasRead[16384];

	memset(wasRead, 0, numDvars);
	numRead = 0;

	for ( i = 0; i < numDvars; i++ )
	{
		dvar = Dvar_FindVar(dvarnames[i]);
		Dvar_Reset(dvar, DVAR_SOURCE_INTERNAL);
	}

	Com_BeginParseSession(filename);

	while ( 1 )
	{
		token = Com_Parse(&buffer);
		if ( !*token )
			break;

		for ( i = 0; i < numDvars; i++ )
		{
			if ( !strcasecmp(token, dvarnames[i]) )
			{
				dvar = Dvar_FindVar(dvarnames[i]);
				token = Com_ParseOnLine(&buffer);
				Dvar_SetFromString(dvar, token);
				if ( !wasRead[i] )
				{
					wasRead[i] = 1;
					numRead++;
				}
				goto nextLine;
			}
		}

		Com_Printf("^3WARNING: unknown dvar '%s' in file '%s'\n", token, filename);
nextLine:
		Com_SkipRestOfLine(&buffer);
	}

	Com_EndParseSessionInl();

	if ( numRead == numDvars )
		return qtrue;

	Com_Printf("^1ERROR: the following dvars were not specified in file '%s'\n", filename);
	for ( i = 0; i < numDvars; i++ )
	{
		if ( !wasRead[i] )
			Com_Printf("^1  %s\n", dvarnames[i]);
	}

	return qfalse;
}

