#include "kinterface.h"

char* strcpy(char* str1, const char* str2) 
{
	int index = 0;

	while(1) {
		str1[index] = str2[index];
		if(!str2[index]) {
			break;
		}
		index++;
	}

	return str1;
}

