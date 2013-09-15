#include "kinterface.h"

struct kinterface* kinterface;

extern void init_packed_classes();
extern void init_module(void *);

kuint _start(struct kinterface* _kinterface)
{
	kuint8** info;
	kinterface = _kinterface;

	if( ! kinterface->kinterface_check_version(
				KERNEL_INTERFACE_VERSION, 
				KERNEL_IMPLEMENTATION_VERSION)) {
		return 1;
	}

	info = kinterface->kinterface_kernel_info();
	if(info) {
		kuint i;

		for(i = 0; info[i]; i++) {
			kinterface->kprintf("%s\n", info[i]);
		}
	}

	init_packed_classes();

	kinterface->kthread_create(init_module, KNULL, 32, 0);

	return 0;
}

