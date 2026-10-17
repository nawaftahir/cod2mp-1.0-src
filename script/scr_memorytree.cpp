#include "../qcommon/qcommon.h"
#include "script_public.h"

scrMemTreeGlob_t scrMemTreeGlob;
scrMemTreePub_t scrMemTreePub;

int MT_GetSubTreeSize( int nodeNum );
void MT_DumpTree();
int Scr_GetStringUsage();
void MT_InitBits();
int MT_GetScore( int num );
void MT_AddMemoryNode( int newNode, int size );
bool MT_RemoveMemoryNode( int oldNode, int size );
void MT_RemoveHeadMemoryNode( int size );
void MT_Init();
void MT_Error( const char *funcName, int numBytes );
int MT_GetSize( int numBytes );
unsigned short MT_AllocIndex( int numBytes, int type );
void MT_FreeIndex( unsigned int nodeNum, int numBytes );
void* MT_Alloc( int numBytes, int type );
void MT_Free( void *p, int numBytes );
byte *MT_InitForceAlloc();
void MT_ForceAllocIndex( byte *allocBits, unsigned int nodeNum, int numBytes );
void MT_FinishForceAlloc( byte *allocBits );
int MT_Realloc( int oldNumBytes, int newNumbytes );

/*
============
MT_Alloc
============
*/
// 1.0 MT_Alloc passes the allocation type through; the sanitized header still
// declares the later 1-arg form, so alias the 2-arg call onto the same symbol.

/*
==============
MT_GetSubTreeSize
==============
*/
int MT_GetSubTreeSize( int nodeNum )
{
	MemoryNode *node;

	if ( !nodeNum )
	{
		return 0;
	}

	node = &scrMemTreeGlob.nodes[nodeNum];

	return MT_GetSubTreeSize(node->prev) + MT_GetSubTreeSize(node->next) + 1;
}

/*
============
MT_DumpTree
============
*/
void MT_DumpTree()
{
	int nodeNum;
	int subTreeSize;
	int buckets;
	int totalBuckets;

	Com_Printf("********************************\n");
	totalBuckets = scrMemTreeGlob.totalAllocBuckets;

	for ( nodeNum = 0; nodeNum <= MEMORY_NODE_BITS; nodeNum++ )
	{
		subTreeSize = MT_GetSubTreeSize(scrMemTreeGlob.head[nodeNum]);
		buckets = subTreeSize * (1 << nodeNum);
		totalBuckets += buckets;
		Com_Printf("%d subtree has %d * %d = %d free buckets\n", nodeNum, subTreeSize, 1 << nodeNum, buckets);
	}

	Com_Printf("********************************\n");
	Com_Printf("********************************\n");
	Com_Printf("total memory alloc buckets: %d (%d instances)\n", scrMemTreeGlob.totalAllocBuckets, scrMemTreeGlob.totalAlloc);
	Com_Printf("total memory free buckets: %d\n", MEMORY_NODE_COUNT - 1 - scrMemTreeGlob.totalAllocBuckets);
	Com_Printf("********************************\n");
}

/*
============
Scr_GetStringUsage
============
*/
int Scr_GetStringUsage()
{
	return scrMemTreeGlob.totalAllocBuckets;
}

/*
==============
MT_InitBits
==============
*/
void MT_InitBits()
{
	int i;
	int bits;
	int temp;

	for (i = 0; i < 256; i++)
	{
		bits = 0;
		for (temp = i; temp; temp >>= 1)
		{
			if (temp & 1)
			{
				bits++;
			}
		}

		scrMemTreeGlob.numBits[i] = bits;
		for (bits = 8; i & ((1 << bits) - 1); bits--)
		{
		}

		scrMemTreeGlob.leftBits[i] = bits;
		bits = 0;
		for (temp = i; temp; temp >>= 1)
		{
			bits++;
		}
		scrMemTreeGlob.logBits[i] = bits;
	}
}

/*
============
MT_GetScore
============
*/
int MT_GetScore( int num )
{
	int score;
	int bits;
	int hi;
	int lo;

	num = MEMORY_NODE_COUNT - num;

	lo = ((byte *)&num)[0];
	hi = ((byte *)&num)[1];

	score = num - ( ( (byte *)scrMemTreeGlob.numBits )[lo] + ( (byte *)scrMemTreeGlob.numBits )[hi] );
	bits = ( (byte *)scrMemTreeGlob.leftBits )[lo];

	if ( !lo )
	{
		bits += ( (byte *)scrMemTreeGlob.leftBits )[hi];
	}

	return (1 << bits) + score;
}

