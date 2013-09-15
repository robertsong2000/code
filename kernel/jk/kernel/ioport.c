/*      
 * Copyright (C) 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 *      
 * Contributor: Jimchyun Huang <jimchyun@ccns.ncku.edu.tw>
 * 
 */

#include "internal.h"

kfunction kuint kioport_in_kuint(kuint port) 
{
	register kuint value;
	asm volatile(
		"inl %%dx, %%eax ;"
		: "=a" (value)
		: "d" (port), "0" (value)
	);
	return value;
}

kfunction kuint8 kioport_in_kuint8(kuint port) 
{
	register kuint value;
	asm volatile(
		"inb %%dx, %%al ;"
		: "=a" (value)
		: "d" (port), "0" (value)
	);
	return value;
}

kfunction kuint16 kioport_in_kuint16(kuint port) 
{
	register kuint value;
	asm volatile(
		"inw %%dx, %%ax ;"
		: "=a" (value)
		: "d" (port), "0" (value)
	);
	return value;
}

kfunction kuint32 kioport_in_kuint32(kuint port) 
{
	register kuint value;
	asm volatile(
		"inl %%dx, %%eax ;"
		: "=a" (value)
		: "d" (port), "0" (value)
	);
	return value;
}

kfunction kuint64 kioport_in_kuint64(kuint port) 
{
	return 0;
}

kfunction void kioport_out_kuint(kuint port, kuint value) 
{
	asm volatile(
		"outl %%eax, %%dx ;"
		:
		: "d" (port), "a" (value)
	);
}

kfunction void kioport_out_kuint8(kuint port, kuint8 value) 
{
	asm volatile(
		"outb %%al, %%dx ;"
		:
		: "d" (port), "a" (value)
	);
}

kfunction void kioport_out_kuint16(kuint port, kuint16 value) 
{
	asm volatile(
		"outw %%ax, %%dx ;"
		:
		: "d" (port), "a" (value)
	);
}

kfunction void kioport_out_kuint32(kuint port, kuint32 value) 
{
	asm volatile(
		"outl %%eax, %%dx ;"
		:
		: "d" (port), "a" (value)
	);
}

kfunction void kioport_out_kuint64(kuint port, kuint64 value) 
{
	
}

kfunction void kioport_in_kuint_array(kuint port, kuint* array, kuint length) 
{
	asm volatile(
		"cld ;"
		"rep ;"
		"insl ;"
		: "=c" (length), "=D" (array)
		: "d" (port), "0" (length), "1" (array)
	);
}

kfunction void kioport_in_kuint8_array(kuint port, kuint8* array, kuint length) 
{
	asm volatile(
		"cld ;"
		"rep ;"
		"insb ;"
		: "=c" (length), "=D" (array)
		: "d" (port), "0" (length), "1" (array)
	);
}

kfunction void kioport_in_kuint16_array(kuint port, kuint16* array, kuint length)
{
	asm volatile(
		"cld ;"
		"rep ;"
		"insw ;"
		: "=c" (length), "=D" (array)
		: "d" (port), "0" (length), "1" (array)
	);
}

kfunction void kioport_in_kuint32_array(kuint port, kuint32* array, kuint length)
{
	asm volatile(
		"cld ;"
		"rep ;"
		"insl ;"
		: "=c" (length), "=D" (array)
		: "d" (port), "0" (length), "1" (array)
	);
}

kfunction void kioport_in_kuint64_array(kuint port, kuint64* array, kuint length) {
}

kfunction void kioport_out_kuint_array(kuint port, kuint* array, kuint length)
{
	asm volatile(
		"cld ;"
		"rep ;"
		"outsl ;"
		: "=c" (length), "=D" (array)
		: "d" (port), "0" (length), "1" (array)
	);
}

kfunction void kioport_out_kuint8_array(kuint port, kuint8* array, kuint length)
{
	asm volatile(
		"cld ;"
		"rep ;"
		"outsb ;"
		: "=c" (length), "=D" (array)
		: "d" (port), "0" (length), "1" (array)
	);
}

kfunction void kioport_out_kuint16_array(kuint port, kuint16* array, kuint length){
	asm volatile(
		"cld ;"
		"rep ;"
		"outsw ;"
		: "=c" (length), "=D" (array)
		: "d" (port), "0" (length), "1" (array)
	);
}

kfunction void kioport_out_kuint32_array(kuint port, kuint32* array, kuint length) {
	asm volatile(
		"cld ;"
		"rep ;"
		"outsl ;"
		: "=c" (length), "=D" (array)
		: "d" (port), "0" (length), "1" (array)
	);
}

kfunction void kioport_out_kuint64_array(kuint port, kuint64* array, kuint length)
{
}

