#include "../qcommon/qcommon.h"
#include "../script/script_public.h"

#define XANIM_VERSION 14

extern void FS_FreeFileInl(void *buffer);

void ConsumeQuat(const unsigned char **pos, short *out)
{
	int unused[2];
	int w;
	int z;
	int y;
	int x;
	int temp;

	out[0] = XModelDataReadShort(pos).value;
	out[1] = XModelDataReadShort(pos).value;
	out[2] = XModelDataReadShort(pos).value;

	x = out[0];
	y = out[1];
	z = out[2];

	temp = 1073676289 - (x * x + y * y + z * z);
	w = temp > 0 ? (int)floor((double)(0.5f + (float)sqrt((double)temp))) : 0;

	out[3] = w;
}

void ConsumeQuat2(const unsigned char **pos, short *out)
{
	int w;
	int x;
	int temp;

	out[0] = XModelDataReadShort(pos).value;

	x = out[0];

	temp = 1073676289 - x * x;
	w = temp > 0 ? (int)floor((double)(0.5f + (float)sqrt((double)temp))) : 0;

	out[1] = w;
}

void XAnimLoadNotifyInfo(const char *name, const unsigned char **pos, XAnimParts *parts, void *(*Alloc)(int))
{
	int count;
	int i;
	int frame;
	XAnimNotifyInfo *notify;

	count = **pos;
	*pos = *pos + 1;
	parts->notifyCount = count + 1;
	notify = (XAnimNotifyInfo *)Alloc(sizeof(XAnimNotifyInfo) * parts->notifyCount);
	parts->notify = notify;

	for ( i = 0; i < count; ++i, ++notify )
	{
		notify->name = SL_GetString_((const char *)*pos, 0, 3);
		*pos = *pos + strlen((const char *)*pos) + 1;
		frame = XModelDataReadUnsignedShort(pos).value;
		notify->time = parts->numframes ? (float)frame / parts->numframes : 0.0f;
	}

	notify->name = SL_GetString_("end", 0, 3);
	notify->time = 1.0;
}

