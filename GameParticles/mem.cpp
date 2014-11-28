#include <new>
#include <assert.h>
#include <cmath>
#include "mem.h"
#include "heapHdr.h"
#include "freeHdr.h"
#include "usedHdr.h"
#include "crtdbg.h"

#define ALIGNMENT_PAD (16)

freeHdr* divide_with_new_free_header(freeHdr* dest, memU32 blockSize, freeHdr* next, memBool isLastNode);
freeHdr* find_next_free_node(usedHdr* current);
memU32 max(const memU32 a, const memU32 b);
freeHdr* merge_blocks(freeHdr* destination, freeHdr* headerToRemove);

memSystem::~memSystem()
{
	delete this->rawMem;
}


heapHdr *memSystem::getHeap()
{
	return heap;
}

memSystem::memSystem()
{
	// create the memory block for the heap
	this->rawMem = new char[HEAP_SIZE+ALIGNMENT_PAD];

	// Correct pointer to make sure it's 16 byte aligned
	char *p_corrected = (char *) ( ((memU32)rawMem + (16-1) ) & ~(16-1) );   

	// instantiate the heap header on the raw memory
	heap = new(p_corrected) heapHdr(p_corrected);

}

memVoid memSystem::InitializeSystem( )
{
	freeHdr* firstHeader = (freeHdr*) (this->heap + 1);
	firstHeader->freeNext = 0;
	firstHeader->freePrev = 0;
	firstHeader->blockSize = HEAP_SIZE - sizeof(heapHdr) - sizeof(freeHdr);
	firstHeader->blockType = FREE_HEADER_TYPE;
	firstHeader->aboveBlockFree = false;
	firstHeader->isLastNode = true;
	freeHdr** endPtr = (freeHdr**) ((unsigned char*) (firstHeader + 1) + firstHeader->blockSize - sizeof(freeHdr*));
	*endPtr = firstHeader;

	heap->freeHead = firstHeader;
	heap->freeTail = firstHeader;
	heap->usedHead = 0;

	heap->stats.peakNumUsed = 0;
	heap->stats.peakUsedMemory = 0;

	heap->stats.currNumUsedBlocks = 0;
	heap->stats.currUsedMem = 0;

	heap->stats.currNumFreeBlocks = 1;
	heap->stats.currFreeMem = firstHeader->blockSize;

	heap->stats.sizeHeap = HEAP_SIZE;
	heap->stats.sizeHeapHeader = sizeof(heapHdr);
}

memVoid memSystem::Free( memVoid * const data )
{
	usedHdr* header = ((usedHdr*) data) - 1;

	this->heap->stats.currNumUsedBlocks--;
	this->heap->stats.currUsedMem -= header->blockSize;

	this->heap->stats.currNumFreeBlocks++;
	this->heap->stats.currFreeMem += header->blockSize;

	usedHdr* preA = header->usedNext;
	usedHdr* postA = header->usedPrev;

	freeHdr* preF;
	freeHdr* postF;
	
	if (this->heap->freeHead == 0) {
		preF = 0;
		postF = 0;
	}else if ((memUInt) this->heap->freeHead > (memUInt) header) {
		preF = 0;
		postF = this->heap->freeHead;
	}else{
		if ((memUInt) this->heap->freeTail < (memUInt) header) {
			preF = this->heap->freeTail;
			postF = 0;
		}else{
			postF = find_next_free_node(header);
			preF = postF->freePrev;
		}
	}

	// Change used header to free header
	freeHdr* freeHeader = (freeHdr*) header;
	freeHeader->freeNext = postF;
	freeHeader->freePrev = preF;
	freeHeader->blockType = FREE_HEADER_TYPE;

	// Add free header to free nodes list
	if (preF != 0)
		preF->freeNext = freeHeader;
	else
		this->heap->freeHead = freeHeader;

	if (postF != 0)
		postF->freePrev = freeHeader;
	else
		this->heap->freeTail = freeHeader;

	// Remove free header from used nodes list
	if (preA != 0) 
		preA->usedPrev = postA;

	if (postA != 0)
		postA->usedNext = preA;
	else
		this->heap->usedHead = preA;

	freeHdr* neighborBelow;

	if (postA != 0 || postF != 0) {
		// Both the freeHdr and usedHdr have the aboveBlockFree value in the same place in the struct, so the type
		// here doesn't really matter
		neighborBelow = (freeHdr*) ((char*) header + sizeof(freeHdr) + header->blockSize);
		neighborBelow->aboveBlockFree = true;
	}
	
	if (postF != 0 && postF->aboveBlockFree) {
		freeHeader = merge_blocks(freeHeader, postF);
		freeHeader->freePrev = preF;
		if (freeHeader->freeNext == 0)
			this->heap->freeTail = freeHeader;

		this->heap->stats.currFreeMem += sizeof(freeHdr);
		this->heap->stats.currNumFreeBlocks--;
	}

	freeHdr** pointerAtEnd = (freeHdr**) ((char*) freeHeader + sizeof(freeHdr) + freeHeader->blockSize - sizeof(freeHdr*));
	*pointerAtEnd = freeHeader;

	if (freeHeader->aboveBlockFree) {
		preF = merge_blocks(preF, freeHeader);
		if (preF->freeNext == 0)
			this->heap->freeTail = preF;
		this->heap->stats.currFreeMem += sizeof(freeHdr);
		this->heap->stats.currNumFreeBlocks--;
	}
}

