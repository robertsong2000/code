#include "kinterface.h"
#include <stdarg.h>

int printf(char* format, ...) 
{
	int count;
	va_list args;

	va_start(args, format);
	count = kinterface->kvprintf(format, args);
	va_end(args);

	return count;
}


