// Windows mixer helpers for voice chat: record source, record level, mic mute.
#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <string.h>

int mixerGetRecordLevel( const char *source )
{
	HMIXER hMixer;
	MIXERLINEA line;
	MIXERLINECONTROLSA lineControls;
	MIXERCONTROLA control;
	MIXERCONTROLDETAILS details;
	MIXERCONTROLDETAILS_UNSIGNED value;
	DWORD i;
	DWORD connections;

	if ( waveInGetNumDevs() < 1 || mixerGetNumDevs() < 1 )
		return -1;

	if ( mixerOpen( &hMixer, 0, 0, 0, 0 ) )
		return -1;

	line.cbStruct = sizeof( line );
	line.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
	mixerGetLineInfoA( (HMIXEROBJ)hMixer, &line, MIXER_GETLINEINFOF_COMPONENTTYPE );

	connections = line.cConnections;
	for ( i = 0; i < connections; i++ )
	{
		line.dwSource = i;
		mixerGetLineInfoA( (HMIXEROBJ)hMixer, &line, MIXER_GETLINEINFOF_SOURCE );
		if ( !strstr( line.szName, source ) )
			continue;

		lineControls.cbStruct = sizeof( lineControls );
		lineControls.dwLineID = line.dwLineID;
		lineControls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
		lineControls.cControls = 1;
		lineControls.cbmxctrl = sizeof( control );
		lineControls.pamxctrl = &control;
		if ( mixerGetLineControlsA( (HMIXEROBJ)hMixer, &lineControls, MIXER_GETLINECONTROLSF_ONEBYTYPE ) )
			break;

		details.paDetails = &value;
		details.cbStruct = sizeof( details );
		details.cChannels = 1;
		details.cbDetails = sizeof( value );
		details.dwControlID = control.dwControlID;
		details.cMultipleItems = 0;
		mixerGetControlDetailsA( (HMIXEROBJ)hMixer, &details, MIXER_GETCONTROLDETAILSF_VALUE );
		mixerClose( hMixer );
		return value.dwValue;
	}

	mixerClose( hMixer );
	return -1;
}

int mixerSetRecordLevel( const char *source, unsigned short level )
{
	HMIXER hMixer;
	MIXERLINEA line;
	MIXERLINECONTROLSA lineControls;
	MIXERCONTROLA control;
	MIXERCONTROLDETAILS details;
	MIXERCONTROLDETAILS_UNSIGNED value;
	DWORD i;
	DWORD connections;

	if ( waveInGetNumDevs() < 1 || mixerGetNumDevs() < 1 )
		return 0;

	if ( mixerOpen( &hMixer, 0, 0, 0, 0 ) )
		return 0;

	line.cbStruct = sizeof( line );
	line.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
	mixerGetLineInfoA( (HMIXEROBJ)hMixer, &line, MIXER_GETLINEINFOF_COMPONENTTYPE );

	connections = line.cConnections;
	for ( i = 0; i < connections; i++ )
	{
		line.dwSource = i;
		mixerGetLineInfoA( (HMIXEROBJ)hMixer, &line, MIXER_GETLINEINFOF_SOURCE );
		if ( !strstr( line.szName, source ) )
			continue;

		lineControls.cbStruct = sizeof( lineControls );
		lineControls.dwLineID = line.dwLineID;
		lineControls.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
		lineControls.cControls = 1;
		lineControls.cbmxctrl = sizeof( control );
		lineControls.pamxctrl = &control;
		if ( mixerGetLineControlsA( (HMIXEROBJ)hMixer, &lineControls, MIXER_GETLINECONTROLSF_ONEBYTYPE ) )
			break;

		details.paDetails = &value;
		details.cbStruct = sizeof( details );
		details.cChannels = 1;
		details.cbDetails = sizeof( value );
		details.dwControlID = control.dwControlID;
		details.cMultipleItems = 0;
		mixerGetControlDetailsA( (HMIXEROBJ)hMixer, &details, MIXER_GETCONTROLDETAILSF_VALUE );
		value.dwValue = level;
		mixerSetControlDetails( (HMIXEROBJ)hMixer, &details, MIXER_SETCONTROLDETAILSF_VALUE );
		mixerClose( hMixer );
		return 0;
	}

	mixerClose( hMixer );
	return 0;
}

