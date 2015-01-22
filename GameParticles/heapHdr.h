#ifndef HEAPHDR_H
#define HEAPHDR_H

#include <cstdint>

// forward declares
struct usedHdr;
struct freeHdr;

struct memStats
{
	uint32_t peakNumUsed;		// number of peak used allocations
	uint32_t peakUsedMemory;		// peak size of used memory

	uint32_t currNumUsedBlocks;	// number of current used allocations
	uint32_t currUsedMem;		// current size of the total used memory

	uint32_t	currNumFreeBlocks;	// number of current free blocks
	uint32_t currFreeMem;		// current size of the total free memory

	uint32_t sizeHeap;			// size of Heap total space, including header
	uint32_t sizeHeapHeader;		// size of heap header

	void *heapTopAddr;		// start address available heap
	void *heapBottomAddr;    // bottom of address of heap
};


struct heapHdr
{
	// Make sure that the heapHdr is 16 byte aligned.

	// allocation links
	usedHdr		*usedHead;
	freeHdr		*freeHead;

	bool		heapInitialize;
	uint8_t		bytePad1;
	uint8_t		bytePad2;
	uint8_t		bytePad3;

	freeHdr*	freeTail;
	uint32_t		pad2;
	uint32_t		pad3;

	// recording stats
	memStats	stats;		

	// specialize constructor
	heapHdr(void * ptr);
};

#endif //heapHdr.h