#include <config.h>

#include "wrappers.h"

int cond_timedwait(tCondVariable* cond, tMutex* mutex, tTimeSpec* spec)
{
	kinterface->kprintf("!!! cond_timedwait not supported yet !!!\n");

	return -1;
}

