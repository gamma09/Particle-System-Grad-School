#ifndef USEDHDR_H
#define USEDHDR_H

#include "memTypes.h"

#define USED_HEADER_TYPE 0xAA

struct usedHdr
{
	usedHdr *usedNext;		// next used block
	usedHdr *usedPrev;		// prev used block
	memU32  blockSize;		// size of block
	memU8	blockType;		// block type is used -> 0xAA  if block type is free -> 0xCC
	
	// if(block is free) -> true or if(block is used) -> false
	// NOTE: This must have the same offset into the freeHdr struct as the corresponding field in usedHdr
	memBool	aboveBlockFree;

	memBool isLastNode;	// indicates whether there is another header below this one in memory
	memU8	pad1;			// future use
};

#endif //USEDHDR_H