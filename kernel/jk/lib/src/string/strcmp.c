#include "kinterface.h"

int strcmp(const char* str1, const char* str2) 
{
	int index = 0;
	char c;

	while(1) {
		c = str1[index] - str2[index];
		if(c) {
			return c;
		} else if(!str2[index]) {
			return 0;
		}
		index++;
	}
}

