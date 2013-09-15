#include "kinterface.h"
#include "sys/types.h"

char* strncpy(char* str1, const char* str2, size_t n) 
{
	int index = 0;

	while(index < n) {
		str1[index] = str2[index];
		if(!str2[index]) {
			break;
		}
		index++;
	}

	return str1;
}

