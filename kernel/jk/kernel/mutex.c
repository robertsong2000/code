/*
 * Copyright (C) 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

kfunction kmutex* kmutex_create(void) {
	return (kmutex*)kmemory_virtual_page_allocate(1, 0);
}

kfunction void kmutex_destroy(kmutex* mutex) {
	kmemory_virtual_page_unallocate(mutex);
}

kfunction kuint kmutex_lock(kmutex* mutex) {
	kuint current = kthread_current();

	while(1) {
		if(
			katomic_compare_and_set_if_equal(&mutex->thread, 0, current) ||
			katomic_compare(&mutex->thread, current)
		) {
			mutex->count++;
			break;
		} else {
			kthread_yield();
		}
	}

	return 0;
}

kfunction kuint kmutex_unlock(kmutex* mutex) {
	if(!kmutex_test(mutex)) {
		return -1;
	}

	if(!(--mutex->count)) {
		katomic_set(&mutex->thread, 0);
	}

	return 0;
}

kfunction kuint kmutex_test(kmutex* mutex) {
	return katomic_compare(&mutex->thread, kthread_current());
}

