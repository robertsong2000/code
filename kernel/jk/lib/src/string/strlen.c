#include "kinterface.h"
#include "sys/types.h"

size_t strlen(const char* str) 
{
	size_t length = 0;

	while(str[length]) {
		length++;
	}

	return length;
}


