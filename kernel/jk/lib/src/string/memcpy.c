#include "kinterface.h"
#include "sys/types.h"

void* memcpy(void* str1, const void* str2, size_t n) 
{
	kinterface->kmemory_virtual_copy(str1, (void*)str2, n);
	return str1;
}