// Copies the name of the selected wave-in source; the mixer handle is never closed.
int mixerGetRecordSource( char *source )
{
	HMIXER hMixer;
	MIXERLINEA line;
	MIXERLINECONTROLSA lineControls;
	MIXERCONTROLDETAILS details;
	MIXERCONTROLA *controls;
	MIXERCONTROLDETAILS_LISTTEXTA *listText;
	MIXERCONTROLDETAILS_BOOLEAN *selected;
	DWORD channels;
	DWORD items;
	int i;
	int j;

	if ( waveInGetNumDevs() < 1 )
		return 0;
	if ( mixerGetNumDevs() < 1 )
		return 0;

	listText = NULL;
	if ( mixerOpen( &hMixer, 0, 0, 0, 0 ) )
		return 0;

	selected = NULL;
	line.cbStruct = sizeof( line );
	line.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
	mixerGetLineInfoA( (HMIXEROBJ)hMixer, &line, MIXER_GETLINEINFOF_COMPONENTTYPE );

	controls = (MIXERCONTROLA *)calloc( line.cControls * sizeof( MIXERCONTROLA ), 1 );
	lineControls.cbStruct = sizeof( lineControls );
	lineControls.dwLineID = line.dwLineID;
	lineControls.dwControlID = 0;
	lineControls.cControls = line.cControls;
	lineControls.cbmxctrl = sizeof( MIXERCONTROLA );
	lineControls.pamxctrl = controls;

	if ( !mixerGetLineControlsA( (HMIXEROBJ)hMixer, &lineControls, MIXER_GETLINECONTROLSF_ALL ) )
	{
		for ( i = 0; i < (int)line.cControls; i++ )
		{
			if ( ( controls[i].dwControlType & MIXERCONTROL_CT_CLASS_MASK ) != MIXERCONTROL_CT_CLASS_LIST )
				continue;

			channels = line.cChannels;
			items = 0;
			if ( controls[i].fdwControl & MIXERCONTROL_CONTROLF_UNIFORM )
				channels = 1;
			if ( controls[i].fdwControl & MIXERCONTROL_CONTROLF_MULTIPLE )
				items = controls[i].cMultipleItems;

			listText = (MIXERCONTROLDETAILS_LISTTEXTA *)calloc( items * channels * sizeof( MIXERCONTROLDETAILS_LISTTEXTA ), 1 );
			details.cbStruct = sizeof( details );
			details.dwControlID = controls[i].dwControlID;
			details.cChannels = channels;
			details.cMultipleItems = items;
			details.cbDetails = sizeof( MIXERCONTROLDETAILS_LISTTEXTA );
			details.paDetails = listText;
			if ( mixerGetControlDetailsA( (HMIXEROBJ)hMixer, &details, MIXER_GETCONTROLDETAILSF_LISTTEXT ) )
				continue;

			selected = (MIXERCONTROLDETAILS_BOOLEAN *)calloc( items * channels * sizeof( MIXERCONTROLDETAILS_BOOLEAN ), 1 );
			details.cbDetails = sizeof( MIXERCONTROLDETAILS_BOOLEAN );
			details.paDetails = selected;
			if ( mixerGetControlDetailsA( (HMIXEROBJ)hMixer, &details, MIXER_GETCONTROLDETAILSF_VALUE ) )
				continue;

			for ( j = 0; j < (int)items; j++ )
			{
				if ( selected[j].fValue == 1 )
					strncpy( source, listText[j].szName, 255 );
			}

			if ( controls )
				free( controls );
			if ( listText )
				free( listText );
			if ( selected )
				free( selected );
			return 1;
		}
	}

	if ( controls )
		free( controls );
	if ( listText )
		free( listText );
	if ( selected )
		free( selected );
	return 0;
}

