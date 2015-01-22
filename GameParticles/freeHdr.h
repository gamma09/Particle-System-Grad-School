#ifndef FREEHDR_H
#define FREEHDR_H

#define FREE_HEADER_TYPE 0xCC

struct freeHdr
{
	freeHdr *freeNext;		// next free block
	freeHdr *freePrev;		// prev free block
	uint32_t blockSize;		// size of block
	uint8_t	blockType;		// block type is used -> 0xAA  if block type is free -> 0xCC
	bool	aboveBlockFree;	// NOTE: This must have the same offset into the freeHdr struct as the corresponding field in usedHdr
	bool	isLastNode;	// indicates whether there is another header below this one in memory
	uint8_t	pad2;			// future use
};

#endif //FREEHDR_H