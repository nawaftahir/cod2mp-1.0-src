#include "qcommon.h"
#include "server.h"

#define FLOAT_INT_BITS      13
#define FLOAT_INT_BIAS      ( 1 << ( FLOAT_INT_BITS - 1 ) )
#define HUDELEM_COORD_BITS  10
#define HUDELEM_COORD_BIAS  ( 1 << ( HUDELEM_COORD_BITS - 1 ) )

#define KEY_MASK_FORWARD    1
#define KEY_MASK_BACK       2
#define KEY_MASK_MOVERIGHT  4
#define KEY_MASK_MOVELEFT   8

static unsigned int kbitmask[] =
{
	0x00000000, 0x00000001, 0x00000003,
	0x00000007, 0x0000000F, 0x0000001F,
	0x0000003F, 0x0000007F, 0x000000FF,
	0x000001FF, 0x000003FF, 0x000007FF,
	0x00000FFF, 0x00001FFF, 0x00003FFF,
	0x00007FFF, 0x0000FFFF, 0x0001FFFF,
	0x0003FFFF, 0x0007FFFF, 0x000FFFFF,
	0x001FFFFf, 0x003FFFFF, 0x007FFFFF,
	0x00FFFFFF, 0x01FFFFFF, 0x03FFFFFF,
	0x07FFFFFF, 0x0FFFFFFF, 0x1FFFFFFF,
	0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF,
};

#define OBJF( x ) # x, offsetof( objective_t, x )

static netField_t objectiveFields[] =
{
	{ OBJF( origin[0] ), 0},
	{ OBJF( origin[1] ), 0},
	{ OBJF( origin[2] ), 0},
	{ OBJF( icon ), 12},
	{ OBJF( entNum ), 10},
	{ OBJF( teamNum ), 4},
};

#define HEF( x ) # x, offsetof( hudelem_t, x )

static netField_t hudElemFields[] =
{
	{ HEF( y ), -99},
	{ HEF( type ), 4},
	{ HEF( color.rgba ), 32},
	{ HEF( x ), -99},
	{ HEF( alignScreen ), 6},
	{ HEF( fontScale ), 0},
	{ HEF( materialIndex ), 8},
	{ HEF( width ), 10},
	{ HEF( height ), 10},
	{ HEF( fadeStartTime ), 32},
	{ HEF( fromColor.rgba ), 32},
	{ HEF( fadeTime ), 16},
	{ HEF( value ), 0},
	{ HEF( time ), 32},
	{ HEF( z ), -99},
	{ HEF( alignOrg ), 4},
	{ HEF( sort ), 0},
	{ HEF( text ), 8},
	{ HEF( font ), 4},
	{ HEF( scaleStartTime ), 32},
	{ HEF( scaleTime ), 16},
	{ HEF( fromHeight ), 10},
	{ HEF( label ), 8},
	{ HEF( fromWidth ), 10},
	{ HEF( moveStartTime ), 32},
	{ HEF( moveTime ), 16},
	{ HEF( fromX ), -99},
	{ HEF( fromY ), -99},
	{ HEF( fromAlignScreen ), 6},
	{ HEF( fromAlignOrg ), 4},
	{ HEF( duration ), 32},
	{ HEF( foreground ), 1},
};

// Q3 TA frequency table
static int msg_hData[256] =
{
	250315,			// 0
	41193,			// 1
	6292,			// 2
	7106,			// 3
	3730,			// 4
	3750,			// 5
	6110,			// 6
	23283,			// 7
	33317,			// 8
	6950,			// 9
	7838,			// 10
	9714,			// 11
	9257,			// 12
	17259,			// 13
	3949,			// 14
	1778,			// 15
	8288,			// 16
	1604,			// 17
	1590,			// 18
	1663,			// 19
	1100,			// 20
	1213,			// 21
	1238,			// 22
	1134,			// 23
	1749,			// 24
	1059,			// 25
	1246,			// 26
	1149,			// 27
	1273,			// 28
	4486,			// 29
	2805,			// 30
	3472,			// 31
	21819,			// 32
	1159,			// 33
	1670,			// 34
	1066,			// 35
	1043,			// 36
	1012,			// 37
	1053,			// 38
	1070,			// 39
	1726,			// 40
	888,			// 41
	1180,			// 42
	850,			// 43
	960,			// 44
	780,			// 45
	1752,			// 46
	3296,			// 47
	10630,			// 48
	4514,			// 49
	5881,			// 50
	2685,			// 51
	4650,			// 52
	3837,			// 53
	2093,			// 54
	1867,			// 55
	2584,			// 56
	1949,			// 57
	1972,			// 58
	940,			// 59
	1134,			// 60
	1788,			// 61
	1670,			// 62
	1206,			// 63
	5719,			// 64
	6128,			// 65
	7222,			// 66
	6654,			// 67
	3710,			// 68
	3795,			// 69
	1492,			// 70
	1524,			// 71
	2215,			// 72
	1140,			// 73
	1355,			// 74
	971,			// 75
	2180,			// 76
	1248,			// 77
	1328,			// 78
	1195,			// 79
	1770,			// 80
	1078,			// 81
	1264,			// 82
	1266,			// 83
	1168,			// 84
	965,			// 85
	1155,			// 86
	1186,			// 87
	1347,			// 88
	1228,			// 89
	1529,			// 90
	1600,			// 91
	2617,			// 92
	2048,			// 93
	2546,			// 94
	3275,			// 95
	2410,			// 96
	3585,			// 97
	2504,			// 98
	2800,			// 99
	2675,			// 100
	6146,			// 101
	3663,			// 102
	2840,			// 103
	14253,			// 104
	3164,			// 105
	2221,			// 106
	1687,			// 107
	3208,			// 108
	2739,			// 109
	3512,			// 110
	4796,			// 111
	4091,			// 112
	3515,			// 113
	5288,			// 114
	4016,			// 115
	7937,			// 116
	6031,			// 117
	5360,			// 118
	3924,			// 119
	4892,			// 120
	3743,			// 121
	4566,			// 122
	4807,			// 123
	5852,			// 124
	6400,			// 125
	6225,			// 126
	8291,			// 127
	23243,			// 128
	7838,			// 129
	7073,			// 130
	8935,			// 131
	5437,			// 132
	4483,			// 133
	3641,			// 134
	5256,			// 135
	5312,			// 136
	5328,			// 137
	5370,			// 138
	3492,			// 139
	2458,			// 140
	1694,			// 141
	1821,			// 142
	2121,			// 143
	1916,			// 144
	1149,			// 145
	1516,			// 146
	1367,			// 147
	1236,			// 148
	1029,			// 149
	1258,			// 150
	1104,			// 151
	1245,			// 152
	1006,			// 153
	1149,			// 154
	1025,			// 155
	1241,			// 156
	952,			// 157
	1287,			// 158
	997,			// 159
	1713,			// 160
	1009,			// 161
	1187,			// 162
	879,			// 163
	1099,			// 164
	929,			// 165
	1078,			// 166
	951,			// 167
	1656,			// 168
	930,			// 169
	1153,			// 170
	1030,			// 171
	1262,			// 172
	1062,			// 173
	1214,			// 174
	1060,			// 175
	1621,			// 176
	930,			// 177
	1106,			// 178
	912,			// 179
	1034,			// 180
	892,			// 181
	1158,			// 182
	990,			// 183
	1175,			// 184
	850,			// 185
	1121,			// 186
	903,			// 187
	1087,			// 188
	920,			// 189
	1144,			// 190
	1056,			// 191
	3462,			// 192
	2240,			// 193
	4397,			// 194
	12136,			// 195
	7758,			// 196
	1345,			// 197
	1307,			// 198
	3278,			// 199
	1950,			// 200
	886,			// 201
	1023,			// 202
	1112,			// 203
	1077,			// 204
	1042,			// 205
	1061,			// 206
	1071,			// 207
	1484,			// 208
	1001,			// 209
	1096,			// 210
	915,			// 211
	1052,			// 212
	995,			// 213
	1070,			// 214
	876,			// 215
	1111,			// 216
	851,			// 217
	1059,			// 218
	805,			// 219
	1112,			// 220
	923,			// 221
	1103,			// 222
	817,			// 223
	1899,			// 224
	1872,			// 225
	976,			// 226
	841,			// 227
	1127,			// 228
	956,			// 229
	1159,			// 230
	950,			// 231
	7791,			// 232
	954,			// 233
	1289,			// 234
	933,			// 235
	1127,			// 236
	3207,			// 237
	1020,			// 238
	927,			// 239
	1355,			// 240
	768,			// 241
	1040,			// 242
	745,			// 243
	952,			// 244
	805,			// 245
	1073,			// 246
	740,			// 247
	1013,			// 248
	805,			// 249
	1008,			// 250
	796,			// 251
	996,			// 252
	1057,			// 253
	11457,			// 254
	13504,			// 255
};