memVoid* memSystem::Malloc( const memU32 size )
{
	memVoid* ptr;

	if (size == 0) {
		ptr = 0;
	
	}else{
	
		// Allocate memory so that the addresses are 4-byte aligned
		memU32 aligned = 4 * (memU32) ceilf(size / 4.0f);
		memU32 memSize = max(sizeof(freeHdr*), aligned);
	
		// Find first free node that is large enough for the allocation
		freeHdr* freeHeader = this->heap->freeHead;
		while (freeHeader != 0 && freeHeader->blockSize < memSize)
			freeHeader = freeHeader->freeNext;

		// Do we have a large enough chunk of contiguous memory?
		if (freeHeader == 0) {
			ptr = 0;
		}else{

			// Find the previous and next free nodes in the list
			freeHdr* preF = freeHeader->freePrev;
			freeHdr* postF;

			if (freeHeader->blockSize < memSize + sizeof(freeHdr) + sizeof(freeHdr *)) {
				postF = freeHeader->freeNext;
				memSize = freeHeader->blockSize;
				this->heap->stats.currNumFreeBlocks--;
			}else{
				freeHdr* address = (freeHdr*) ((unsigned char *) (freeHeader + 1) + memSize);
				memU32 blockSize = freeHeader->blockSize - memSize - sizeof(freeHdr);
				postF = divide_with_new_free_header(address, blockSize, freeHeader->freeNext, freeHeader->isLastNode);
				freeHeader->isLastNode = false;
				if (this->heap->freeHead == freeHeader)
					this->heap->freeHead = postF;
				if (this->heap->freeTail == freeHeader)
					this->heap->freeTail = postF;

				this->heap->stats.currFreeMem -= sizeof(freeHdr);
			}

			// Find the previous and next used nodes in the list
			usedHdr* preA = this->heap->usedHead;
			
			// Change the header to allocated
			usedHdr* usedHeader = (usedHdr*) freeHeader;
			usedHeader->usedPrev = 0;
			usedHeader->usedNext = preA;
			usedHeader->blockSize = memSize;
			usedHeader->blockType = USED_HEADER_TYPE;
			
			// Clean up the used nodes linked list
			if (preA != 0)
				preA->usedPrev = usedHeader;

			this->heap->usedHead = usedHeader;
			
			// Clean up the free nodes linked list
			if (preF != 0)
				preF->freeNext = postF;
			else
				this->heap->freeHead = postF;

			if (postF != 0)
				postF->freePrev = preF;

			// Update our neighbor below (he needs to know that we are now a used node)
			if (!usedHeader->isLastNode) {
				usedHdr* neighbor = (usedHdr*) ((unsigned char*) (usedHeader + 1) + usedHeader->blockSize);
				neighbor->aboveBlockFree = false;
			}

			ptr = usedHeader + 1;
			this->heap->stats.currNumUsedBlocks++;
			this->heap->stats.currUsedMem += memSize;

			this->heap->stats.currFreeMem -= memSize;

			this->heap->stats.peakNumUsed = max(this->heap->stats.peakNumUsed, this->heap->stats.currNumUsedBlocks);
			this->heap->stats.peakUsedMemory = max(this->heap->stats.peakUsedMemory, this->heap->stats.currUsedMem);
		}
	}

	return ptr;
}

#define EMPTY_FIELD "        "

// ---------------- Internal functions --------------------
inline freeHdr* divide_with_new_free_header(freeHdr* dest, memU32 blockSize, freeHdr* next, memBool isLastNode) {
	dest->freeNext = next;
	dest->blockSize = blockSize;
	dest->blockType = FREE_HEADER_TYPE;
	dest->aboveBlockFree = true;
	dest->isLastNode = isLastNode;

	if (next != 0)
		next->freePrev = dest;

	freeHdr** endingPtr = (freeHdr**) ((unsigned char*) (dest + 1) + blockSize - sizeof(freeHdr*));
	*endingPtr = dest;

	return dest;
}

freeHdr* find_next_free_node(usedHdr* current) {
	if (current == 0)
		return 0;

	if (current->blockType == FREE_HEADER_TYPE)
		return (freeHdr*) current;

	else
		return find_next_free_node((usedHdr*) ((memUInt) current + sizeof(usedHdr) + current->blockSize));
}

inline memU32 max(const memU32 a, const memU32 b) {
	return (a > b) ? a : b;
}

inline freeHdr* merge_blocks(freeHdr* destination, freeHdr* headerToRemove) {
	destination->blockSize += sizeof(freeHdr) + headerToRemove->blockSize;
	destination->freeNext = headerToRemove->freeNext;
	if (destination->freeNext != 0)
		destination->freeNext->freePrev = destination;

	destination->isLastNode = headerToRemove->isLastNode;

	freeHdr** endPtr = (freeHdr**) ((memUInt) (destination + 1) + destination->blockSize - sizeof(freeHdr*));
	*endPtr = destination;
	
	return destination;
}