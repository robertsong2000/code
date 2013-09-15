#include "kinterface.h"

void free(void* ptr) 
{
	kinterface->kmemory_virtual_page_unallocate(ptr);
}