#define ESF( x ) # x, offsetof( entityState_t, x )

static const netField_t entityStateFields[] =
{
	{ ESF( pos.trTime ), 32},
	{ ESF( pos.trBase[1] ), 0},
	{ ESF( pos.trBase[0] ), 0},
	{ ESF( pos.trDelta[0] ), 0},
	{ ESF( pos.trDelta[1] ), 0},
	{ ESF( angles2[1] ), 0},
	{ ESF( apos.trBase[1] ), -100},
	{ ESF( pos.trDelta[2] ), 0},
	{ ESF( pos.trBase[2] ), 0},
	{ ESF( apos.trBase[0] ), -100},
	{ ESF( eventSequence ), 8},
	{ ESF( legsAnim ), 10},
	{ ESF( eType ), 8},
	{ ESF( eFlags ), 24},
	{ ESF( otherEntityNum ), 10},
	{ ESF( surfType ), 8},
	{ ESF( eventParm ), 8},
	{ ESF( scale ), 8},
	{ ESF( clientNum ), 8},
	{ ESF( torsoAnim ), 10},
	{ ESF( groundEntityNum ), 10},
	{ ESF( events[0] ), 8},
	{ ESF( events[1] ), 8},
	{ ESF( events[2] ), 8},
	{ ESF( angles2[0] ), 0},
	{ ESF( events[3] ), 8},
	{ ESF( apos.trBase[2] ), -100},
	{ ESF( pos.trType ), 8},
	{ ESF( fWaistPitch ), 0},
	{ ESF( fTorsoPitch ), 0},
	{ ESF( apos.trTime ), 32},
	{ ESF( solid ), 24},
	{ ESF( apos.trDelta[0] ), 0},
	{ ESF( apos.trType ), 8},
	{ ESF( animMovetype ), 4},
	{ ESF( fTorsoHeight ), 0},
	{ ESF( apos.trDelta[2] ), 0},
	{ ESF( weapon ), 7},
	{ ESF( index ), 10},
	{ ESF( apos.trDelta[1] ), 0},
	{ ESF( eventParms[0] ), 8},
	{ ESF( eventParms[1] ), 8},
	{ ESF( eventParms[2] ), 8},
	{ ESF( eventParms[3] ), 8},
	{ ESF( iHeadIcon ), 4},
	{ ESF( pos.trDuration ), 32},
	{ ESF( iHeadIconTeam ), 2},
	{ ESF( time ), 32},
	{ ESF( leanf ), 0},
	{ ESF( attackerEntityNum ), 10},
	{ ESF( time2 ), 32},
	{ ESF( loopSound ), 8},
	{ ESF( origin2[2] ), 0},
	{ ESF( origin2[0] ), 0},
	{ ESF( origin2[1] ), 0},
	{ ESF( angles2[2] ), 0},
	{ ESF( constantLight ), 32},
	{ ESF( apos.trDuration ), 32},
	{ ESF( dmgFlags ), 32},
};

#define AESF( x ) # x, offsetof( archivedEntity_t, s.x )
#define AERF( x ) # x, offsetof( archivedEntity_t, r.x )

static const netField_t archivedEntityFields[] =
{
	{ AERF( absmin[1] ), 0},
	{ AERF( absmax[1] ), 0},
	{ AERF( absmin[0] ), 0},
	{ AERF( absmax[0] ), 0},
	{ AERF( absmin[2] ), 0},
	{ AERF( absmax[2] ), 0},
	{ AESF( pos.trBase[1] ), 0},
	{ AESF( pos.trBase[0] ), 0},
	{ AESF( eType ), 8},
	{ AESF( eFlags ), 24},
	{ AESF( pos.trBase[2] ), 0},
	{ AERF( svFlags ), 32},
	{ AESF( groundEntityNum ), 10},
	{ AESF( apos.trBase[1] ), 0},
	{ AESF( clientNum ), 8},
	{ AESF( apos.trBase[0] ), 0},
	{ AESF( index ), 10},
	{ AESF( apos.trBase[2] ), 0},
	{ AESF( eventSequence ), 8},
	{ AESF( events[0] ), 8},
	{ AESF( legsAnim ), 10},
	{ AESF( events[1] ), 8},
	{ AESF( events[2] ), 8},
	{ AESF( events[3] ), 8},
	{ AESF( weapon ), 7},
	{ AESF( pos.trType ), 8},
	{ AESF( pos.trTime ), 32},
	{ AESF( apos.trType ), 8},
	{ AESF( solid ), 24},
	{ AESF( pos.trDuration ), 32},
	{ AESF( eventParms[0] ), 8},
	{ AESF( torsoAnim ), 10},
	{ AESF( pos.trDelta[0] ), 0},
	{ AESF( pos.trDelta[1] ), 0},
	{ AESF( angles2[1] ), 0},
	{ AESF( angles2[0] ), 0},
	{ AESF( animMovetype ), 4},
	{ AESF( pos.trDelta[2] ), 0},
	{ AESF( otherEntityNum ), 10},
	{ AESF( eventParms[1] ), 8},
	{ AESF( surfType ), 8},
	{ AESF( eventParm ), 8},
	{ AESF( eventParms[2] ), 8},
	{ AESF( scale ), 8},
	{ AESF( eventParms[3] ), 8},
	{ AESF( fTorsoHeight ), 0},
	{ AESF( fWaistPitch ), 0},
	{ AESF( fTorsoPitch ), 0},
	{ AESF( apos.trTime ), 32},
	{ AESF( apos.trDelta[0] ), 0},
	{ AESF( apos.trDelta[2] ), 0},
	{ AERF( clientMask[0] ), 32},
	{ AERF( clientMask[1] ), 32},
	{ AESF( leanf ), 0},
	{ AESF( apos.trDelta[1] ), 0},
	{ AESF( loopSound ), 8},
	{ AESF( attackerEntityNum ), 10},
	{ AESF( iHeadIcon ), 4},
	{ AESF( iHeadIconTeam ), 2},
	{ AESF( apos.trDuration ), 32},
	{ AESF( time ), 32},
	{ AESF( time2 ), 32},
	{ AESF( origin2[0] ), 0},
	{ AESF( origin2[1] ), 0},
	{ AESF( origin2[2] ), 0},
	{ AESF( angles2[2] ), 0},
	{ AESF( constantLight ), 32},
	{ AESF( dmgFlags ), 32},
};

#define CSF( x ) # x, offsetof( clientState_t, x )

static const netField_t clientStateFields[] =
{
	{ CSF( team ), 2},
	{ CSF( name[0] ), 32},
	{ CSF( name[4] ), 32},
	{ CSF( modelindex ), 8},
	{ CSF( attachModelIndex[1] ), 8},
	{ CSF( attachModelIndex[0] ), 8},
	{ CSF( name[8] ), 32},
	{ CSF( name[12] ), 32},
	{ CSF( name[16] ), 32},
	{ CSF( name[20] ), 32},
	{ CSF( name[24] ), 32},
	{ CSF( name[28] ), 32},
	{ CSF( attachTagIndex[5] ), 5},
	{ CSF( attachTagIndex[0] ), 5},
	{ CSF( attachTagIndex[1] ), 5},
	{ CSF( attachTagIndex[2] ), 5},
	{ CSF( attachTagIndex[3] ), 5},
	{ CSF( attachTagIndex[4] ), 5},
	{ CSF( attachModelIndex[2] ), 8},
	{ CSF( attachModelIndex[3] ), 8},
	{ CSF( attachModelIndex[4] ), 8},
	{ CSF( attachModelIndex[5] ), 8},
};

#define PSF( x ) # x, offsetof( playerState_t, x )

