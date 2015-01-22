#ifndef USEDHDR_H
#define USEDHDR_H

#define USED_HEADER_TYPE 0xAA

struct usedHdr
{
	usedHdr *usedNext;		// next used block
	usedHdr *usedPrev;		// prev used block
	uint32_t  blockSize;		// size of block
	uint8_t	blockType;		// block type is used -> 0xAA  if block type is free -> 0xCC
	
	// if(block is free) -> true or if(block is used) -> false
	// NOTE: This must have the same offset into the freeHdr struct as the corresponding field in usedHdr
	bool	aboveBlockFree;

	bool isLastNode;	// indicates whether there is another header below this one in memory
	uint8_t	pad1;			// future use
};

#endif //USEDHDR_H