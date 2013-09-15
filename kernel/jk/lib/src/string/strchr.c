#include "kinterface.h"
#include "sys/types.h"

char* strchr(const char* str, int c) 
{
	int index = 0;

	while(str[index]) {
		if(str[index] == c) {
			return (char*)((unsigned long)str + index);
		}
		index++;
	}

	return NULL;
}

