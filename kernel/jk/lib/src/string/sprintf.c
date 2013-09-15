#include "kinterface.h"
#include "stdarg.h"

int sprintf(char* buffer, char* format, ...) 
{
	int count;
	va_list args;

	va_start(args, format);
	count = kinterface->kvsprintf(buffer, format, args);
	va_end(args);
	return count;
}