XAnimParts *XAnimLoadFile(const char *name, void *(*Alloc)(int))
{
	int fileSize;
	unsigned short *names;
	XAnimParts *parts;
	short boneCount;
	unsigned short frames;
	unsigned short numloopframes;
	XAnimPart *part;
	XAnimDeltaPart *deltaPart;
	byte *buf;
	const unsigned char *pos;
	const unsigned char *quatBits;
	int m;
	char filename[64];
	unsigned int len;
	short framerate;
	short version;
	unsigned char flags;
	bool bLoop;
	bool bDelta;
	unsigned short numQuatIndices;
	unsigned short numTransIndices;
	bool useSmallIndices;
	short quat[4];
	vec3_t vTrans;
	int n;
	int i;
	short *sQ;
	char *simpleQuatBits;
	bool bFlipQuat;
	bool bSimpleQuat;

	if (Com_sprintf(filename, sizeof(filename), "xanim/%s", name) < 0)
	{
		Com_Printf("^1ERROR: filename '%s' too long\n", filename);
		return NULL;
	}
	fileSize = FS_ReadFile(filename, (void **)&buf);
	if (fileSize < 0)
	{
		Com_Printf("^1ERROR: xanim '%s' not found\n", name);
		return NULL;
	}
	if (fileSize == 0)
	{
		Com_Printf("^1ERROR: xanim '%s' has 0 length\n", name);
		FS_FreeFile(buf);
		return NULL;
	}
	pos = buf;
	version = XModelDataReadShort(&pos).value;
	if (version != XANIM_VERSION)
	{
		FS_FreeFile(buf);
		Com_Printf("^1ERROR: xanim '%s' out of date (version %d, expecting %d)\n", name, version, XANIM_VERSION);
		return NULL;
	}
	{
		frames = XModelDataReadShort(&pos).value;
		boneCount = XModelDataReadShort(&pos).value;
		names = boneCount ? (unsigned short *)Alloc(2 * boneCount) : 0;
		flags = *pos;
		pos += 1;
		bLoop = flags & 1;
		bDelta = (flags & 2) != 0;
		framerate = XModelDataReadShort(&pos).value;
		parts = (XAnimParts *)Alloc(44);
		parts->boneCount = boneCount;
		parts->names = names;
		parts->framerate = (float)framerate;
		parts->bLoop = bLoop;
		parts->bDelta = bDelta;
		numloopframes = bLoop ? frames + 1 : frames;
		useSmallIndices = numloopframes <= 0x100u;
		parts->numframes = numloopframes - 1;
		parts->frequency = parts->numframes ? parts->framerate / (float)parts->numframes : 0.0f;
		if (bDelta)
		{
			deltaPart = (XAnimDeltaPart *)Alloc(8);
			parts->deltaPart = deltaPart;
			numQuatIndices = XModelDataReadUnsignedShort(&pos).value;
			if (!numQuatIndices)
			{
				deltaPart->quat = 0;
			}
			else
			{
				if (numQuatIndices == 1)
				{
					ConsumeQuat2(&pos, quat);
					deltaPart->quat = (XAnimDeltaPartQuat *)Alloc(8);
					deltaPart->quat->size = 0;
					deltaPart->quat->u.frame0[0] = quat[0];
					deltaPart->quat->u.frame0[1] = quat[1];
				}
				else
				{
					if (numQuatIndices < numloopframes)
					{
						if (useSmallIndices)
						{
							deltaPart->quat = (XAnimDeltaPartQuat *)Alloc(numQuatIndices + 8);
							n = numQuatIndices;
							memcpy(deltaPart->quat->u.frames.indices._1, pos, n);
							pos += n;
						}
						else
						{
							deltaPart->quat = (XAnimDeltaPartQuat *)Alloc(2 * numQuatIndices + 8);
							n = 2 * numQuatIndices;
							memcpy(deltaPart->quat->u.frames.indices._1, pos, n);
							pos += n;
						}
					}
					else
					{
						deltaPart->quat = (XAnimDeltaPartQuat *)Alloc(8);
					}
					deltaPart->quat->size = numQuatIndices - 1;
					deltaPart->quat->u.frames.frames = (short (*)[2])Alloc(4 * numQuatIndices);
					for (i = 0; i < numQuatIndices; ++i)
					{
						ConsumeQuat2(&pos, quat);
						deltaPart->quat->u.frames.frames[i][0] = quat[0];
						deltaPart->quat->u.frames.frames[i][1] = quat[1];
					}
					for (i = 1; i < numQuatIndices; ++i)
					{
						short *sQ = deltaPart->quat->u.frames.frames[i];
						short *temp2 = sQ - 2;
						int temp = sQ[0] * temp2[0] + sQ[1] * temp2[1];
						if (temp < 0)
						{
							sQ[0] = -sQ[0];
							sQ[1] = -sQ[1];
						}
					}
				}
			}
			numTransIndices = XModelDataReadUnsignedShort(&pos).value;
			if (!numTransIndices)
			{
				deltaPart->trans = 0;
			}
			else
			{
				if (numTransIndices == 1)
				{
					vTrans[0] = XModelDataReadFloat(&pos);
					vTrans[1] = XModelDataReadFloat(&pos);
					vTrans[2] = XModelDataReadFloat(&pos);
					deltaPart->trans = (XAnimDeltaPartTrans *)Alloc(16);
					deltaPart->trans->size = 0;
					VectorCopy(vTrans, deltaPart->trans->u.frame0);
				}
				else
				{
					if (numTransIndices < numloopframes)
					{
						if (useSmallIndices)
						{
							deltaPart->trans = (XAnimDeltaPartTrans *)Alloc(numTransIndices + 8);
							n = numTransIndices;
							memcpy(deltaPart->trans->u.frames.indices._1, pos, n);
							pos += n;
						}
						else
						{
							deltaPart->trans = (XAnimDeltaPartTrans *)Alloc(2 * numTransIndices + 8);
							n = 2 * numTransIndices;
							memcpy(deltaPart->trans->u.frames.indices._1, pos, n);
							pos += n;
						}
					}
					else
					{
						deltaPart->trans = (XAnimDeltaPartTrans *)Alloc(8);
					}
					deltaPart->trans->size = numTransIndices - 1;
					deltaPart->trans->u.frames.frames = (float (*)[3])Alloc(12 * numTransIndices);
					for (i = 0; i < numTransIndices; ++i)
					{
						deltaPart->trans->u.frames.frames[i][0] = XModelDataReadFloat(&pos);
						deltaPart->trans->u.frames.frames[i][1] = XModelDataReadFloat(&pos);
						deltaPart->trans->u.frames.frames[i][2] = XModelDataReadFloat(&pos);
					}
				}
			}
		}
		if (boneCount)
		{
			len = ((boneCount - 1) >> 3) + 1;
			quatBits = pos;
			pos += len;
			simpleQuatBits = (char *)Alloc(len);
			memcpy(simpleQuatBits, pos, len);
			pos += len;
			parts->simpleQuatBits = simpleQuatBits;
			parts->parts = (XAnimPart *)Alloc(8 * boneCount);
		}
		else
		{
			quatBits = 0;
			simpleQuatBits = 0;
		}
		for (m = 0; m < boneCount; ++m)
		{
			len = strlen((const char *)pos) + 1;
			names[m] = SL_GetStringOfLen((const char *)pos, 0, len, 9);
			pos += len;
		}
		for (m = 0; m < boneCount; ++m)
		{
			bFlipQuat = ( 1 << (m & 7) ) & quatBits[m >> 3];
			bSimpleQuat = ( 1 << (m & 7) ) & simpleQuatBits[m >> 3];
			part = &parts->parts[m];
			numQuatIndices = XModelDataReadUnsignedShort(&pos).value;
			if (!numQuatIndices)
			{
				part->quat = 0;
			}
			else
			{
				if (numQuatIndices == 1)
				{
					if (bSimpleQuat)
					{
						ConsumeQuat2(&pos, quat);
						if (bFlipQuat)
						{
							quat[0] = -quat[0];
							quat[1] = -quat[1];
						}
						part->quat = (XAnimPartQuat *)Alloc(8);
						part->quat->u.frame0[0] = quat[0];
						part->quat->u.frame0[1] = quat[1];
					}
					else
					{
						ConsumeQuat(&pos, quat);
						if (bFlipQuat)
						{
							quat[0] = -quat[0];
							quat[1] = -quat[1];
							quat[2] = -quat[2];
							quat[3] = -quat[3];
						}
						part->quat = (XAnimPartQuat *)Alloc(12);
						part->quat->u.frame0[0] = quat[0];
						part->quat->u.frame0[1] = quat[1];
						part->quat->u.frame0[2] = quat[2];
						part->quat->u.frame0[3] = quat[3];
					}
					part->quat->size = 0;
				}
				else
				{
					if (numQuatIndices < numloopframes)
					{
						if (useSmallIndices)
						{
							part->quat = (XAnimPartQuat *)Alloc(numQuatIndices + 8);
							n = numQuatIndices;
							memcpy(part->quat->u.frames.indices._1, pos, n);
							pos += n;
						}
						else
						{
							part->quat = (XAnimPartQuat *)Alloc(2 * numQuatIndices + 8);
							n = 2 * numQuatIndices;
							memcpy(part->quat->u.frames.indices._1, pos, n);
							pos += n;
						}
					}
					else
					{
						part->quat = (XAnimPartQuat *)Alloc(8);
					}
					if (bSimpleQuat)
					{
						part->quat->u.frames.u.frames2 = (short (*)[2])Alloc(4 * numQuatIndices);
						ConsumeQuat2(&pos, quat);
						if (bFlipQuat)
						{
							quat[0] = -quat[0];
							quat[1] = -quat[1];
						}
						(*part->quat->u.frames.u.frames2)[0] = quat[0];
						(*part->quat->u.frames.u.frames2)[1] = quat[1];
						for (i = 1; i < numQuatIndices; ++i)
						{
							ConsumeQuat2(&pos, quat);
							(*part->quat->u.frames.u.frames2)[2 * i] = quat[0];
							(*part->quat->u.frames.u.frames2)[2 * i + 1] = quat[1];
						}
						for (i = 1; i < numQuatIndices; ++i)
						{
							sQ = &(*part->quat->u.frames.u.frames2)[2 * i];
							short *temp2 = sQ - 2;
							int temp = sQ[0] * temp2[0] + sQ[1] * temp2[1];
							if (temp < 0)
							{
								sQ[0] = -sQ[0];
								sQ[1] = -sQ[1];
							}
						}
					}
					else
					{
						part->quat->u.frames.u.frames = (short (*)[4])Alloc(8 * numQuatIndices);
						ConsumeQuat(&pos, quat);
						if (bFlipQuat)
						{
							quat[0] = -quat[0];
							quat[1] = -quat[1];
							quat[2] = -quat[2];
							quat[3] = -quat[3];
						}
						(*part->quat->u.frames.u.frames)[0] = quat[0];
						(*part->quat->u.frames.u.frames)[1] = quat[1];
						(*part->quat->u.frames.u.frames)[2] = quat[2];
						(*part->quat->u.frames.u.frames)[3] = quat[3];
						for (i = 1; i < numQuatIndices; ++i)
						{
							ConsumeQuat(&pos, quat);
							(*part->quat->u.frames.u.frames)[4 * i] = quat[0];
							(*part->quat->u.frames.u.frames)[4 * i + 1] = quat[1];
							(*part->quat->u.frames.u.frames)[4 * i + 2] = quat[2];
							(*part->quat->u.frames.u.frames)[4 * i + 3] = quat[3];
						}
						for (i = 1; i < numQuatIndices; ++i)
						{
							sQ = &(*part->quat->u.frames.u.frames)[4 * i];
							short *temp2 = sQ - 4;
							int temp = (sQ[2] * temp2[2] + sQ[3] * temp2[3]) + (sQ[0] * temp2[0] + sQ[1] * temp2[1]);
							if (temp < 0)
							{
								sQ[0] = -sQ[0];
								sQ[1] = -sQ[1];
								sQ[2] = -sQ[2];
								sQ[3] = -sQ[3];
							}
						}
					}
					part->quat->size = numQuatIndices - 1;
				}
			}
			numTransIndices = XModelDataReadUnsignedShort(&pos).value;
			if (!numTransIndices)
			{
				part->trans = 0;
			}
			else
			{
				if (numTransIndices == 1)
				{
					vTrans[0] = XModelDataReadFloat(&pos);
					vTrans[1] = XModelDataReadFloat(&pos);
					vTrans[2] = XModelDataReadFloat(&pos);
					part->trans = (XAnimPartTrans *)Alloc(16);
					part->trans->size = 0;
					VectorCopy(vTrans, part->trans->u.frame0);
				}
				else
				{
					if (numTransIndices < numloopframes)
					{
						if (useSmallIndices)
						{
							part->trans = (XAnimPartTrans *)Alloc(numTransIndices + 8);
							n = numTransIndices;
							memcpy(part->trans->u.frames.indices._1, pos, n);
							pos += n;
						}
						else
						{
							part->trans = (XAnimPartTrans *)Alloc(2 * numTransIndices + 8);
							n = 2 * numTransIndices;
							memcpy(part->trans->u.frames.indices._1, pos, n);
							pos += n;
						}
					}
					else
					{
						part->trans = (XAnimPartTrans *)Alloc(8);
					}
					part->trans->size = numTransIndices - 1;
					part->trans->u.frames.frames = (float (*)[3])Alloc(12 * numTransIndices);
					for (i = 0; i < numTransIndices; ++i)
					{
						part->trans->u.frames.frames[i][0] = XModelDataReadFloat(&pos);
						part->trans->u.frames.frames[i][1] = XModelDataReadFloat(&pos);
						part->trans->u.frames.frames[i][2] = XModelDataReadFloat(&pos);
					}
				}
			}
		}
		XAnimLoadNotifyInfo(name, &pos, parts, Alloc);
		FS_FreeFileInl(buf);
		return parts;
	}
}


