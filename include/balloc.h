/*
 * balloc.h - Based roughly on Wohali's old block allocator.
 *
 * Mangled by Aaron Sethman <androsyn@ratbox.org>
 * Below is the original header found on this file
 *
 * File:   blalloc.h
 * Owner:   Wohali (Joan Touzet)
 *
 *
 * $Id: balloc.h,v 1.7 2001/10/03 19:12:40 androsyn Exp $
 */
#ifndef INCLUDED_blalloc_h
#define INCLUDED_blalloc_h
#ifndef NOBALLOC
#ifndef INCLUDED_sys_types_h
#include <sys/types.h>       /* size_t */
#define INCLUDED_sys_types_h
#endif

#ifndef INCLUDED_stddef_h
#include <stddef.h>
#define INCLUDED_stddef_h
#endif
#include <sys/mman.h>
#include "tools.h"
#include "memory.h"
#include "ircd_defs.h"


/* 
 * Block contains status information for an allocated block in our
 * heap.
 */


struct Block {
	int		freeElems;		/* Number of available elems */
	size_t		alloc_size;
	struct Block*	next;			/* Next in our chain of blocks */
	void*		elems;			/* Points to allocated memory */
	dlink_list	free_list;
	dlink_list	used_list;					
};

typedef struct Block Block;

struct MemBlock {
	dlink_node self;		
	Block *block;				/* Which block we belong to */
	void *data;				/* Maybe pointless? :P */
};
typedef struct MemBlock MemBlock;

/* 
 * BlockHeap contains the information for the root node of the
 * memory heap.
 */
struct BlockHeap {
   size_t  elemSize;                    /* Size of each element to be stored */
   int     elemsPerBlock;               /* Number of elements per block */
   int     blocksAllocated;             /* Number of blocks allocated */
   int     freeElems;                   /* Number of free elements */
   Block*  base;                        /* Pointer to first block */
};

typedef struct BlockHeap BlockHeap;


extern BlockHeap* BlockHeapCreate(size_t elemsize, int elemsperblock);
extern int        BlockHeapDestroy(BlockHeap *bh);

#if 0 /* These are in memory.h... */
extern int        BlockHeapFree(BlockHeap *bh, void *ptr);
extern void *	  BlockHeapAlloc(BlockHeap *bh);
#endif

extern int        BlockHeapGarbageCollect(BlockHeap *);
extern void	  initBlockHeap(void);
#else /* NOBALLOC */
typedef struct BlockHeap BlockHeap;
#define initBlockHeap()
#define BlockHeapGarbageCollect(x)
/* This is really kludgy, passing ints as pointers is always bad. */
#define BlockHeapCreate(es, epb) ((BlockHeap*)(es))
#define BlockHeapDestroy(x)
#endif /* NOBALLOC */
#endif /* INCLUDED_blalloc_h */
