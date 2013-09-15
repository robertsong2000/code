/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"
#include "elf32.h"

extern void kmemory_linear_setup(kuint mem_lower, kuint mem_upper, kuint mb_max);
extern void kmemory_virtual_setup(void);
extern void linear_block(kuint start, kuint end);
extern kuint kthread_setup(void);
void idle(void* data);
void start_module(void* data);

kuint entry;
struct multiboot_info* mb_info;

kuint setup(struct multiboot_info* mb_info_local) {
	extern void* _start;
	extern void* _end;
	kuint mem_lower;
	kuint mem_upper;
	kuint mb_max = (kuint)mb_info_local;

	mb_info = mb_info_local;

	if(!(mb_info->flags & 1)) {
		raw_print("JK needs to have it's data page-aligned by GRUB\n");
		while(1) {}
	}

	mem_lower = (mb_info->mem_lower * 1024);
	mem_upper = ((mb_info->mem_upper * 1024) + (1024 * 1024));

	if(mb_info->flags & 8) {
		struct multiboot_mod* mods_array = (void*)mb_info->mods_addr;
		kuint i;

		mb_max = max(mb_max, mods_array);

		for(i = 0; i < mb_info->mods_count; i++) {
			struct multiboot_mod* mod = (void*)&mods_array[i];
			mb_max = max(mb_max, mod->mod_end);
		}
	} else {
		raw_print("JK needs to have a module passed in by GRUB\n");
		while(1) {}
	}

	kmemory_linear_setup(mem_lower, mem_upper, mb_max);

	linear_block((kuint)&_start, (kuint)&_end);
	linear_block((kuint)mb_info, ((kuint)mb_info + sizeof(struct multiboot_info)));

	if(mb_info->flags & 8) {
		struct multiboot_mod* mods_array = (void*)mb_info->mods_addr;
		kuint i;

		linear_block((kuint)mods_array, ((kuint)mods_array + (sizeof(struct multiboot_mod) * mb_info->mods_count)));

		for(i = 0; i < mb_info->mods_count; i++) {
			struct multiboot_mod* mod = (void*)&mods_array[i];

			linear_block((kuint)mod->mod_start, (kuint)mod->mod_end);
		}
	}

	kmemory_virtual_setup();

	return kthread_setup();
}

void init(void) {
	kprintf_init();
	kirq_init();
	ktime_init();
	ktimer_init();

	kirq_enable_all();

	if(mb_info->flags & 8) {
		struct multiboot_mod* mods_array = (void*)mb_info->mods_addr;
		kuint i;

		for(i = 0; i < mb_info->mods_count; i++) {
			struct multiboot_mod* mod = (void*)&mods_array[i];

			if(read_elf((kuint8*)mod->mod_start, &entry)) {
				break;
			}
		}
	}

	if(entry) {
		kthread_create(start_module, KNULL, 0, 0);
	} else {
		kprintf("Don't have a module entry point to jump to!\n");
	}

	kthread_create(idle, KNULL, 0, 0);
}

void idle(void* data) {
	while(1) {
		asm volatile("hlt ;");
	}
}

void start_module(void* data) {
	kuint (*start)(struct kinterface* _kinterface) = (void*)entry;

	if(start(&_kinterface)) {
		kprintf("Failed to start module!\n");
	}
}

