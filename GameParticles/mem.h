#ifndef MEM_H
#define MEM_H

#include "heapHdr.h"

#define HEAP_SIZE ( (256 * 1024 * 1024) )  //256 MB

void* operator new(const size_t size);
void operator delete(void* const p);

class memSystem
{
public:	
	~memSystem();
	heapHdr* getHeap() const;
	void dump() const;

	// implement these functions
	void Free(void* const data) const;
	void* Malloc(const uint32_t size) const;
	void InitializeSystem() const;

	static const memSystem& Instance();

private:
	memSystem();

private:
	heapHdr	*heap;
	char	*rawMem;
};

#endif // mem.h