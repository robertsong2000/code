#include "kinterface.h"
#include "sys/types.h"

void* memset(void* str, int c, size_t n) 
{
	kinterface->kmemory_virtual_fill(str, n, c);
	return str;
}

