#include "kinterface.h"
#include "sys/types.h"

extern char* strchr(const char*, int);
extern int strcmp(const char*, const char*);

char* strstr(const char* str1, const char* str2) 
{
	char* p = (char*)str1;

	while(1) {
		p = strchr(p, str2[0]);
		if(!p) {
			return NULL;
		}
		if(!strcmp(p, str2)) {
			return p;
		}
		p++;
	}

	return NULL;
}

