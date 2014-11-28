#ifndef FREEHDR_H
#define FREEHDR_H

#include "memTypes.h"

#define FREE_HEADER_TYPE 0xCC

struct freeHdr
{
	freeHdr *freeNext;		// next free block
	freeHdr *freePrev;		// prev free block
	memU32  blockSize;		// size of block
	memU8	blockType;		// block type is used -> 0xAA  if block type is free -> 0xCC
	memBool	aboveBlockFree;	// NOTE: This must have the same offset into the freeHdr struct as the corresponding field in usedHdr
	memBool isLastNode;	// indicates whether there is another header below this one in memory
	memU8	pad2;			// future use
};

#endif //FREEHDR_H