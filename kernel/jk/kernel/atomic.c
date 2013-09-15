/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

kfunction kuint katomic_get(volatile kuint* atomic) {
	return (*atomic);
}

kfunction void katomic_set(volatile kuint* atomic, kuint value) {
	(*atomic) = value;
}

kfunction void katomic_inc(volatile kuint* atomic) {
	asm volatile(
		LOCK "incl (%0) ;"
		: "=d" (atomic)
		: "0" (atomic)
	);
}

kfunction void katomic_dec(volatile kuint* atomic) {
	asm volatile(
		LOCK "decl (%0) ;"
		: "=d" (atomic)
		: "0" (atomic)
	);
}

kfunction void katomic_add(volatile kuint* atomic, kuint value) {
	asm volatile(
		LOCK "addl %1, (%0) ;"
		: "=d" (atomic)
		: "c" (value), "0" (atomic)
	);
}

kfunction void katomic_sub(volatile kuint* atomic, kuint value) {
	asm volatile(
		LOCK "subl %1, (%0) ;"
		: "=d" (atomic)
		: "c" (value), "0" (atomic)
	);
}

kfunction kuint katomic_dec_and_test(volatile kuint* atomic) {
	kuint test = 0;

	asm volatile(
		LOCK "decl (%0) ;"
		"sete %b1 ;"
		: "=d" (atomic), "=a" (test)
		: "0" (atomic), "1" (test)
	);

	return test;
}

kfunction kuint katomic_inc_and_test(volatile kuint* atomic) {
	kuint test = 0;

	asm volatile(
		LOCK "incl (%0) ;"
		"sete %b1 ;"
		: "=d" (atomic), "=a" (test)
		: "0" (atomic), "1" (test)
	);

	return test;
}

kfunction kuint katomic_compare(volatile kuint* atomic, kuint compare) {
	kuint test = 0;

	asm volatile(
		LOCK "subl %1, (%0) ;"
		"sete %b2 ;"
		: "=d" (atomic), "=c" (compare), "=a" (test)
		: "0" (atomic), "1" (compare), "2" (test)
	);

	return test;
}

kfunction kuint katomic_compare_and_set_if_equal(volatile kuint* atomic, kuint compare, kuint value) {
#if(CPU == 386)
	kuint test = 0;
	kuint flags;

	kprocessor_save_flags(&flags);
	kirq_disable_all();

	if((*atomic) == compare) {
		(*atomic) = value;
		test = 1;
	}

	kprocessor_restore_flags(&flags);

	return test;
#else
	kuint test;

	asm volatile(
		LOCK "cmpxchgl %3, (%0) ;"
		"xor %1, %1 ;"
		"sete %b1 ;"
		: "=d" (atomic), "=a" (test)
		: "a" (compare), "c" (value), "0" (atomic)
	);

	return test;
#endif
}

kfunction void katomic_mask_set(volatile kuint* atomic, kuint mask) {
	asm volatile(
		LOCK "orl (%0), %1 ;"
		: "=d" (atomic)
		: "r" (mask), "0" (atomic)
	);
}

kfunction void katomic_mask_clear(volatile kuint* atomic, kuint mask) {
	asm volatile(
		LOCK "andl (%0), %1 ;"
		: "=d" (atomic)
		: "r" (~mask), "0" (atomic)
	);
}

kfunction kuint katomic_bit_get(volatile kuint* atomic, kuint bit) {
	kuint test = 0;

	asm volatile(
		LOCK "btl %2, (%0) ;"
		"setc %b1 ;"
		: "=d" (atomic), "=a" (test)
		: "ic" (bit), "0" (atomic), "1" (test)
	);

	return test;
}

kfunction void katomic_bit_set(volatile kuint* atomic, kuint bit) {
	asm volatile(
		LOCK "btsl %1, (%0) ;"
		: "=d" (atomic)
		: "ic" (bit), "0" (atomic)
	);
}

kfunction void katomic_bit_reset(volatile kuint* atomic, kuint bit) {
	asm volatile(
		LOCK "btrl %1, (%0) ;"
		: "=d" (atomic)
		: "ic" (bit), "0" (atomic)
	);
}

kfunction kuint katomic_bit_test_and_set(volatile kuint* atomic, kuint bit) {
	kuint test = 0;

	asm volatile(
		LOCK "btsl %2, (%0) ;"
		"setc %b1 ;"
		: "=d" (atomic), "=a" (test)
		: "ic" (bit), "0" (atomic), "1" (test)
	);

	return test;
}

kfunction kuint katomic_bit_test_and_reset(volatile kuint* atomic, kuint bit) {
	kuint test = 0;

	asm volatile(
		LOCK "btrl %2, (%0) ;"
		"setc %b1 ;"
		: "=d" (atomic), "=a" (test)
		: "ic" (bit), "0" (atomic), "1" (test)
	);

	return test;
}