static const netField_t playerStateFields[] =
{
	{ PSF( commandTime ), 32},
	{ PSF( origin[1] ), 0},
	{ PSF( origin[0] ), 0},
	{ PSF( bobCycle ), 8},
	{ PSF( viewangles[1] ), -100},
	{ PSF( origin[2] ), 0},
	{ PSF( velocity[1] ), 0},
	{ PSF( velocity[0] ), 0},
	{ PSF( viewangles[0] ), -100},
	{ PSF( movementDir ), -8},
	{ PSF( velocity[2] ), 0},
	{ PSF( eventSequence ), 8},
	{ PSF( legsAnim ), 10},
	{ PSF( aimSpreadScale ), 0},
	{ PSF( weaponTime ), -16},
	{ PSF( pm_flags ), 27},
	{ PSF( events[0] ), 8},
	{ PSF( events[1] ), 8},
	{ PSF( events[2] ), 8},
	{ PSF( events[3] ), 8},
	{ PSF( weapAnim ), 10},
	{ PSF( viewHeightCurrent ), 0},
	{ PSF( torsoTimer ), 16},
	{ PSF( torsoAnim ), 10},
	{ PSF( eFlags ), 24},
	{ PSF( fWeaponPosFrac ), 0},
	{ PSF( holdBreathScale ), 0},
	{ PSF( weaponstate ), 5},
	{ PSF( viewHeightTarget ), -8},
	{ PSF( weaponDelay ), -16},
	{ PSF( legsTimer ), 16},
	{ PSF( viewHeightLerpTarget ), -8},
	{ PSF( groundEntityNum ), 10},
	{ PSF( pm_time ), -16},
	{ PSF( eventParms[3] ), 8},
	{ PSF( eventParms[1] ), 8},
	{ PSF( eventParms[0] ), 8},
	{ PSF( eventParms[2] ), 8},
	{ PSF( weapon ), 7},
	{ PSF( weapons[0] ), 32},
	{ PSF( viewHeightLerpDown ), 1},
	{ PSF( weaponslots[0] ), 32},
	{ PSF( delta_angles[0] ), 16},
	{ PSF( delta_angles[1] ), 16},
	{ PSF( cursorHintString ), -8},
	{ PSF( offHandIndex ), 7},
	{ PSF( clientNum ), 8},
	{ PSF( viewlocked_entNum ), 16},
	{ PSF( viewmodelIndex ), 8},
	{ PSF( viewHeightLerpTime ), 32},
	{ PSF( speed ), 16},
	{ PSF( mins[1] ), 0},
	{ PSF( mins[0] ), 0},
	{ PSF( maxs[2] ), 0},
	{ PSF( maxs[1] ), 0},
	{ PSF( maxs[0] ), 0},
	{ PSF( gravity ), 16},
	{ PSF( damageTimer ), 16},
	{ PSF( cursorHint ), 8},
	{ PSF( mantleState.flags ), 4},
	{ PSF( flinchYaw ), 16},
	{ PSF( fWaistPitch ), 0},
	{ PSF( mantleState.timer ), 32},
	{ PSF( fTorsoPitch ), 0},
	{ PSF( proneTorsoPitch ), 0},
	{ PSF( holdBreathTimer ), 16},
	{ PSF( jumpTime ), 32},
	{ PSF( viewangles[2] ), -100},
	{ PSF( foliageSoundTime ), 32},
	{ PSF( weapons[1] ), 32},
	{ PSF( damageEvent ), 8},
	{ PSF( damageDuration ), 16},
	{ PSF( damageYaw ), 8},
	{ PSF( proneDirection ), 0},
	{ PSF( proneDirectionPitch ), 0},
	{ PSF( mantleState.yaw ), 0},
	{ PSF( mantleState.transIndex ), 4},
	{ PSF( fTorsoHeight ), 0},
	{ PSF( damagePitch ), 8},
	{ PSF( jumpOriginZ ), 0},
	{ PSF( pm_type ), 8},
	{ PSF( viewlocked ), 8},
	{ PSF( weaponrechamber[0] ), 32},
	{ PSF( vLadderVec[0] ), 0},
	{ PSF( weaponslots[4] ), 32},
	{ PSF( weaponRestrictKickTime ), -16},
	{ PSF( vLadderVec[1] ), 0},
	{ PSF( viewAngleClampRange[1] ), 0},
	{ PSF( viewAngleClampRange[0] ), 0},
	{ PSF( viewAngleClampBase[1] ), 0},
	{ PSF( weaponrechamber[1] ), 32},
	{ PSF( leanf ), 0},
	{ PSF( damageCount ), 7},
	{ PSF( grenadeTimeLeft ), -16},
	{ PSF( deltaTime ), 32},
	{ PSF( shellshockTime ), 32},
	{ PSF( shellshockIndex ), 4},
	{ PSF( shellshockDuration ), 16},
	{ PSF( vLadderVec[2] ), 0},
	{ PSF( delta_angles[2] ), 16},
	{ PSF( viewHeightLerpPosAdj ), 0},
	{ PSF( mins[2] ), 0},
	{ PSF( viewAngleClampBase[0] ), 0},
	{ PSF( adsDelayTime ), 32},
	{ PSF( iCompassFriendInfo ), 32},
};

static huffman_t msgHuff;
static qboolean msgInit;

void MSG_initHuffman();

extern const dvar_t *cl_shownet;

extern const dvar_t *cl_shownet;
#define LOG( x ) if ( cl_shownet && cl_shownet->current.integer == 4 ) { Com_Printf( "%s ", x ); }

void MSG_Init( msg_t *buf, byte *data, int length )
{
	if ( !msgInit )
	{
		MSG_initHuffman();
	}

	memset( buf, 0, sizeof( *buf ) );
	buf->data = data;
	buf->maxsize = length;
}

void MSG_BeginReading( msg_t *msg )
{
	msg->overflowed = qfalse;
	msg->readcount = 0;
	msg->bit = 0;
}

int MSG_GetUsedBitCount( const msg_t *msg )
{
	return ( msg->cursize << 3 ) - ( ( 8 - msg->bit ) & 7 );
}

// the space check covers the worst case once; bits are then stored unchecked
void MSG_WriteBits( msg_t *msg, int value, int bits )
{
	int bit;

	if ( msg->maxsize - msg->cursize < 4 )
	{
		msg->overflowed = qtrue;
		return;
	}

	while ( bits )
	{
		bits--;

		bit = msg->bit & 7;

		if ( !bit )
		{
			msg->bit = msg->cursize * 8;
			msg->data[msg->cursize] = 0;
			msg->cursize++;
		}

		if ( value & 1 )
		{
			msg->data[msg->bit >> 3] |= 1 << bit;
		}

		msg->bit++;
		value = value >> 1;
	}
}

void MSG_WriteBit0( msg_t *msg )
{
	int bit;

	if ( msg->cursize >= msg->maxsize )
	{
		msg->overflowed = qtrue;
		return;
	}

	bit = msg->bit & 7;

	if ( !bit )
	{
		msg->bit = msg->cursize * 8;
		msg->data[msg->cursize] = 0;
		msg->cursize++;
	}

	msg->bit++;
}

void MSG_WriteBit1( msg_t *msg )
{
	int bit;

	if ( msg->cursize >= msg->maxsize )
	{
		msg->overflowed = qtrue;
		return;
	}

	bit = msg->bit & 7;

	if ( !bit )
	{
		msg->bit = msg->cursize * 8;
		msg->data[msg->cursize] = 0;
		msg->cursize++;
	}

	msg->data[msg->bit >> 3] |= 1 << bit;
	msg->bit++;
}

int MSG_ReadBits( msg_t *msg, int bits )
{
	int value = 0;
	int bit;
	int i;

	for ( i = 0; i < bits; i++ )
	{
		bit = msg->bit & 7;

		if ( !bit )
		{
			if ( msg->readcount >= msg->cursize )
			{
				msg->overflowed = qtrue;
				return -1;
			}

			msg->bit = msg->readcount * 8;
			msg->readcount++;
		}

		value |= ( msg->data[msg->bit >> 3] >> bit & 1 ) << i;
		msg->bit++;
	}

	return value;
}

int MSG_ReadBit( msg_t *msg )
{
	int value;
	int bit;

	bit = msg->bit & 7;

	if ( !bit )
	{
		if ( msg->readcount >= msg->cursize )
		{
			msg->overflowed = qtrue;
			return -1;
		}

		msg->bit = msg->readcount * 8;
		msg->readcount++;
	}

	value = msg->data[msg->bit >> 3] >> bit & 1;
	msg->bit++;
	return value;
}

int MSG_WriteBitsCompress( byte *from, byte *to, int fromSizeBytes )
{
	int size;
	int bit;

	bit = 0;
	size = fromSizeBytes;

	while ( size )
	{
		Huff_offsetTransmit( &msgHuff.compressor, *from, to, &bit );
		--size;
		++from;
	}

	return ( bit + 7 ) >> 3;
}

int MSG_ReadBitsCompress( byte *from, byte *to, int toSizeBytes )
{
	int get;
	int bits;
	int bit;
	int unused;     // zeroed but never read; the frame carries it
	byte *data;

	bits = toSizeBytes * 8;
	unused = 0;
	data = to;
	bit = 0;

	while ( bit < bits )
	{
		Huff_offsetReceive( msgHuff.decompressor.tree, &get, from, &bit );
		*data = get;
		*data++;
	}

	return data - to;
}

void MSG_WriteByte( msg_t *msg, int c )
{
	if ( msg->cursize < msg->maxsize )
	{
		msg->data[msg->cursize] = c;
		msg->cursize++;
		return;
	}

	msg->overflowed = qtrue;
}

