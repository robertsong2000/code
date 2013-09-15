#include <config.h>

#include "wrappers.h"

int cond_signal(tCondVariable* cond)
{
	if(!cond)
		return -1;

	return kinterface->kcondition_signal(cond);
}

