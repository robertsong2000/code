/*
 * Copyright (C) 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

extern struct multiboot_info* mb_info;

kfunction kuint kmodule_get(kuint value, kuint8** data, kuint* length) {
	kuint index = value;
	struct multiboot_mod* mods_array = (void*)mb_info->mods_addr;

	if(index < mb_info->mods_count) {
		struct multiboot_mod* mod = (void*)&mods_array[index];

		*data = (kuint8*)mod->mod_start;
		*length = ((kuint)mod->mod_end - (kuint)mod->mod_start);

		return ++index;
	}

	return 0;
}

kfunction void kmodule_free() {
}

