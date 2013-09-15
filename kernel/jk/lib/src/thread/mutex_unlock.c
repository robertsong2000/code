#include <config.h>

#include "wrappers.h"

int sys_mutex_unlock(tMutex* mutex)
{
	if(!mutex)
		return -1;

	return kinterface->kmutex_unlock(mutex);
}