void MSG_WriteData( msg_t *msg, const void *data, int length )
{
	int newsize = msg->cursize + length;

	if ( newsize <= msg->maxsize )
	{
		memcpy( &msg->data[msg->cursize], data, length );
		msg->cursize = newsize;
		return;
	}

	msg->overflowed = qtrue;
}

void MSG_WriteShort( msg_t *msg, int c )
{
	int newsize = msg->cursize + 2;

	if ( newsize <= msg->maxsize )
	{
		*(short *)&msg->data[msg->cursize] = LittleShort( c );
		msg->cursize = newsize;
		return;
	}

	msg->overflowed = qtrue;
}

void MSG_WriteLong( msg_t *msg, int c )
{
	int newsize = msg->cursize + 4;

	if ( newsize <= msg->maxsize )
	{
		*(int *)&msg->data[msg->cursize] = LittleLong( c );
		msg->cursize = newsize;
		return;
	}

	msg->overflowed = qtrue;
}

void MSG_WriteInt64( msg_t *msg, int64_t c )
{
	int newsize;

	newsize = msg->cursize + 8;

	if ( newsize <= msg->maxsize )
	{
		*(int64_t *)&msg->data[msg->cursize] = LittleLong64( c );
		msg->cursize = newsize;
	}
	else
	{
		msg->overflowed = qtrue;
	}
}

void MSG_WriteString( msg_t *sb, const char *s )
{
	int l;
	int i;
	char string[MAX_STRING_CHARS];

	l = strlen( s );

	if ( l >= MAX_STRING_CHARS )
	{
		Com_Printf( "MSG_WriteString: MAX_STRING_CHARS" );
		MSG_WriteData( sb, "", 1 );
		return;
	}

	for ( i = 0; i < l; i++ )
	{
		string[i] = I_CleanChar( s[i] );
	}

	string[i] = 0;
	MSG_WriteData( sb, string, l + 1 );
}

void MSG_WriteBigString( msg_t *sb, const char *s )
{
	int l, i;
	char string[BIG_INFO_STRING];

	l = strlen( s );

	if ( l >= BIG_INFO_STRING )
	{
		Com_Printf( "MSG_WriteString: BIG_INFO_STRING" );
		MSG_WriteData( sb, "", 1 );
		return;
	}

	I_strncpyz( string, s, sizeof( string ) );

	for ( i = 0; i < l; ++i )
	{
		string[i] = I_CleanChar( string[i] );
	}

	MSG_WriteData( sb, string, l + 1 );
}

void MSG_WriteAngle( msg_t *sb, float f )
{
	MSG_WriteByte( sb, (int)( f * 256.0f / 360.0f ) & 255 );
}

void MSG_WriteAngle16( msg_t *sb, float f )
{
	MSG_WriteShort( sb, ANGLE2SHORT( f ) );
}

int MSG_ReadByte( msg_t *msg )
{
	if ( msg->readcount < msg->cursize )
	{
		int c = msg->data[msg->readcount];
		msg->readcount++;
		return c;
	}

	msg->overflowed = qtrue;
	return -1;
}

int MSG_ReadShort( msg_t *msg )
{
	int c;
	int newcount = msg->readcount + 2;

	if ( newcount <= msg->cursize )
	{
		c = LittleShort( *(short *)&msg->data[msg->readcount] );
		msg->readcount = newcount;
		return c;
	}

	msg->overflowed = qtrue;
	return -1;
}

int MSG_ReadLong( msg_t *msg )
{
	int c;
	int newcount = msg->readcount + 4;

	if ( newcount <= msg->cursize )
	{
		c = LittleLong( *(int *)&msg->data[msg->readcount] );
		msg->readcount = newcount;
		return c;
	}

	msg->overflowed = qtrue;
	return -1;
}

int64_t MSG_ReadInt64( msg_t *msg )
{
	int64_t c;
	int newcount = msg->readcount + 8;

	if ( newcount <= msg->cursize )
	{
		c = LittleLong64( *(int64_t *)&msg->data[msg->readcount] );
		msg->readcount = newcount;
		return c;
	}

	msg->overflowed = qtrue;
	return 0;
}

char *MSG_ReadCommandString( msg_t *msg )
{
	static char string[MAX_STRING_CHARS];
	unsigned int l;
	int c;

	for ( l = 0; ; l++ )
	{
		c = MSG_ReadByte( msg );

		if ( c == -1 )
		{
			c = 0;
		}

		if ( l < sizeof( string ) )
		{
			string[l] = I_CleanChar( c );
		}

		if ( c == 0 )
		{
			break;
		}
	}

	string[sizeof( string ) - 1] = 0;

	return string;
}

char *MSG_ReadBigString( msg_t *msg )
{
	static char string[BIG_INFO_STRING];
	unsigned int l;
	int c;

	for ( l = 0; ; l++ )
	{
		c = MSG_ReadByte( msg );

		if ( c == '%' )
		{
			c = '.';
		}
		else if ( c == -1 )
		{
			c = 0;
		}

		if ( l < sizeof( string ) )
		{
			string[l] = I_CleanChar( c );
		}

		if ( c == 0 )
		{
			break;
		}
	}

	string[sizeof( string ) - 1] = 0;

	return string;
}

char *MSG_ReadStringLine( msg_t *msg )
{
	static char string[MAX_STRING_CHARS];
	unsigned int l;
	int c;

	for ( l = 0; ; l++ )
	{
		c = MSG_ReadByte( msg );

		if ( c == '%' )
		{
			c = '.';
		}
		else if ( c == '\n' || c == -1 )
		{
			c = 0;
		}

		if ( l < sizeof( string ) )
		{
			string[l] = I_CleanChar( c );
		}

		if ( c == 0 )
		{
			break;
		}
	}

	string[sizeof( string ) - 1] = 0;

	return string;
}

float MSG_ReadAngle16( msg_t *msg )
{
	return SHORT2ANGLE( MSG_ReadShort( msg ) );
}

void MSG_ReadData( msg_t *msg, void *data, int len )
{
	int newcount = msg->readcount + len;

	if ( newcount <= msg->cursize )
	{
		memcpy( data, &msg->data[msg->readcount], len );
		msg->readcount = newcount;
		return;
	}

	msg->overflowed = qtrue;
	memset( data, -1, len );
}

void MSG_WriteDeltaKey( msg_t *msg, int key, int oldV, int newV, int bits )
{
	if ( oldV == newV )
	{
		MSG_WriteBit0( msg );
	}
	else
	{
		MSG_WriteBit1( msg );
		MSG_WriteBits( msg, newV ^ key, bits );
	}
}

int MSG_ReadDeltaKey( msg_t *msg, int key, int oldV, int bits )
{
	if ( MSG_ReadBit( msg ) )
	{
		return MSG_ReadBits( msg, bits ) ^ ( key & kbitmask[bits] );
	}

	return oldV;
}

void MSG_WriteKey( msg_t *msg, int key, int newV, int bits )
{
	MSG_WriteBits( msg, newV ^ key, bits );
}

int MSG_ReadKey( msg_t *msg, int key, int bits )
{
	return MSG_ReadBits( msg, bits ) ^ ( key & kbitmask[bits] );
}

void MSG_WriteDeltaKeyByte( msg_t *msg, int key, int oldV, int newV )
{
	if ( (byte)oldV == (byte)newV )
	{
		MSG_WriteBit0( msg );
		return;
	}

	MSG_WriteBit1( msg );
	MSG_WriteByte( msg, newV ^ key );
}

int MSG_ReadDeltaKeyByte( msg_t *msg, int key, int oldV )
{
	if ( MSG_ReadBit( msg ) )
	{
		return (byte)( MSG_ReadByte( msg ) ^ key );
	}

	return oldV;
}

void MSG_WriteDeltaKeyShort( msg_t *msg, int key, int oldV, int newV )
{
	if ( (short)oldV == (short)newV )
	{
		MSG_WriteBit0( msg );
	}
	else
	{
		MSG_WriteBit1( msg );
		MSG_WriteShort( msg, newV ^ key );
	}
}

int MSG_ReadDeltaKeyShort( msg_t *msg, int key, int oldV )
{
	if ( MSG_ReadBit( msg ) )
	{
		return (short)( MSG_ReadShort( msg ) ^ key );
	}

	return oldV;
}

void MSG_WriteReliableCommandToBuffer( const char *pszCommand, char *pszBuffer, int iBufferSize )
{
	int i;
	int iCommandLength;
	char *p;

	iCommandLength = strlen( pszCommand );

	if ( iCommandLength >= iBufferSize )
	{
		Com_Printf( "WARNING: Reliable command is too long (%i/%i) and will be truncated: '%s'\n", iCommandLength, iBufferSize, pszCommand );
	}

	if ( !iCommandLength )
	{
		Com_Printf( "WARNING: Empty reliable command\n" );
	}

	p = pszBuffer;

	for ( i = 0; i < iBufferSize && pszCommand[i]; i++, p++ )
	{
		*p = I_CleanChar( pszCommand[i] );

		if ( *p == '%' )
		{
			*p = '.';
		}
	}

	if ( i < iBufferSize )
	{
		pszBuffer[i] = 0;
	}
	else
	{
		pszBuffer[iBufferSize - 1] = 0;
	}
}