/*
==============
MT_AddMemoryNode
==============
*/
void MT_AddMemoryNode( int newNode, int size )
{
	int node;
	uint16_t* parentNode;
	int newScore;
	int nodeNum;
	int level;
	int score;

	assert(size >= 0 && size <= MEMORY_NODE_BITS);

	parentNode = &scrMemTreeGlob.head[size];
	node = *parentNode;
	if (node)
	{
		newScore = MT_GetScore(newNode);
		nodeNum = 0;
		level = MEMORY_NODE_COUNT;
		do
		{
			assert(newNode != node);
			score = MT_GetScore(node);

			assert(score != newScore);

			if (score < newScore)
			{
				while (1)
				{
					assert(node == *parentNode);
					assert(node != newNode);

					*parentNode = newNode;
					scrMemTreeGlob.nodes[newNode] = scrMemTreeGlob.nodes[node];
					if (!node)
					{
						break;
					}
					level >>= 1;

					assert(node != nodeNum);

					if (node < nodeNum)
					{
						parentNode = &scrMemTreeGlob.nodes[newNode].prev;
						nodeNum -= level;
					}
					else
					{
						parentNode = &scrMemTreeGlob.nodes[newNode].next;
						nodeNum += level;
					}
					newNode = node;
					node = *parentNode;
				}
				return;
			}
			level >>= 1;

			assert(newNode != nodeNum);

			if (newNode < nodeNum)
			{
				parentNode = &scrMemTreeGlob.nodes[node].prev;
				nodeNum -= level;
			}
			else
			{
				parentNode = &scrMemTreeGlob.nodes[node].next;
				nodeNum += level;
			}

			node = *parentNode;
		}
		while (node);
	}

	*parentNode = newNode;
	scrMemTreeGlob.nodes[newNode].prev = 0;
	scrMemTreeGlob.nodes[newNode].next = 0;
}

/*
==============
MT_RemoveMemoryNode
==============
*/
bool MT_RemoveMemoryNode( int oldNode, int size )
{
	int node;
	uint16_t* parentNode;
	int prevScore;
	int nextScore;
	int nodeNum;
	int level;
	MemoryNode oldNodeValue;
	MemoryNode tempNodeValue;

	assert(size >= 0 && size <= MEMORY_NODE_BITS);

	nodeNum = 0;
	level = MEMORY_NODE_COUNT;
	parentNode = &scrMemTreeGlob.head[size];
	for (node = *parentNode; node; node = *parentNode)
	{
		if (oldNode == node)
		{
			oldNodeValue = scrMemTreeGlob.nodes[oldNode];

			while (1)
			{
				if (!oldNodeValue.prev)
				{
					oldNode = oldNodeValue.next;
					*parentNode = oldNode;

					if (!oldNode)
					{
						return true;
					}

					parentNode = &scrMemTreeGlob.nodes[oldNode].next;
				}
				else if (!oldNodeValue.next)
				{
					oldNode = oldNodeValue.prev;
					*parentNode = oldNode;
					parentNode = &scrMemTreeGlob.nodes[oldNode].prev;
				}
				else
				{
					prevScore = MT_GetScore(oldNodeValue.prev);
					nextScore = MT_GetScore(oldNodeValue.next);

					assert(prevScore != nextScore);

					if (prevScore < nextScore)
					{
						oldNode = oldNodeValue.next;
						*parentNode = oldNode;
						parentNode = &scrMemTreeGlob.nodes[oldNode].next;
					}
					else
					{
						oldNode = oldNodeValue.prev;
						*parentNode = oldNode;
						parentNode = &scrMemTreeGlob.nodes[oldNode].prev;
					}
				}

				assert(oldNode != 0);

				tempNodeValue = oldNodeValue;
				oldNodeValue = scrMemTreeGlob.nodes[oldNode];
				scrMemTreeGlob.nodes[oldNode] = tempNodeValue;
			}
		}

		if (oldNode == nodeNum)
		{
			return false;
		}

		level >>= 1;
		if (oldNode < nodeNum)
		{
			parentNode = &scrMemTreeGlob.nodes[node].prev;
			nodeNum -= level;
		}
		else
		{
			parentNode = &scrMemTreeGlob.nodes[node].next;
			nodeNum += level;
		}
	}

	return false;
}

