#include <config.h>

#include "wrappers.h"

int mutex_lock(tMutex* mutex)
{
	return kinterface->kmutex_lock(mutex);
}