void MSG_SetDefaultUserCmd( playerState_t *ps, usercmd_t *cmd )
{
	int i;

	memset( cmd, 0, sizeof( *cmd ) );
	cmd->weapon = ps->weapon;
	cmd->offHandIndex = ps->offHandIndex;

	for ( i = 0; i <= 1; ++i )
	{
		cmd->angles[i] = ( ANGLE2SHORT( ps->viewangles[i] ) - ps->delta_angles[i] ) & 65535;
	}

	if ( !( ps->pm_flags & PMF_PLAYER ) )
	{
		goto end;
	}

	if ( ps->eFlags & EF_PRONE )
	{
		cmd->buttons |= BUTTON_PRONE;
	}
	else if ( ps->eFlags & EF_CROUCH )
	{
		cmd->buttons |= BUTTON_CROUCH;
	}

	if ( ps->leanf > 0.0f )
	{
		cmd->buttons |= BUTTON_LEANRIGHT;
	}
	else if ( ps->leanf < 0.0f )
	{
		cmd->buttons |= BUTTON_LEANLEFT;
	}

	if ( ps->fWeaponPosFrac != 0.0f )
	{
		cmd->buttons |= BUTTON_ADS;
	}
end:;
}

int MSG_GetMoveKeyMask( int forwardmove, int rightmove )
{
	int keys = 0;

	if ( forwardmove > 10 )
	{
		keys |= KEY_MASK_FORWARD;
	}
	else if ( forwardmove < -10 )
	{
		keys |= KEY_MASK_BACK;
	}

	if ( rightmove > 10 )
	{
		keys |= KEY_MASK_MOVERIGHT;
	}
	else if ( rightmove < -10 )
	{
		keys |= KEY_MASK_MOVELEFT;
	}

	return keys;
}

void MSG_UnmaskMoveKey( int keys, char *forwardmove, char *rightmove )
{
	if ( (bool)( keys & KEY_MASK_FORWARD ) )
	{
		*forwardmove = 127;
	}
	else if ( keys & KEY_MASK_BACK )
	{
		*forwardmove = -127;
	}
	else
	{
		*forwardmove = 0;
	}

	if ( keys & KEY_MASK_MOVERIGHT )
	{
		*rightmove = 127;
	}
	else if ( keys & KEY_MASK_MOVELEFT )
	{
		*rightmove = -127;
	}
	else
	{
		*rightmove = 0;
	}
}

void MSG_WriteDeltaUsercmdKey( msg_t *msg, int key, usercmd_t *from, usercmd_t *to )
{
	int newKeys;
	int oldKeys;
	bool keysEqual;
	unsigned int delta;

	delta = to->serverTime - from->serverTime;

	if ( delta < 256 )
	{
		MSG_WriteBit1( msg );
		MSG_WriteByte( msg, delta );
	}
	else
	{
		MSG_WriteBit0( msg );
		MSG_WriteLong( msg, to->serverTime );
	}

	newKeys = MSG_GetMoveKeyMask( to->forwardmove, to->rightmove );
	oldKeys = MSG_GetMoveKeyMask( from->forwardmove, from->rightmove );
	keysEqual = oldKeys == newKeys;

	if ( from->buttons >> 1 == to->buttons >> 1 && from->weapon == to->weapon && from->offHandIndex == to->offHandIndex && from->angles[2] == to->angles[2] )
	{
		if ( from->angles[0] == to->angles[0] && from->angles[1] == to->angles[1] && ( from->buttons & 1 ) == ( to->buttons & 1 ) && keysEqual )
		{
			MSG_WriteKey( msg, key, 0, 1 ); // no change
			return;
		}

		MSG_WriteKey( msg, key, 1, 1 );
		MSG_WriteKey( msg, key, 0, 1 );
		key ^= to->serverTime;
		MSG_WriteKey( msg, key, to->buttons, 1 );
		MSG_WriteDeltaKeyShort( msg, key, from->angles[0], to->angles[0] );
		MSG_WriteDeltaKeyShort( msg, key, from->angles[1], to->angles[1] );
		MSG_WriteDeltaKey( msg, key, oldKeys, newKeys, 4 );
	}
	else
	{
		MSG_WriteKey( msg, key, 1, 1 );
		MSG_WriteKey( msg, key, 1, 1 );
		MSG_WriteKey( msg, key, to->buttons, 1 );
		MSG_WriteDeltaKeyShort( msg, key, from->angles[0], to->angles[0] );
		MSG_WriteDeltaKeyShort( msg, key, from->angles[1], to->angles[1] );
		MSG_WriteDeltaKey( msg, key, oldKeys, newKeys, 4 );
		key ^= to->serverTime;
		MSG_WriteDeltaKeyShort( msg, key, from->angles[2], to->angles[2] );
		MSG_WriteDeltaKey( msg, key, from->buttons >> 1, to->buttons >> 1, 18 );
		MSG_WriteDeltaKey( msg, key, from->weapon, to->weapon, 7 );
		MSG_WriteDeltaKey( msg, key, from->offHandIndex, to->offHandIndex, 7 );
	}
}

void MSG_ReadDeltaUsercmdKey( msg_t *msg, int key, usercmd_t *from, usercmd_t *to )
{
	int keys;
	int oldKeys;

	*to = *from;

	if ( MSG_ReadBit( msg ) )
	{
		to->serverTime = from->serverTime + MSG_ReadByte( msg );
	}
	else
	{
		to->serverTime = MSG_ReadLong( msg );
	}

	if ( !MSG_ReadKey( msg, key, 1 ) )
	{
		return;
	}

	to->buttons &= ~1;

	if ( !MSG_ReadKey( msg, key, 1 ) )
	{
		key ^= to->serverTime;

		to->buttons |= MSG_ReadKey( msg, key, 1 );
		to->angles[0] = (unsigned short)MSG_ReadDeltaKeyShort( msg, key, from->angles[0] );
		to->angles[1] = (unsigned short)MSG_ReadDeltaKeyShort( msg, key, from->angles[1] );
		oldKeys = MSG_GetMoveKeyMask( from->forwardmove, from->rightmove );
		keys = MSG_ReadDeltaKey( msg, key, oldKeys, 4 );
		MSG_UnmaskMoveKey( keys, &to->forwardmove, &to->rightmove );
		return;
	}

	to->buttons |= MSG_ReadKey( msg, key, 1 );
	to->angles[0] = (unsigned short)MSG_ReadDeltaKeyShort( msg, key, from->angles[0] );
	to->angles[1] = (unsigned short)MSG_ReadDeltaKeyShort( msg, key, from->angles[1] );
	oldKeys = MSG_GetMoveKeyMask( from->forwardmove, from->rightmove );
	keys = MSG_ReadDeltaKey( msg, key, oldKeys, 4 );
	MSG_UnmaskMoveKey( keys, &to->forwardmove, &to->rightmove );

	key ^= to->serverTime;

	to->angles[2] = (unsigned short)MSG_ReadDeltaKeyShort( msg, key, from->angles[2] );
	to->buttons &= 1u;
	to->buttons |= 2 * MSG_ReadDeltaKey( msg, key, from->buttons >> 1, 18 );
	to->weapon = MSG_ReadDeltaKey( msg, key, from->weapon, 7 );
	to->offHandIndex = MSG_ReadDeltaKey( msg, key, from->offHandIndex, 7 );
}