/*
==============
MT_RemoveHeadMemoryNode
==============
*/
void MT_RemoveHeadMemoryNode( int size )
{
	int oldNode;
	uint16_t* parentNode;
	int prevScore;
	int nextScore;
	MemoryNode oldNodeValue;
	MemoryNode tempNodeValue;

	assert(size >= 0 && size <= MEMORY_NODE_BITS);

	parentNode = &scrMemTreeGlob.head[size];
	oldNode = scrMemTreeGlob.head[size];
	oldNodeValue = scrMemTreeGlob.nodes[oldNode];

	while (1)
	{
		if (!oldNodeValue.prev)
		{
			oldNode = oldNodeValue.next;
			*parentNode = oldNode;

			if (!oldNode)
			{
				break;
			}

			parentNode = &scrMemTreeGlob.nodes[oldNode].next;
		}
		else if (!oldNodeValue.next)
		{
			oldNode = oldNodeValue.prev;
			*parentNode = oldNode;
			parentNode = &scrMemTreeGlob.nodes[oldNode].prev;
		}
		else
		{
			prevScore = MT_GetScore(oldNodeValue.prev);
			nextScore = MT_GetScore(oldNodeValue.next);

			assert(prevScore != nextScore);

			if (prevScore < nextScore)
			{
				oldNode = oldNodeValue.next;
				*parentNode = oldNode;
				parentNode = &scrMemTreeGlob.nodes[oldNode].next;
			}
			else
			{
				oldNode = oldNodeValue.prev;
				*parentNode = oldNode;
				parentNode = &scrMemTreeGlob.nodes[oldNode].prev;
			}
		}

		assert(oldNode != 0);

		tempNodeValue = oldNodeValue;
		oldNodeValue = scrMemTreeGlob.nodes[oldNode];
		scrMemTreeGlob.nodes[oldNode] = tempNodeValue;
	}
}

/*
============
MT_Init
============
*/
void MT_Init()
{
	int i;

	scrMemTreePub.mt_buffer = (char *)scrMemTreeGlob.nodes;

	MT_InitBits();

	for ( i = 0; i <= MEMORY_NODE_BITS; i++ )
	{
		scrMemTreeGlob.head[i] = 0;
	}

	scrMemTreeGlob.nodes[0].prev = 0;
	scrMemTreeGlob.nodes[0].next = 0;

	for ( i = 0; i < MEMORY_NODE_BITS; i++ )
	{
		MT_AddMemoryNode(1 << i, i);
	}

	scrMemTreeGlob.totalAlloc = 0;
	scrMemTreeGlob.totalAllocBuckets = 0;
}

/*
==============
MT_Error
==============
*/
void MT_Error( const char *funcName, int numBytes )
{
	MT_DumpTree();
	Com_Printf("%s: failed memory allocation of %d bytes for script usage\n", funcName, numBytes);
	Scr_TerminalError("failed memory allocation for script usage");
}

/*
==============
MT_GetSize
==============
*/
int MT_GetSize( int numBytes )
{
	int numBuckets;
	int size;

	assert(numBytes > 0);

	if ( numBytes < MEMORY_NODE_COUNT )
	{
		numBuckets = (numBytes + 7) / 8 - 1;
		size = numBuckets <= 256 - 1 ? scrMemTreeGlob.logBits[numBuckets] : scrMemTreeGlob.logBits[numBuckets >> 8] + 8;
	}
	else
	{
		MT_Error("MT_GetSize: max allocation exceeded", numBytes);
		size = 0;
	}

	return size;
}

/*
============
MT_AllocIndex
============
*/
unsigned short MT_AllocIndex( int numBytes, int type )
{
	int nodeNum, size, newSize;

	size = MT_GetSize(numBytes);
	assert(size >= 0 && size <= MEMORY_NODE_BITS);

	for ( newSize = size; newSize <= MEMORY_NODE_BITS; newSize++ )
	{
		nodeNum = scrMemTreeGlob.head[newSize];

		if ( !nodeNum )
		{
			continue;
		}

		MT_RemoveHeadMemoryNode(newSize);

		while ( newSize != size )
		{
			newSize--;
			MT_AddMemoryNode(nodeNum + (1 << newSize), newSize);
		}

		scrMemTreeGlob.totalAlloc++;
		scrMemTreeGlob.totalAllocBuckets += 1 << size;

		return nodeNum;
	}

	MT_Error("MT_AllocIndex", numBytes);
	return 0;
}

