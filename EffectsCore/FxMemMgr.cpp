// Every effects pool draws 32 KB blocks from one fixed arena. A block's free
// items form a list threaded through their first word; freeCount < 0 marks
// the block itself as unused.
struct FxMemBlock
{
	char data[0x7FF0];
	int freeCount;
	void *freeList;
	FxMemBlock *next;
	FxMemBlock *prev;
};

static FxMemBlock fx_memBlocks[64];
static int fx_memHighWater;

void Com_DPrintf( const char *fmt, ... );

void FxMem_Init()
{
	FxMemBlock *block;
	int offset;

	for ( block = fx_memBlocks, offset = 0; offset < sizeof( fx_memBlocks ); block++, offset += sizeof( FxMemBlock ) )
		block->freeCount = -1;
}

// Block management names are unknown; these describe what each does.
FxMemBlock *FxMem_AllocBlock( FxMemBlock *head, int itemCount, int itemSize )
{
	FxMemBlock *block;
	int offset;

	for ( block = fx_memBlocks, offset = 0; offset < sizeof( fx_memBlocks ); block++, offset += sizeof( FxMemBlock ) )
	{
		if ( block->freeCount < 0 )
		{
			if ( fx_memHighWater < offset + (int)sizeof( FxMemBlock ) )
				fx_memHighWater = offset + sizeof( FxMemBlock );
			block->freeCount = itemCount;
			for ( offset = 0; offset < itemCount - 1; offset++ )
				*(char **)( (char *)block + offset * itemSize ) = (char *)block + ( offset + 1 ) * itemSize;
			*(char **)( (char *)block + offset * itemSize ) = 0;
			block->freeList = block;
			if ( head )
				head->prev = block;
			block->next = head;
			block->prev = 0;
			return block;
		}
	}
	Com_DPrintf( "^1Out of effects memory!\n" );
	return 0;
}

void FxMem_UnlinkBlock( FxMemBlock *block )
{
	if ( block->prev )
		block->prev->next = block->next;
	if ( block->next )
		block->next->prev = block->prev;
	block->prev = 0;
	block->next = 0;
}

void FxMem_FreeBlock( FxMemBlock *block )
{
	FxMem_UnlinkBlock( block );
	block->freeCount = -1;
}

// Returns an item to the block that holds it.
FxMemBlock *FxMem_FreeItem( void *item, unsigned int itemSize )
{
	int offset;
	int index;
	FxMemBlock *block;

	offset = (char *)item - (char *)fx_memBlocks;
	block = (FxMemBlock *)( (char *)fx_memBlocks + ( offset & ~0x7FFF ) );
	index = offset & 0x7FFF;
	*(void **)item = block->freeList;
	block->freeList = item;
	block->freeCount++;
	return block;
}


// Unreferenced storage; original declarations unknown (sized from the layout).
static int unusedStorage;