// bits 0: float, sent as a 13-bit biased integer when it is one
// bits -99: hud coordinate, the same with a 10-bit bias
// bits -100: angle, sent as 16 bits
// bits < 0 otherwise: signed integer of -bits
void MSG_WriteDeltaField( msg_t *msg, byte *from, byte *to, const netField_t *field )
{
	int *fromF;
	float *toF;
	int trunc;
	float fullFloat;
	int value;
	int bits;
	int partialBits;

	fromF = (int *)( from + field->offset );
	toF = (float *)( to + field->offset );

	if ( *fromF == *(int *)toF )
	{
		MSG_WriteBit0( msg );
		return;
	}

	MSG_WriteBit1( msg );

	if ( field->bits == 0 )
	{
		fullFloat = *toF;
		trunc = (int)fullFloat;

		if ( fullFloat == 0.0f )
		{
			MSG_WriteBit0( msg );
			return;
		}

		MSG_WriteBit1( msg );

		if ( (long double)trunc == fullFloat && trunc + FLOAT_INT_BIAS >= 0 && trunc + FLOAT_INT_BIAS < ( 1 << FLOAT_INT_BITS ) )
		{
			MSG_WriteBit0( msg );
			trunc += FLOAT_INT_BIAS;
			MSG_WriteBits( msg, trunc, 5 );
			MSG_WriteByte( msg, trunc >> 5 );
		}
		else
		{
			MSG_WriteBit1( msg );
			MSG_WriteLong( msg, *(int *)toF );
		}
	}
	else if ( field->bits == -99 )
	{
		fullFloat = *toF;
		trunc = (int)fullFloat;

		if ( fullFloat == 0.0f )
		{
			MSG_WriteBit0( msg );
			return;
		}

		MSG_WriteBit1( msg );

		if ( (long double)trunc == fullFloat && trunc + HUDELEM_COORD_BIAS >= 0 && trunc + HUDELEM_COORD_BIAS < ( 1 << HUDELEM_COORD_BITS ) )
		{
			MSG_WriteBit0( msg );
			trunc += HUDELEM_COORD_BIAS;
			MSG_WriteBits( msg, trunc, 2 );
			MSG_WriteByte( msg, trunc >> 2 );
		}
		else
		{
			MSG_WriteBit1( msg );
			MSG_WriteLong( msg, *(int *)toF );
		}
	}
	else if ( field->bits == -100 )
	{
		if ( !*(int *)toF )
		{
			MSG_WriteBit0( msg );
		}
		else
		{
			MSG_WriteBit1( msg );
			MSG_WriteAngle16( msg, *toF );
		}
	}
	else
	{
		if ( !*(int *)toF )
		{
			MSG_WriteBit0( msg );
		}
		else
		{
			MSG_WriteBit1( msg );
			value = *(int *)toF;
			bits = field->bits > 0 ? field->bits : -field->bits;

			partialBits = bits & 7;

			if ( partialBits )
			{
				MSG_WriteBits( msg, value, partialBits );
				bits -= partialBits;
				value >>= partialBits;
			}

			while ( bits )
			{
				MSG_WriteByte( msg, value );
				value >>= 8;
				bits -= 8;
			}
		}
	}
}

void MSG_WriteDeltaObjective( msg_t *msg, byte *from, byte *to, qboolean force, int numFields, const netField_t *fields )
{
	int i;
	const netField_t *field;
	int *fromF;
	int *toF;

	if ( !force )
	{
		for ( i = 0; i < numFields; i++ )
		{
			field = &fields[i];
			fromF = (int *)( from + field->offset );
			toF = (int *)( to + field->offset );

			if ( *fromF != *toF )
			{
				goto delta;
			}
		}

		MSG_WriteBit0( msg );
		return;
	}

delta:
	MSG_WriteBit1( msg );

	for ( i = 0; i < numFields; i++ )
	{
		MSG_WriteDeltaField( msg, from, to, &fields[i] );
	}
}

void MSG_WriteDeltaStruct( msg_t *msg, byte *from, byte *to, qboolean force, int numFields, int indexBits, const netField_t *stateFields, qboolean bChangeBit )
{
	int i;
	int lc;
	const netField_t *field;
	int *fromF;
	int *toF;

	if ( !to )
	{
		if ( cl_shownet && ( cl_shownet->current.integer >= 2 || cl_shownet->current.integer == -1 ) )
		{
			Com_Printf( "W|%3i: #%-3i remove\n", msg->cursize, *(int *)from );
		}

		if ( bChangeBit )
		{
			MSG_WriteBit1( msg );
		}

		MSG_WriteBits( msg, *(int *)from, indexBits );
		MSG_WriteBit1( msg );
		return;
	}

	lc = 0;

	for ( i = 0, field = stateFields; i < numFields; i++, field++ )
	{
		fromF = (int *)( from + field->offset );
		toF = (int *)( to + field->offset );

		if ( *fromF != *toF )
		{
			lc = i + 1;
		}
	}

	if ( !lc )
	{
		if ( !force )
		{
			return;
		}

		if ( bChangeBit )
		{
			MSG_WriteBit1( msg );
		}

		MSG_WriteBits( msg, *(int *)to, indexBits );
		MSG_WriteBit0( msg );
		MSG_WriteBit0( msg );
		return;
	}

	if ( bChangeBit )
	{
		MSG_WriteBit1( msg );
	}

	MSG_WriteBits( msg, *(int *)to, indexBits );
	MSG_WriteBit0( msg );
	MSG_WriteBit1( msg );
	MSG_WriteByte( msg, lc );

	for ( i = 0, field = stateFields; i < lc; i++, field++ )
	{
		MSG_WriteDeltaField( msg, from, to, field );
	}
}

void MSG_WriteDeltaEntity( msg_t *msg, entityState_t *from, entityState_t *to, qboolean force )
{
	int numFields;

	numFields = ARRAY_COUNT( entityStateFields );
	MSG_WriteDeltaStruct( msg, (byte *)from, (byte *)to, force, numFields, GENTITYNUM_BITS, entityStateFields, qfalse );
}

void MSG_WriteDeltaArchivedEntity( msg_t *msg, archivedEntity_t *from, archivedEntity_t *to, qboolean force )
{
	int numFields;

	numFields = ARRAY_COUNT( archivedEntityFields );
	MSG_WriteDeltaStruct( msg, (byte *)from, (byte *)to, force, numFields, GENTITYNUM_BITS, archivedEntityFields, qfalse );
}

void MSG_WriteDeltaClient( msg_t *msg, clientState_t *from, clientState_t *to, qboolean force )
{
	int numFields;
	clientState_t nullstate;

	numFields = ARRAY_COUNT( clientStateFields );

	if ( !from )
	{
		from = &nullstate;
		memset( &nullstate, 0, sizeof( nullstate ) );
	}

	MSG_WriteDeltaStruct( msg, (byte *)from, (byte *)to, force, numFields, CLIENTNUM_BITS, clientStateFields, qtrue );
}

void MSG_ReadDeltaField( msg_t *msg, byte *from, byte *to, const netField_t *field, qboolean print )
{
	int *fromF;
	int *toF;
	int trunc;
	int isSigned;
	int bits;
	int partialBits;
	int value;
	int i;

	fromF = (int *)( from + field->offset );
	toF = (int *)( to + field->offset );

	if ( !MSG_ReadBit( msg ) )
	{
		*toF = *fromF;
		return;
	}

	if ( field->bits == 0 )
	{
		if ( !MSG_ReadBit( msg ) )
		{
			*toF = 0;
			return;
		}

		if ( MSG_ReadBit( msg ) == 0 )
		{
			trunc = MSG_ReadBits( msg, 5 );
			trunc = 32 * MSG_ReadByte( msg ) + trunc - FLOAT_INT_BIAS;
			*(float *)toF = (float)trunc;

			if ( print )
			{
				Com_Printf( "%s:%i ", field->name, trunc );
			}
		}
		else
		{
			*toF = MSG_ReadLong( msg );

			if ( print )
			{
				Com_Printf( "%s:%f ", field->name, *(float *)toF );
			}
		}
	}
	else if ( field->bits == -99 )
	{
		if ( !MSG_ReadBit( msg ) )
		{
			*toF = 0;
			return;
		}

		if ( MSG_ReadBit( msg ) == 0 )
		{
			trunc = MSG_ReadBits( msg, 2 );
			trunc = 4 * MSG_ReadByte( msg ) + trunc - HUDELEM_COORD_BIAS;
			*(float *)toF = (float)trunc;

			if ( print )
			{
				Com_Printf( "%s:%i ", field->name, trunc );
			}
		}
		else
		{
			*toF = MSG_ReadLong( msg );

			if ( print )
			{
				Com_Printf( "%s:%f ", field->name, *(float *)toF );
			}
		}
	}
	else if ( field->bits == -100 )
	{
		if ( !MSG_ReadBit( msg ) )
		{
			*toF = 0;
		}
		else
		{
			*(float *)toF = MSG_ReadAngle16( msg );
		}
	}
	else
	{
		if ( !MSG_ReadBit( msg ) )
		{
			*toF = 0;
			return;
		}

		isSigned = field->bits < 0;
		bits = !isSigned ? field->bits : -field->bits;
		partialBits = bits & 7;
		value = partialBits ? MSG_ReadBits( msg, partialBits ) : 0;

		for ( i = partialBits; i < bits; i += 8 )
		{
			value |= MSG_ReadByte( msg ) << i;
		}

		if ( isSigned && ( value & ( 1 << ( bits - 1 ) ) ) )
		{
			value |= ~( ( 1 << bits ) - 1 );
		}

		*toF = value;

		if ( print )
		{
			Com_Printf( "%s:%i ", field->name, value );
		}
	}
}

