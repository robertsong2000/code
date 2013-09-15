#include <config.h>

#include "wrappers.h"

int sys_mutex_destroy(tMutex* mutex)
{
	if(!mutex)
		return -1;

	kinterface->kmutex_destroy(mutex);

	return 0;
}