/*
============
MT_FreeIndex
============
*/
void MT_FreeIndex( unsigned int nodeNum, int numBytes )
{
	int size, lowBit;

	size = MT_GetSize(numBytes);
	assert(size >= 0 && size <= MEMORY_NODE_BITS);
	assert(nodeNum > 0 && nodeNum < MEMORY_NODE_COUNT);

	scrMemTreeGlob.totalAlloc--;
	scrMemTreeGlob.totalAllocBuckets -= 1 << size;

	while ( 1 )
	{
		assert(size <= MEMORY_NODE_BITS);
		lowBit = 1 << size;
		assert(nodeNum == (nodeNum & ~(lowBit - 1)));

		if ( size == MEMORY_NODE_BITS || !MT_RemoveMemoryNode(nodeNum ^ lowBit, size) )
		{
			break;
		}

		nodeNum &= ~lowBit;
		size++;
	}

	MT_AddMemoryNode(nodeNum, size);
}

/*
==============
MT_AddMemoryNode
==============
*/
void MT_SafeFreeIndex( unsigned int nodeNum )
{
	int lowBit;
	int size;
	int oldNode;

	size = 0;
	oldNode = nodeNum;

	while ( 1 )
	{
		lowBit = 1 << size;

		if ( MT_RemoveMemoryNode(oldNode, size) )
		{
			MT_AddMemoryNode(oldNode, size);
			return;
		}

		if ( size == MEMORY_NODE_BITS )
		{
			break;
		}

		oldNode &= ~lowBit;
		size++;
	}

	size = 0;
	oldNode = nodeNum;

	while ( 1 )
	{
		lowBit = 1 << size;

		if ( size == MEMORY_NODE_BITS || !MT_RemoveMemoryNode(oldNode ^ lowBit, size) )
		{
			MT_AddMemoryNode(oldNode, size);
			return;
		}

		oldNode &= ~lowBit;
		size++;
	}
}

void* MT_Alloc( int numBytes, int type )
{
	return scrMemTreeGlob.nodes + MT_AllocIndex( numBytes, type );
}

/*
============
MT_Free
============
*/
void MT_Free( void *p, int numBytes )
{
	assert( ( (MemoryNode *)p - scrMemTreeGlob.nodes >= 0 && (MemoryNode *)p - scrMemTreeGlob.nodes < MEMORY_NODE_COUNT ) );
	MT_FreeIndex( (MemoryNode *)p - scrMemTreeGlob.nodes, numBytes );
}

/*
============
MT_InitForceAlloc
============
*/
byte *MT_InitForceAlloc()
{
	scrMemTreeGlob.totalAlloc = 0;
	scrMemTreeGlob.totalAllocBuckets = 0;

	return (byte *)Z_VirtualAlloc( 8192 );
}

/*
============
MT_ForceAllocIndex
============
*/
void MT_ForceAllocIndex( byte *allocBits, unsigned int nodeNum, int numBytes )
{
	int size, newSize;

	size = MT_GetSize(numBytes);
	scrMemTreeGlob.totalAlloc++;

	newSize = 1 << size;
	scrMemTreeGlob.totalAllocBuckets += newSize;

	while ( newSize )
	{
		allocBits[ nodeNum >> 3 ] |= 1 << ( nodeNum & 7 );
		nodeNum++;
		newSize--;
	}
}

/*
============
MT_FinishForceAlloc
============
*/
void MT_FinishForceAlloc( byte *allocBits )
{
	int nodeNum;

	for ( nodeNum = 1; nodeNum < MEMORY_NODE_COUNT; nodeNum++ )
	{
		if ( allocBits[nodeNum >> 3] & ( 1 << ( nodeNum & 7 ) ) )
		{
			continue;
		}

		MT_SafeFreeIndex(nodeNum);
	}

	Z_VirtualFree(allocBits);
}

/*
============
MT_Realloc
============
*/
int MT_Realloc( int oldNumBytes, int newNumbytes )
{
	return MT_GetSize(oldNumBytes) >= MT_GetSize(newNumbytes);
}

