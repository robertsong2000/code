#include <config.h>

#include "wrappers.h"

int cond_wait(tCondVariable* cond, tMutex* mutex)
{
	if(!cond)
		return -1;

	return kinterface->kcondition_wait(cond, mutex);
}