void MSG_ReadDeltaObjective( msg_t *msg, byte *from, byte *to, int numFields, const netField_t *fields )
{
	int i;
	const netField_t *field;
	int *fromF;
	int *toF;

	if ( MSG_ReadBit( msg ) )
	{
		for ( i = 0; i < numFields; i++ )
		{
			MSG_ReadDeltaField( msg, from, to, &fields[i], qfalse );
		}
	}
	else
	{
		for ( i = 0; i < numFields; i++ )
		{
			field = &fields[i];
			fromF = (int *)( from + field->offset );
			toF = (int *)( to + field->offset );
			*toF = *fromF;
		}
	}
}

qboolean MSG_ReadDeltaStruct( msg_t *msg, byte *from, byte *to, int number, int numFields, int indexBits, const netField_t *stateFields )
{
	int i;
	int lc;
	const netField_t *field;
	int *fromF;
	int *toF;
	qboolean print;

	if ( MSG_ReadBit( msg ) == 1 )
	{
		if ( cl_shownet && ( cl_shownet->current.integer >= 2 || cl_shownet->current.integer == -1 ) )
		{
			Com_Printf( "%3i: #%-3i remove\n", msg->readcount, number );
		}

		return qtrue;
	}

	if ( !MSG_ReadBit( msg ) )
	{
		memcpy( to, from, ( numFields + 1 ) * sizeof( int ) );
		return qfalse;
	}

	lc = MSG_ReadByte( msg );

	if ( lc > numFields )
	{
		msg->overflowed = qtrue;
		return qfalse;
	}

	if ( cl_shownet && ( cl_shownet->current.integer >= 2 || cl_shownet->current.integer == -1 ) )
	{
		print = qtrue;
		Com_Printf( "%3i: #%-3i ", msg->readcount, *(int *)to );
	}
	else
	{
		print = qfalse;
	}

	*(int *)to = number;

	for ( i = 0, field = stateFields; i < lc; i++, field++ )
	{
		MSG_ReadDeltaField( msg, from, to, field, print );
	}

	for ( i = lc, field = stateFields + lc; i < numFields; i++, field++ )
	{
		fromF = (int *)( from + field->offset );
		toF = (int *)( to + field->offset );
		*toF = *fromF;
	}

	return qfalse;
}

qboolean MSG_ReadDeltaEntity( msg_t *msg, entityState_t *from, entityState_t *to, int number )
{
	int numFields;

	numFields = ARRAY_COUNT( entityStateFields );
	return MSG_ReadDeltaStruct( msg, (byte *)from, (byte *)to, number, numFields, GENTITYNUM_BITS, entityStateFields );
}

void MSG_ReadDeltaArchivedEntity( msg_t *msg, archivedEntity_t *from, archivedEntity_t *to, int number )
{
	int numFields;

	numFields = ARRAY_COUNT( archivedEntityFields );
	MSG_ReadDeltaStruct( msg, (byte *)from, (byte *)to, number, numFields, GENTITYNUM_BITS, archivedEntityFields );
}

qboolean MSG_ReadDeltaClient( msg_t *msg, clientState_t *from, clientState_t *to, int number )
{
	int numFields;
	clientState_t nullstate;

	numFields = ARRAY_COUNT( clientStateFields );

	if ( !from )
	{
		from = &nullstate;
		memset( &nullstate, 0, sizeof( nullstate ) );
	}

	return MSG_ReadDeltaStruct( msg, (byte *)from, (byte *)to, number, numFields, CLIENTNUM_BITS, clientStateFields );
}

void MSG_WriteDeltaHudElems( msg_t *msg, hudelem_t *from, hudelem_t *to, int count )
{
	int i;
	unsigned int j;
	int inuse;
	int lc;
	int *fromF;
	int *toF;

	for ( inuse = 0; inuse < count; ++inuse )
	{
		if ( to[inuse].type == HE_TYPE_FREE )
		{
			break;
		}
	}

	MSG_WriteBits( msg, inuse, 5 );

	for ( i = 0; ; ++i )
	{
		if ( i >= inuse )
		{
			break;
		}

		lc = 0;

		for ( j = 0; j < ARRAY_COUNT( hudElemFields ); ++j )
		{
			fromF = (int *)( (byte *)&from[i] + hudElemFields[j].offset );
			toF = (int *)( (byte *)&to[i] + hudElemFields[j].offset );

			if ( *fromF != *toF )
			{
				lc = j;
			}
		}

		MSG_WriteBits( msg, lc, 5 );

		for ( j = 0; (int)j <= lc; ++j )
		{
			MSG_WriteDeltaField( msg, (byte *)&from[i], (byte *)&to[i], &hudElemFields[j] );
		}
	}
}

void MSG_ReadDeltaHudElems( msg_t *msg, hudelem_t *from, hudelem_t *to, int count )
{
	int i;
	unsigned int j;
	int inuse;
	int lc;

	inuse = MSG_ReadBits( msg, 5 );

	for ( i = 0; i < inuse; ++i )
	{
		lc = MSG_ReadBits( msg, 5 );

		for ( j = 0; (int)j <= lc; ++j )
		{
			MSG_ReadDeltaField( msg, (byte *)&from[i], (byte *)&to[i], &hudElemFields[j], qfalse );
		}

		for ( ; j < ARRAY_COUNT( hudElemFields ); ++j )
		{
			*(int *)( (byte *)&to[i] + hudElemFields[j].offset ) = *(int *)( (byte *)&from[i] + hudElemFields[j].offset );
		}
	}

	memset( &to[inuse], 0, sizeof( hudelem_t ) * ( count - inuse ) );
}

void MSG_WriteDeltaPlayerstate( msg_t *msg, playerState_t *from, playerState_t *to )
{
	int i;
	int j;
	int lc;
	playerState_t dummy;
	int statsbits;
	int ammobits[4];
	int clipbits;
	int numFields;
	const netField_t *field;
	int *fromF;
	int *toF;
	float fullFloat;
	int trunc;
	int value;
	int bits;
	int partialBits;

	if ( !from )
	{
		from = &dummy;
		memset( &dummy, 0, sizeof( dummy ) );
	}

	numFields = ARRAY_COUNT( playerStateFields );
	lc = 0;

	for ( i = 0, field = playerStateFields; i < numFields; i++, field++ )
	{
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );

		if ( *fromF != *toF )
		{
			lc = i + 1;
		}
	}

	MSG_WriteByte( msg, lc );

	for ( i = 0, field = playerStateFields; i < lc; i++, field++ )
	{
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );

		if ( *fromF == *toF )
		{
			MSG_WriteBit0( msg );
		}
		else
		{
			MSG_WriteBit1( msg );

			if ( field->bits == 0 )
			{
				fullFloat = *(float *)toF;
				trunc = (int)fullFloat;

				if ( trunc == fullFloat && trunc + FLOAT_INT_BIAS >= 0 && trunc + FLOAT_INT_BIAS < ( 1 << FLOAT_INT_BITS ) )
				{
					MSG_WriteBit0( msg );
					trunc += FLOAT_INT_BIAS;
					MSG_WriteBits( msg, trunc, 5 );
					MSG_WriteByte( msg, trunc >> 5 );
				}
				else
				{
					MSG_WriteBit1( msg );
					MSG_WriteLong( msg, *toF );
				}
			}
			else if ( field->bits == -100 )
			{
				if ( !*toF )
				{
					MSG_WriteBit0( msg );
				}
				else
				{
					MSG_WriteBit1( msg );
					fullFloat = *(float *)toF;
					MSG_WriteAngle16( msg, fullFloat );
				}
			}
			else
			{
				value = *toF;
				bits = field->bits > 0 ? field->bits : -field->bits;
				partialBits = bits & 7;

				if ( partialBits )
				{
					MSG_WriteBits( msg, value, partialBits );
					bits -= partialBits;
					value >>= partialBits;
				}

				while ( bits )
				{
					MSG_WriteByte( msg, value );
					value >>= 8;
					bits -= 8;
				}
			}
		}
	}

	statsbits = 0;

	for ( i = 0; i < MAX_STATS; ++i )
	{
		if ( to->stats[i] != from->stats[i] )
		{
			statsbits |= 1 << i;
		}
	}

	if ( statsbits )
	{
		MSG_WriteBit1( msg );
		MSG_WriteBits( msg, statsbits, MAX_STATS );

		if ( statsbits & 1 )
		{
			MSG_WriteShort( msg, to->stats[0] );
		}

		if ( statsbits & 2 )
		{
			MSG_WriteShort( msg, to->stats[1] );
		}

		if ( statsbits & 4 )
		{
			MSG_WriteShort( msg, to->stats[2] );
		}

		if ( statsbits & 8 )
		{
			MSG_WriteBits( msg, to->stats[3], 6 );
		}

		if ( statsbits & 0x10 )
		{
			MSG_WriteShort( msg, to->stats[4] );
		}

		if ( statsbits & 0x20 )
		{
			MSG_WriteByte( msg, to->stats[5] );
		}
	}
	else
	{
		MSG_WriteBit0( msg );
	}

	for ( j = 0; j < 4; ++j )
	{
		ammobits[j] = 0;

		for ( i = 0; i < 16; ++i )
		{
			if ( to->ammo[16 * j + i] != from->ammo[16 * j + i] )
			{
				ammobits[j] |= 1 << i;
			}
		}
	}

	if ( ammobits[0] || ammobits[1] || ammobits[2] || ammobits[3] )
	{
		MSG_WriteBit1( msg );

		for ( j = 0; j < 4; ++j )
		{
			if ( ammobits[j] )
			{
				MSG_WriteBit1( msg );
				MSG_WriteShort( msg, ammobits[j] );

				for ( i = 0; i < 16; ++i )
				{
					if ( ammobits[j] & ( 1 << i ) )
					{
						MSG_WriteShort( msg, to->ammo[16 * j + i] );
					}
				}
			}
			else
			{
				MSG_WriteBit0( msg );
			}
		}
	}
	else
	{
		MSG_WriteBit0( msg );
	}

	for ( j = 0; j < 4; ++j )
	{
		clipbits = 0;

		for ( i = 0; i < 16; ++i )
		{
			if ( to->ammoclip[16 * j + i] != from->ammoclip[16 * j + i] )
			{
				clipbits |= 1 << i;
			}
		}

		if ( clipbits )
		{
			MSG_WriteBit1( msg );
			MSG_WriteShort( msg, clipbits );

			for ( i = 0; i < 16; ++i )
			{
				if ( clipbits & ( 1 << i ) )
				{
					MSG_WriteShort( msg, to->ammoclip[16 * j + i] );
				}
			}
		}
		else
		{
			MSG_WriteBit0( msg );
		}
	}

	if ( !memcmp( from->objective, to->objective, sizeof( from->objective ) ) )
	{
		MSG_WriteBit0( msg );
	}
	else
	{
		MSG_WriteBit1( msg );

		for ( i = 0; i < MAX_OBJECTIVES; ++i )
		{
			MSG_WriteBits( msg, to->objective[i].state, 3 );
			MSG_WriteDeltaObjective( msg, (byte *)&from->objective[i], (byte *)&to->objective[i], qfalse, ARRAY_COUNT( objectiveFields ), objectiveFields );
		}
	}

	if ( !memcmp( &from->hud, &to->hud, sizeof( from->hud ) ) )
	{
		MSG_WriteBit0( msg );
	}
	else
	{
		MSG_WriteBit1( msg );
		MSG_WriteDeltaHudElems( msg, from->hud.archival, to->hud.archival, MAX_HUDELEMS_ARCHIVAL );
		MSG_WriteDeltaHudElems( msg, from->hud.current, to->hud.current, MAX_HUDELEMS_CURRENT );
	}
}

