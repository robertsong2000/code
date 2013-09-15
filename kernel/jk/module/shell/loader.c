#include "kinterface.h"

kuint8* packed_classes = KNULL;

void init_packed_classes() 
{
	kuint mod_index = 0;
	kuint8* buffer;
	kuint buffer_length;

	while((mod_index = kinterface->kmodule_get(
					mod_index, 
					&buffer, 
					&buffer_length))) {
		{
			packed_classes = buffer;
			kinterface->kprintf(
				"Loaded: module_index=%d buffer_size=%d\n",
				mod_index, buffer_length );
			return;
		}
	}

	kinterface->kprintf("Failed!\n");
}