// Selects every wave-in source whose name contains 'source'; the mixer handle is never closed.
int mixerSetRecordSource( const char *source )
{
	HMIXER hMixer;
	MIXERLINEA line;
	MIXERLINECONTROLSA lineControls;
	MIXERCONTROLDETAILS details;
	MIXERCONTROLA *controls;
	MIXERCONTROLDETAILS_LISTTEXTA *listText;
	MIXERCONTROLDETAILS_BOOLEAN *selected;
	MMRESULT result;
	DWORD channels;
	DWORD items;
	int i;
	int j;

	if ( waveInGetNumDevs() < 1 )
		return 0;
	if ( mixerGetNumDevs() < 1 )
		return 0;

	listText = NULL;
	if ( mixerOpen( &hMixer, 0, 0, 0, 0 ) )
		return 0;

	selected = NULL;
	line.cbStruct = sizeof( line );
	line.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
	mixerGetLineInfoA( (HMIXEROBJ)hMixer, &line, MIXER_GETLINEINFOF_COMPONENTTYPE );

	controls = (MIXERCONTROLA *)calloc( line.cControls * sizeof( MIXERCONTROLA ), 1 );
	lineControls.cbStruct = sizeof( lineControls );
	lineControls.dwLineID = line.dwLineID;
	lineControls.dwControlID = 0;
	lineControls.cControls = line.cControls;
	lineControls.cbmxctrl = sizeof( MIXERCONTROLA );
	lineControls.pamxctrl = controls;

	if ( !mixerGetLineControlsA( (HMIXEROBJ)hMixer, &lineControls, MIXER_GETLINECONTROLSF_ALL ) )
	{
		for ( i = 0; i < (int)line.cControls; i++ )
		{
			if ( ( controls[i].dwControlType & MIXERCONTROL_CT_CLASS_MASK ) != MIXERCONTROL_CT_CLASS_LIST )
				continue;

			channels = line.cChannels;
			items = 0;
			if ( controls[i].fdwControl & MIXERCONTROL_CONTROLF_UNIFORM )
				channels = 1;
			if ( controls[i].fdwControl & MIXERCONTROL_CONTROLF_MULTIPLE )
				items = controls[i].cMultipleItems;

			listText = (MIXERCONTROLDETAILS_LISTTEXTA *)calloc( items * channels * sizeof( MIXERCONTROLDETAILS_LISTTEXTA ), 1 );
			details.cbStruct = sizeof( details );
			details.dwControlID = controls[i].dwControlID;
			details.cChannels = channels;
			details.cMultipleItems = items;
			details.cbDetails = sizeof( MIXERCONTROLDETAILS_LISTTEXTA );
			details.paDetails = listText;
			if ( mixerGetControlDetailsA( (HMIXEROBJ)hMixer, &details, MIXER_GETCONTROLDETAILSF_LISTTEXT ) )
				continue;

			selected = (MIXERCONTROLDETAILS_BOOLEAN *)calloc( items * channels * sizeof( MIXERCONTROLDETAILS_BOOLEAN ), 1 );
			details.cbDetails = sizeof( MIXERCONTROLDETAILS_BOOLEAN );
			details.paDetails = selected;
			if ( mixerGetControlDetailsA( (HMIXEROBJ)hMixer, &details, MIXER_GETCONTROLDETAILSF_VALUE ) )
				continue;

			for ( j = 0; j < (int)items; j++ )
				selected[j].fValue = strstr( listText[j].szName, source ) != NULL;

			result = mixerSetControlDetails( (HMIXEROBJ)hMixer, &details, MIXER_SETCONTROLDETAILSF_VALUE );

			if ( controls )
				free( controls );
			controls = NULL;
			if ( listText )
				free( listText );
			listText = NULL;
			if ( selected )
				free( selected );
			selected = NULL;

			if ( !result )
				return 1;
		}
	}

	if ( controls )
		free( controls );
	if ( listText )
		free( listText );
	if ( selected )
		free( selected );
	return 0;
}

int mixerSetMicrophoneMute( bool mute )
{
	HMIXER hMixer;
	MIXERLINEA line;
	MIXERLINECONTROLSA lineControls;
	MIXERCONTROLA control;
	MIXERCONTROLDETAILS details;
	MIXERCONTROLDETAILS_BOOLEAN value;
	DWORD i;
	DWORD connections;

	if ( waveInGetNumDevs() < 1 || mixerGetNumDevs() < 1 )
		return 0;

	if ( mixerOpen( &hMixer, 0, 0, 0, 0 ) )
		return 0;

	line.cbStruct = sizeof( line );
	line.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	if ( !mixerGetLineInfoA( (HMIXEROBJ)hMixer, &line, MIXER_GETLINEINFOF_COMPONENTTYPE ) )
	{
		connections = line.cConnections;
		for ( i = 0; i < connections; i++ )
		{
			line.dwSource = i;
			mixerGetLineInfoA( (HMIXEROBJ)hMixer, &line, MIXER_GETLINEINFOF_SOURCE );
			if ( !strstr( line.szName, "Mic" ) )
				continue;

			lineControls.cbStruct = sizeof( lineControls );
			lineControls.dwLineID = line.dwLineID;
			lineControls.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
			lineControls.cControls = 1;
			lineControls.cbmxctrl = sizeof( control );
			lineControls.pamxctrl = &control;
			if ( !mixerGetLineControlsA( (HMIXEROBJ)hMixer, &lineControls, MIXER_GETLINECONTROLSF_ONEBYTYPE ) )
			{
				details.paDetails = &value;
				details.cbStruct = sizeof( details );
				details.cChannels = 1;
				details.cbDetails = sizeof( value );
				details.dwControlID = control.dwControlID;
				details.cMultipleItems = 0;
				mixerGetControlDetailsA( (HMIXEROBJ)hMixer, &details, MIXER_GETCONTROLDETAILSF_VALUE );
				value.fValue = mute;
				mixerSetControlDetails( (HMIXEROBJ)hMixer, &details, MIXER_SETCONTROLDETAILSF_VALUE );
			}
			break;
		}
	}

	mixerClose( hMixer );
	return 0;
}