void MSG_ReadDeltaPlayerstate( msg_t *msg, playerState_t *from, playerState_t *to )
{
	int j;
	int i;
	int lc;
	int bitmask;
	const netField_t *field;
	int numFields;
	qboolean print;
	int *fromF;
	int *toF;
	int trunc;
	int isSigned;
	int partialBits;
	int value;
	playerState_t dummy;

	if ( !from )
	{
		from = &dummy;
		memset( &dummy, 0, sizeof( dummy ) );
	}

	*to = *from;

	if ( cl_shownet && ( cl_shownet->current.integer >= 2 || cl_shownet->current.integer == -2 ) )
	{
		print = qtrue;
		Com_Printf( "%3i: playerstate ", msg->readcount );
	}
	else
	{
		print = qfalse;
	}

	numFields = ARRAY_COUNT( playerStateFields );
	lc = MSG_ReadByte( msg );

	for ( j = 0, field = playerStateFields; j < lc; j++, field++ )
	{
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );

		if ( !MSG_ReadBit( msg ) )
		{
			*toF = *fromF;
		}
		else if ( field->bits == 0 )
		{
			if ( !MSG_ReadBit( msg ) )
			{
				trunc = MSG_ReadBits( msg, 5 );
				trunc += 32 * MSG_ReadByte( msg );
				trunc -= FLOAT_INT_BIAS;
				*(float *)toF = (float)trunc;

				if ( print )
				{
					Com_Printf( "%s:%i ", field->name, trunc );
				}
			}
			else
			{
				*toF = MSG_ReadLong( msg );

				if ( print )
				{
					Com_Printf( "%s:%f ", field->name, *(float *)toF );
				}
			}
		}
		else if ( field->bits == -100 )
		{
			if ( !MSG_ReadBit( msg ) )
			{
				*(float *)toF = 0.0f;
			}
			else
			{
				*(float *)toF = MSG_ReadAngle16( msg );
			}
		}
		else
		{
			isSigned = field->bits < 0;
			bitmask = isSigned ? -field->bits : field->bits;
			partialBits = bitmask & 7;
			value = partialBits ? MSG_ReadBits( msg, partialBits ) : 0;

			for ( i = partialBits; i < bitmask; i += 8 )
			{
				value |= MSG_ReadByte( msg ) << i;
			}

			if ( isSigned && ( value & ( 1 << ( bitmask - 1 ) ) ) )
			{
				value |= ~( ( 1 << bitmask ) - 1 );
			}

			*toF = value;

			if ( print )
			{
				Com_Printf( "%s:%i ", field->name, value );
			}
		}
	}

	for ( j = lc, field = &playerStateFields[lc]; j < numFields; j++, field++ )
	{
		fromF = (int *)( (byte *)from + field->offset );
		toF = (int *)( (byte *)to + field->offset );
		*toF = *fromF;
	}

	if ( MSG_ReadBit( msg ) )
	{
		LOG( "PS_STATS" );
		bitmask = MSG_ReadBits( msg, MAX_STATS );

		if ( bitmask & 1 )
		{
			to->stats[0] = MSG_ReadShort( msg );
		}

		if ( bitmask & 2 )
		{
			to->stats[1] = MSG_ReadShort( msg );
		}

		if ( bitmask & 4 )
		{
			to->stats[2] = MSG_ReadShort( msg );
		}

		if ( bitmask & 8 )
		{
			to->stats[3] = MSG_ReadBits( msg, 6 );
		}

		if ( bitmask & 0x10 )
		{
			to->stats[4] = MSG_ReadShort( msg );
		}

		if ( bitmask & 0x20 )
		{
			to->stats[5] = MSG_ReadByte( msg );
		}
	}

	if ( MSG_ReadBit( msg ) )
	{
		for ( i = 0; i < 4; ++i )
		{
			if ( MSG_ReadBit( msg ) )
			{
				LOG( "PS_AMMO" );
				bitmask = MSG_ReadShort( msg );

				for ( j = 0; j < 16; ++j )
				{
					if ( bitmask & ( 1 << j ) )
					{
						to->ammo[16 * i + j] = MSG_ReadShort( msg );
					}
				}
			}
		}
	}

	for ( i = 0; i < 4; ++i )
	{
		if ( MSG_ReadBit( msg ) )
		{
			LOG( "PS_AMMOCLIP" );
			bitmask = MSG_ReadShort( msg );

			for ( j = 0; j < 16; ++j )
			{
				if ( bitmask & ( 1 << j ) )
				{
					to->ammoclip[16 * i + j] = MSG_ReadShort( msg );
				}
			}
		}
	}

	if ( MSG_ReadBit( msg ) )
	{
		for ( j = 0; j < MAX_OBJECTIVES; ++j )
		{
			to->objective[j].state = MSG_ReadBits( msg, 3 );
			MSG_ReadDeltaObjective( msg, (byte *)&from->objective[j], (byte *)&to->objective[j], ARRAY_COUNT( objectiveFields ), objectiveFields );
		}
	}

	if ( MSG_ReadBit( msg ) )
	{
		MSG_ReadDeltaHudElems( msg, from->hud.archival, to->hud.archival, MAX_HUDELEMS_ARCHIVAL );
		MSG_ReadDeltaHudElems( msg, from->hud.current, to->hud.current, MAX_HUDELEMS_CURRENT );
	}
}

void MSG_initHuffmanInternal()
{
	int i, j;

	Huff_Init( &msgHuff );

	for ( i = 0; i < 256; i++ )
	{
		for ( j = 0; j < msg_hData[i]; j++ )
		{
			Huff_addRef( &msgHuff.compressor, (byte)i );
			Huff_addRef( &msgHuff.decompressor, (byte)i );
		}
	}
}

void MSG_initHuffman()
{
	msgInit = qtrue;
	MSG_initHuffmanInternal();
}

