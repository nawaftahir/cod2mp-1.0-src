#include "../qcommon/qcommon.h"
#include <time.h>

typedef struct qtime_s
{
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
} qtime_t;

const char *Com_StringContains( const char *str1, const char *str2, int ( *cmpFunc )( const char *, const char * ) )
{
	int len, i;

	len = I_strlen(str1) - I_strlen(str2);
	for (i = 0; i <= len; i++, str1++)
	{
		if (!cmpFunc(str1, str2))
		{
			return str1;
		}
	}
	return NULL;
}

bool Com_Filter( const char *filter, const char *name, int casesensitive)
{
	char buf[MAX_TOKEN_CHARS];
	const char *ptr;
	int i, found;

	while(*filter)
	{
		if (*filter == '*')
		{
			filter++;
			for (i = 0; *filter; i++)
			{
				if (*filter == '*' || *filter == '?') break;
				buf[i] = *filter;
				filter++;
			}
			buf[i] = '\0';
			if (strlen(buf))
			{
				ptr = Com_StringContains(name, buf, casesensitive ? I_strcmp : I_stricmp);
				if (!ptr) return qfalse;
				name = ptr + strlen(buf);
			}
		}
		else if (*filter == '?')
		{
			filter++;
			name++;
		}
		else if (*filter == '[' && *(filter+1) == '[')
		{
			filter++;
		}
		else if (*filter == '[')
		{
			filter++;
			found = qfalse;
			while(*filter && !found)
			{
				if (*filter == ']' && *(filter+1) != ']') break;
				if (*(filter+1) == '-' && *(filter+2) && (*(filter+2) != ']' || *(filter+3) == ']'))
				{
					if (casesensitive)
					{
						if (*name >= *filter && *name <= *(filter+2)) found = qtrue;
					}
					else
					{
						if (toupper(*name) >= toupper(*filter) &&
						        toupper(*name) <= toupper(*(filter+2))) found = qtrue;
					}
					filter += 3;
				}
				else
				{
					if (casesensitive)
					{
						if (*filter == *name) found = qtrue;
					}
					else
					{
						if (toupper(*filter) == toupper(*name)) found = qtrue;
					}
					filter++;
				}
			}
			if (!found) return qfalse;
			while(*filter)
			{
				if (*filter == ']' && *(filter+1) != ']') break;
				filter++;
			}
			filter++;
			name++;
		}
		else
		{
			if (casesensitive)
			{
				if (*filter != *name) return qfalse;
			}
			else
			{
				if (toupper(*filter) != toupper(*name)) return qfalse;
			}
			filter++;
			name++;
		}
	}
	return qtrue;
}

bool Com_FilterPath(const char *filter, const char *name, int casesensitive)
{
	int i;
	char new_filter[MAX_QPATH];
	char new_name[MAX_QPATH];

	for (i = 0; i < MAX_QPATH-1 && filter[i]; i++)
	{
		if ( filter[i] == '\\' || filter[i] == ':' )
		{
			new_filter[i] = '/';
		}
		else
		{
			new_filter[i] = filter[i];
		}
	}
	new_filter[i] = '\0';
	for (i = 0; i < MAX_QPATH-1 && name[i]; i++)
	{
		if ( name[i] == '\\' || name[i] == ':' )
		{
			new_name[i] = '/';
		}
		else
		{
			new_name[i] = name[i];
		}
	}
	new_name[i] = '\0';
	return Com_Filter(new_filter, new_name, casesensitive);
}

int Com_HashKey( const char *string, int maxlen )
{
	register int hash, i;

	hash = 0;
	for (i = 0; i < maxlen && string[i] != '\0'; i++)
	{
		hash += string[i] * (119 + i);
	}
	hash = hash ^ (hash >> 10) ^ (hash >> 20);
	return hash;
}

int Com_RealTime( qtime_t *qtime )
{
	time_t t;
	struct tm *tms;

	t = time(NULL);
	if (!qtime)
	{
		return t;
	}
	tms = localtime(&t);
	if (tms)
	{
		qtime->tm_sec = tms->tm_sec;
		qtime->tm_min = tms->tm_min;
		qtime->tm_hour = tms->tm_hour;
		qtime->tm_mday = tms->tm_mday;
		qtime->tm_mon = tms->tm_mon;
		qtime->tm_year = tms->tm_year;
		qtime->tm_wday = tms->tm_wday;
		qtime->tm_yday = tms->tm_yday;
		qtime->tm_isdst = tms->tm_isdst;
	}
	return t;
}

