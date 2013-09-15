#include "kinterface.h"
#include "sys/types.h"

int strncmp(const char* str1, const char* str2, size_t n) 
{
	int index = 0;
	char c;

	while(index < n) {
		c = str1[index] - str2[index];
		if(c) {
			return c;
		} else if(!str2[index]) {
			return 0;
		}
		index++;
	}

	return 0;
}

