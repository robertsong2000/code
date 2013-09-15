/*
 * Copyright (C) 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

kfunction void krwlock_read_lock(volatile kuint* lock) {
	kspinlock_lock(lock);

	while(1) {
		(*((kint16*)lock))++;
		if((*((kint16*)lock)) > 0) {
			break;
		}
		(*((kint16*)lock))--;

		kspinlock_unlock(lock);
		kthread_yield();
		kspinlock_lock(lock);
	}

	kspinlock_unlock(lock);
}

kfunction void krwlock_read_unlock(volatile kuint* lock) {
	kspinlock_lock(lock);

	(*((kint16*)lock))--;

	kspinlock_unlock(lock);
}

kfunction void krwlock_write_lock(volatile kuint* lock) {
	kuint temp = 0;

	kspinlock_lock(lock);

	if(katomic_bit_test_and_set(lock, 15)) {
		while((temp = (*lock & 0x7FFF7FFF))) {
			kspinlock_unlock(lock);
			kthread_yield();
			kspinlock_lock(lock);
		}
	} else {
		while((temp = (*lock & 0x00007FFF))) {
			kspinlock_unlock(lock);
			kthread_yield();
			kspinlock_lock(lock);
		}
	}

	*lock += 0x00010000;

	kspinlock_unlock(lock);
}

kfunction void krwlock_write_unlock(volatile kuint* lock) {
	kuint temp = 0;

	kspinlock_lock(lock);

	*lock -= 0x00010000;

	if(!(temp = (*lock & 0x7FFF0000))) {
		katomic_bit_reset(lock, 15);
	}

	kspinlock_unlock(lock);
}

