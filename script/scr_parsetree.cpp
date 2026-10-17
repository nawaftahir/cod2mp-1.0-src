#include "../qcommon/qcommon.h"
#include "script_public.h"

/*
==============
node1_
==============
*/
sval_u node1_( sval_u val1 )
{
	return val1;
}

/*
==============
node_pos
==============
*/
sval_u node_pos( unsigned int sourcePos )
{
	sval_u result;

	result.sourcePosValue = sourcePos;
	return result;
}

/*
==============
node0
==============
*/
sval_u node0( int type )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 1 * sizeof( sval_u ) );
	result.node[0].type = type;
	return result;
}

/*
==============
node1
==============
*/
sval_u node1( int type, sval_u val1 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 2 * sizeof( sval_u ) );
	result.node[0].type = type;
	result.node[1] = val1;
	return result;
}

/*
==============
node2
==============
*/
sval_u node2( int type, sval_u val1, sval_u val2 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 3 * sizeof( sval_u ) );
	result.node[0].type = type;
	result.node[1] = val1;
	result.node[2] = val2;
	return result;
}

/*
==============
node2_
==============
*/
sval_u node2_( sval_u val1, sval_u val2 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 2 * sizeof( sval_u ) );
	result.node[0] = val1;
	result.node[1] = val2;
	return result;
}

/*
==============
node3
==============
*/
sval_u node3( int type, sval_u val1, sval_u val2, sval_u val3 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 4 * sizeof( sval_u ) );
	result.node[0].type = type;
	result.node[1] = val1;
	result.node[2] = val2;
	result.node[3] = val3;
	return result;
}

/*
==============
node3_
==============
*/
sval_u node3_( sval_u val1, sval_u val2, sval_u val3 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 3 * sizeof( sval_u ) );
	result.node[0] = val1;
	result.node[1] = val2;
	result.node[2] = val3;
	return result;
}

/*
==============
node4
==============
*/
sval_u node4( int type, sval_u val1, sval_u val2, sval_u val3, sval_u val4 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 5 * sizeof( sval_u ) );
	result.node[0].type = type;
	result.node[1] = val1;
	result.node[2] = val2;
	result.node[3] = val3;
	result.node[4] = val4;
	return result;
}

/*
==============
node4_
==============
*/
sval_u node4_( sval_u val1, sval_u val2, sval_u val3, sval_u val4 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 4 * sizeof( sval_u ) );
	result.node[0] = val1;
	result.node[1] = val2;
	result.node[2] = val3;
	result.node[3] = val4;
	return result;
}

/*
==============
node5
==============
*/
sval_u node5( int type, sval_u val1, sval_u val2, sval_u val3, sval_u val4, sval_u val5 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 6 * sizeof( sval_u ) );
	result.node[0].type = type;
	result.node[1] = val1;
	result.node[2] = val2;
	result.node[3] = val3;
	result.node[4] = val4;
	result.node[5] = val5;
	return result;
}

/*
==============
node6
==============
*/
sval_u node6( int type, sval_u val1, sval_u val2, sval_u val3, sval_u val4, sval_u val5, sval_u val6 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 7 * sizeof( sval_u ) );
	result.node[0].type = type;
	result.node[1] = val1;
	result.node[2] = val2;
	result.node[3] = val3;
	result.node[4] = val4;
	result.node[5] = val5;
	result.node[6] = val6;
	return result;
}

/*
==============
node7
==============
*/
sval_u node7( int type, sval_u val1, sval_u val2, sval_u val3, sval_u val4, sval_u val5, sval_u val6, sval_u val7 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 8 * sizeof( sval_u ) );
	result.node[0].type = type;
	result.node[1] = val1;
	result.node[2] = val2;
	result.node[3] = val3;
	result.node[4] = val4;
	result.node[5] = val5;
	result.node[6] = val6;
	result.node[7] = val7;
	return result;
}

/*
==============
node8
==============
*/
sval_u node8( int type, sval_u val1, sval_u val2, sval_u val3, sval_u val4, sval_u val5, sval_u val6, sval_u val7, sval_u val8 )
{
	sval_u result;

	result.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 9 * sizeof( sval_u ) );
	result.node[0].type = type;
	result.node[1] = val1;
	result.node[2] = val2;
	result.node[3] = val3;
	result.node[4] = val4;
	result.node[5] = val5;
	result.node[6] = val6;
	result.node[7] = val7;
	result.node[8] = val8;
	return result;
}

/*
==============
linked_list_end
==============
*/
sval_u linked_list_end( sval_u val1 )
{
	sval_u list;
	sval_u *node;

	node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 2 * sizeof( sval_u ) );
	node[0] = val1;
	node[1].node = NULL;
	list.node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 2 * sizeof( sval_u ) );
	list.node[0].node = node;
	list.node[1].node = node;
	return list;
}

/*
==============
prepend_node
==============
*/
sval_u prepend_node( sval_u val1, sval_u list )
{
	sval_u *node;

	node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 2 * sizeof( sval_u ) );
	node[0] = val1;
	node[1].node = list.node[0].node;
	list.node[0].node = node;
	return list;
}

/*
==============
append_node
==============
*/
sval_u append_node( sval_u list, sval_u val1 )
{
	sval_u *node;

	node = (sval_u *)Hunk_AllocateTempMemoryHighInternal( 2 * sizeof( sval_u ) );
	node[0] = val1;
	node[1].node = NULL;
	list.node[1].node[1].node = node;
	list.node[1].node = node;
	return list;
}

/*
==============
concat_list
==============
*/
sval_u concat_list( sval_u list1, sval_u list2 )
{
	list1.node[1].node[1].node = list2.node[0].node;
	list1.node[1].node = list2.node[1].node;
	return list1;
}

