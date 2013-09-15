/*
 * Copyright (C) 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

kfunction void kprocessor_flags_save(kuint* flags) {
	asm volatile(
		"pushfl ;"
		"popl %0 ;"
		: "=g" (*flags)
		: "0" (*flags)
	);
}

kfunction void kprocessor_flags_restore(kuint* flags) {
	asm volatile(
		"pushl %0 ;"
		"popfl ;"
		: "=g" (*flags)
		: "0" (*flags)
	);
}

kfunction void kprocessor_byteswap_kuint(kuint* value) {
#if (CPU == 386)
	asm volatile(
		"xchgb %b0, %h0 ;"
		"rorl $16, %0 ;"
		"xchgb %b0, %h0 ;"
		: "=q" (*value)
		: "0" (*value)
	);
#else
	asm volatile(
		"bswapl %0 ;"
		: "=q" (*value)
		: "0" (*value)
	);
#endif
}

kfunction void kprocessor_byteswap_kuint8(kuint8* value) {
}

kfunction void kprocessor_byteswap_kuint16(kuint16* value) {
	asm volatile(
		"xchgb %b0, %h0 ;"
		: "=q" (*value)
		: "0" (*value)
	);
}

kfunction void kprocessor_byteswap_kuint32(kuint32* value) {
#if (CPU == 386)
	asm volatile(
		"xchgb %b0, %h0 ;"
		"rorl $16, %0 ;"
		"xchgb %b0, %h0 ;"
		: "=q" (*value)
		: "0" (*value)
	);
#else
	asm volatile(
		"bswapl %0 ;"
		: "=q" (*value)
		: "0" (*value)
	);
#endif
}

kfunction void kprocessor_byteswap_kuint64(kuint64* value) {
#if (CPU == 386)
	asm volatile(
		"xchgb %b0, %h0 ;"
		"rorl $16, %0 ;"
		"xchgb %b0, %h0 ;"
		"xchgb %b1, %h1 ;"
		"rorl $16, %1 ;"
		"xchgb %b1, %h1 ;"
		"xchgl %0, %1 ;"
		: "=q" (((kuint32*)value)[0]), "=q" (((kuint32*)value)[1])
		: "0" (((kuint32*)value)[0]), "1" (((kuint32*)value)[1])
	);
#else
	asm volatile(
		"bswapl %0 ;"
		"bswapl %1 ;"
		"xchgl %0, %1 ;"
		: "=q" (((kuint32*)value)[0]), "=q" (((kuint32*)value)[1])
		: "0" (((kuint32*)value)[0]), "1" (((kuint32*)value)[1])
	);
#endif
}

