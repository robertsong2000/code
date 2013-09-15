#include "kinterface.h"

void* malloc(int size) 
{
	return kinterface->kmemory_virtual_page_allocate(
			(((size - 1) / KPAGESIZE) + 1), 1);
}
