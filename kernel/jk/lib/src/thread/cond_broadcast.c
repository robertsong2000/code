#include <config.h>

#include "wrappers.h"

int cond_broadcast(tCondVariable* cond)
{
	if(!cond)
		return -1;

	return kinterface->kcondition_broadcast(cond);
}

