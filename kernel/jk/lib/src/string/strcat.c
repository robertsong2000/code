#include "kinterface.h"
#include "sys/types.h"

extern char *strcpy(char*, const char*);
extern size_t strlen(const char *);

char* strcat(char* str1, const char* str2) 
{
	strcpy(strlen(str1) + str1, str2);
	return str1;
}