#undef Com_Memcpy
#undef Com_Memset
#undef Com_Memcmp

static void _copyDWord( unsigned int *dest, const unsigned int constant, const unsigned int count )
{
	__asm
	{
			mov		edx, dest
			mov		eax, constant
			mov		ecx, count
			and		ecx, ~7
			jz		padding
			sub		ecx, 8
			jmp		loopu
			align	16
loopu:
			test	[edx + ecx * 4 + 28], ebx	// fetch next block destination to L1 cache
			mov		[edx + ecx * 4 + 0], eax
			mov		[edx + ecx * 4 + 4], eax
			mov		[edx + ecx * 4 + 8], eax
			mov		[edx + ecx * 4 + 12], eax
			mov		[edx + ecx * 4 + 16], eax
			mov		[edx + ecx * 4 + 20], eax
			mov		[edx + ecx * 4 + 24], eax
			mov		[edx + ecx * 4 + 28], eax
			sub		ecx, 8
			jge		loopu
padding:
			mov		ecx, count
			mov		ebx, ecx
			and		ecx, 7
			jz		outta
			and		ebx, ~7
			lea		edx, [edx + ebx * 4]		// advance dest pointer
			test	[edx + 0], eax				// fetch destination to L1 cache
			cmp		ecx, 4
			jl		skip4
			mov		[edx + 0], eax
			mov		[edx + 4], eax
			mov		[edx + 8], eax
			mov		[edx + 12], eax
			add		edx, 16
			sub		ecx, 4
skip4:
			cmp		ecx, 2
			jl		skip2
			mov		[edx + 0], eax
			mov		[edx + 4], eax
			add		edx, 8
			sub		ecx, 2
skip2:
			cmp		ecx, 1
			jl		outta
			mov		[edx + 0], eax
outta:
	}
}

enum e_prefetch { PRE_READ, PRE_WRITE, PRE_READ_WRITE };

// Retail 0x42c330 retains this helper as well as the inlined memory-copy loop.
__forceinline void Com_Prefetch( const void *src, int count, e_prefetch type )
{
	switch (type)
	{
	case PRE_WRITE:
		break;
	case PRE_READ:
	case PRE_READ_WRITE:
	__asm
	{
			mov		ebx, src
			mov		ecx, count
			cmp		ecx, 4096					// clamp to 4kB
			jle		skipClamp
			mov		ecx, 4096
skipClamp:
			add		ecx, 0x1f
			shr		ecx, 5						// number of cache lines
			jz		skip
			jmp		loopie
			align	16
loopie:
			test	byte ptr [ebx], al
			add		ebx, 32
			dec		ecx
			jnz		loopie
skip:
	}
		break;
	}
}

// optimized memory copy routine that handles all alignment cases and block sizes efficiently
void Com_Memcpy( void *dest, const void *src, int count )
{
	Com_Prefetch(src, count, PRE_READ);

	__asm
	{
		push	edi
		push	esi
		mov		ecx, count
		cmp		ecx, 0							// count = 0 check (just to be on the safe side)
		je		outta
		mov		edx, dest
		mov		ebx, src
		cmp		ecx, 32							// padding only?
		jl		padding

		mov		edi, ecx
		and		edi, ~31						// edi = count & ~31
		sub		edi, 32

		align	16
loopMisAligned:
		mov		eax, [ebx + edi + 0 + 0 * 8]
		mov		esi, [ebx + edi + 4 + 0 * 8]
		mov		[edx + edi + 0 + 0 * 8], eax
		mov		[edx + edi + 4 + 0 * 8], esi
		mov		eax, [ebx + edi + 0 + 1 * 8]
		mov		esi, [ebx + edi + 4 + 1 * 8]
		mov		[edx + edi + 0 + 1 * 8], eax
		mov		[edx + edi + 4 + 1 * 8], esi
		mov		eax, [ebx + edi + 0 + 2 * 8]
		mov		esi, [ebx + edi + 4 + 2 * 8]
		mov		[edx + edi + 0 + 2 * 8], eax
		mov		[edx + edi + 4 + 2 * 8], esi
		mov		eax, [ebx + edi + 0 + 3 * 8]
		mov		esi, [ebx + edi + 4 + 3 * 8]
		mov		[edx + edi + 0 + 3 * 8], eax
		mov		[edx + edi + 4 + 3 * 8], esi
		sub		edi, 32
		jge		loopMisAligned

		mov		edi, ecx
		and		edi, ~31
		add		ebx, edi						// increase src pointer
		add		edx, edi						// increase dst pointer
		and		ecx, 31							// new count
		jz		outta							// if count = 0, get outta here

padding:
		cmp		ecx, 16
		jl		skip16
		mov		eax, dword ptr [ebx]
		mov		dword ptr [edx], eax
		mov		eax, dword ptr [ebx + 4]
		mov		dword ptr [edx + 4], eax
		mov		eax, dword ptr [ebx + 8]
		mov		dword ptr [edx + 8], eax
		mov		eax, dword ptr [ebx + 12]
		mov		dword ptr [edx + 12], eax
		sub		ecx, 16
		add		ebx, 16
		add		edx, 16
skip16:
		cmp		ecx, 8
		jl		skip8
		mov		eax, dword ptr [ebx]
		mov		dword ptr [edx], eax
		mov		eax, dword ptr [ebx + 4]
		sub		ecx, 8
		mov		dword ptr [edx + 4], eax
		add		ebx, 8
		add		edx, 8
skip8:
		cmp		ecx, 4
		jl		skip4
		mov		eax, dword ptr [ebx]			// here 4-7 bytes
		add		ebx, 4
		sub		ecx, 4
		mov		dword ptr [edx], eax
		add		edx, 4
skip4:											// 0-3 remaining bytes
		cmp		ecx, 2
		jl		skip2
		mov		ax, word ptr [ebx]				// two bytes
		cmp		ecx, 3							// less than 3?
		mov		word ptr [edx], ax
		jl		outta
		mov		al, byte ptr [ebx + 2]			// last byte
		mov		byte ptr [edx + 2], al
		jmp		outta
skip2:
		cmp		ecx, 1
		jl		outta
		mov		al, byte ptr [ebx]
		mov		byte ptr [edx], al
outta:
		pop		esi
		pop		edi
	}
}

