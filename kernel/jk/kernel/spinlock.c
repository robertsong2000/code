/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

kfunction void kspinlock_lock(volatile kuint* lock) {
	asm volatile(
		"0: ;"
		LOCK "btsl $31, (%0) ;"
		"jc 0b ;"
		: "=d" (lock)
		: "0" (lock)
	);
}

kfunction void kspinlock_unlock(volatile kuint* lock) {
	asm volatile(
		LOCK "btrl $31, (%0) ;"
		: "=d" (lock)
		: "0" (lock)
	);
}

kfunction void kspinlock_lock_irqsave(volatile kuint* lock, kuint* irqsave) {
	asm volatile(
		"pushfl ;"
		"popl (%1) ;"
		"cli ;"
		"0: ;"
		LOCK "btsl $31, (%0) ;"
		"jc 0b ;"
		: "=d" (lock), "=r" (irqsave)
		: "0" (lock), "1" (irqsave)
	);
}

kfunction void kspinlock_unlock_irqrestore(volatile kuint* lock, kuint* irqsave) {
	asm volatile(
		LOCK "btrl $31, (%0) ;"
		"pushl (%1) ;"
		"popfl ;"
		: "=d" (lock), "=r" (irqsave)
		: "0" (lock), "1" (irqsave)
	);
}

