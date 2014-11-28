#ifndef MEM_H
#define MEM_H

#include "heapHdr.h"

#define HEAP_SIZE ( (256 * 1024 * 1024) )  //256 MB


class memSystem
{
public:
	memSystem();	
	~memSystem();
	heapHdr *getHeap();
	memVoid dump();

	// implement these functions
	memVoid Free( memVoid * const data );
	memVoid *Malloc(const memU32 size );
	memVoid InitializeSystem( );

private:
	heapHdr	*heap;
	char	*rawMem;

};

#endif // mem.h