void Com_Memset( void *dest, int val, int count )
{
	unsigned int fillval;

	if ( count < 8 )
	{
		__asm
		{
			mov		edx, dest
			mov		eax, val
			mov		ah, al
			mov		ebx, eax
			and		ebx, 0xffff
			shl		eax, 16
			add		eax, ebx					// eax now contains pattern
			mov		ecx, count
			cmp		ecx, 4
			jl		skip4
			mov		[edx], eax					// copy first dword
			add		edx, 4
			sub		ecx, 4
skip4:
			cmp		ecx, 2
			jl		skip2
			mov		word ptr [edx], ax			// copy 2 bytes
			add		edx, 2
			sub		ecx, 2
skip2:
			cmp		ecx, 0
			je		skip1
			mov		byte ptr [edx], al			// copy single byte
skip1:
		}
		return;
	}

	fillval = val;

	fillval = fillval | ( fillval << 8 );
	fillval = fillval | ( fillval << 16 );			// fill dword with 8-bit pattern

	_copyDWord( (unsigned int *)dest, fillval, count / 4 );

	__asm											// padding of 0-3 bytes
	{
		mov		ecx, count
		mov		eax, ecx
		and		ecx, 3
		jz		skipA
		and		eax, ~3
		mov		ebx, dest
		add		ebx, eax
		mov		eax, fillval
		cmp		ecx, 2
		jl		skipB
		mov		word ptr [ebx], ax
		cmp		ecx, 2
		je		skipA
		mov		byte ptr [ebx + 2], al
		jmp		skipA
skipB:
		cmp		ecx, 0
		je		skipA
		mov		byte ptr [ebx], al
skipA:
	}
}

// retail 0x42c290, unreferenced; Q3 Com_Memcmp (SUPPORTED)
qboolean Com_Memcmp( const void *src0, const void *src1, const unsigned int count )
{
	unsigned int i;

	if ( count >= 16 )
	{
		unsigned int *dw = (unsigned int *)src0;
		unsigned int *sw = (unsigned int *)src1;

		unsigned int nm2 = count / 16;
		for ( i = 0; i < nm2; i += 4 )
		{
			unsigned int tmp = ( dw[i + 0] - sw[i + 0] ) | ( dw[i + 1] - sw[i + 1] ) |
			                   ( dw[i + 2] - sw[i + 2] ) | ( dw[i + 3] - sw[i + 3] );
			if ( tmp )
				return qfalse;
		}
	}
	if ( count & 15 )
	{
		byte *d = (byte *)src0;
		byte *s = (byte *)src1;
		for ( i = count & 0xfffffff0; i < count; i++ )
			if ( d[i] != s[i] )
				return qfalse;
	}

	return qtrue;
}

