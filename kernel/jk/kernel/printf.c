/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

/*
 * "%<flags><width><precision><length><specifier>"
 *
 * Flags
 * "#" - Alternate form
 * "0" - Zero padded
 * " " - Space padded
 * Length
 * "hh" - kuint8
 * "h" - kuint16
 * "l" - kuint32
 * "ll" - kuint64
 * Specifier
 * "d", "i" - signed decimal
 * "u" - unsigned decimal
 * "o" - unsigned octal
 * "x" - unsigned hexadecimal
 * "X" - unsigned hexadecimal uppercase
 * "f", "F" - floating point value
 * "c" - char
 * "s" - char* (string)
 * "p" - void* (pointer)
 * "%" - "%"
 */

#define LENGTH_KUINT    0x00000001
#define LENGTH_KFLOAT   0x00000002
#define LENGTH_KUINT8   0x00000010
#define LENGTH_KUINT16  0x00000020
#define LENGTH_KUINT32  0x00000040
#define LENGTH_KUINT64  0x00000080

#define FLAG_ALTERNATE 0x00000100
#define FLAG_ZERO_PAD  0x00000200
#define FLAG_SPACE_PAD 0x00000400
#define FLAG_UNSIGNED  0x00000800
#define FLAG_OCTAL     0x00001000
#define FLAG_HEX       0x00002000
#define FLAG_UPPERCASE 0x00004000
#define FLAG_MINUS     0x00008000

const kuint8 lower_digits[] = "0123456789abcdef";
const kuint8 upper_digits[] = "0123456789ABCDEF";
const kuint8 blank_line[] = " \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07 \x07";

/*
 * Give video_memory a default value so if there is an error in the
 * setup function at least a message will be printed on most PC's.
 */
kuint video_memory = 0x000B8000;

kuint video_next;
kuint8* kprintf_buffer;
volatile kuint kprintf_spinlock;

void write_character(kuint c, kuint8** buffer);
void write_string(kuint8* s, kuint8** buffer);
void write_number_kuint32(kuint32 number, kuint flags, kuint width, kuint8** buffer);
void write_number_kuint64(kuint64 number, kuint flags, kuint width, kuint8** buffer);
void write_number_kfloat(kfloat number, kuint flags, kuint width, kuint8** buffer);

void kprintf_init(void) 
{
	kuint i;

	video_memory = 0x000B8000;

	kmemory_linear_page_map(video_memory, ((80 * 25 * 2) / KPAGESIZE) + 1); 

	for(i = 0; i < (80 * 25 * 2); i += (80 * 2)) {
		kmemory_virtual_copy((void*)(video_memory + i), (void*)blank_line, (80 * 2));
	}

	kprintf_buffer = kmemory_virtual_page_allocate(1, 0);
}

void raw_print(kuint8* buffer) {
	kuint c;

	while((c = *(buffer++))) {
		kioport_out_kuint8(0xE9, c); /* Write out to the bochs debug port.  */

		if(c == '\n') {
			video_next = ((video_next / 160) * 160) + 160;
		} else {
			kmemory_linear_write_kuint8(video_memory + video_next, c);
			video_next += 2;
		}

		if(video_next >= (80 * 25 * 2)) {
			video_next = (80 * 24 * 2);

			kmemory_virtual_copy((void*)(video_memory), (void*)(video_memory + 160), (80 * 24 * 2));
			kmemory_virtual_copy((void*)(video_memory + video_next), (void*)blank_line, (80 * 2));
		}
	}
}

kfunction kint kprintf(kuint8* format, ...) {
	kuint count;
	va_list args;

	va_start(args, format);
	count = kvprintf(format, args);
	va_end(args);

	return count;
}

kfunction kint kvprintf(kuint8* format, va_list args) {
	kuint count;
	kuint irqsave;

	kspinlock_lock_irqsave(&kprintf_spinlock, &irqsave);

	count = kvsprintf(kprintf_buffer, format, args);
	raw_print(kprintf_buffer);

	kspinlock_unlock_irqrestore(&kprintf_spinlock, &irqsave);

	return count;
}

kfunction kint kvsprintf(kuint8* buffer, kuint8* format, va_list args) {
	kuint c;
	kuint8* local_buffer = buffer;

//	write_number_kuint32(kthread_current(), (FLAG_ALTERNATE | FLAG_HEX | FLAG_UNSIGNED | FLAG_ZERO_PAD), 8, &local_buffer);
//	write_character(' ', &local_buffer);

	while((c = *(format++))) {
		if(c != '%') {
			write_character(c, &local_buffer);
			continue;
		} else if((c = *(format++))) {
			void* t;
			kuint temp;
			kuint width = 0;
			kuint flags = 0;

			/* Flags.  */
			while(1) {
				switch(c) {
					case '#':
						flags |= FLAG_ALTERNATE;
						c = *(format++); 
						continue;
					case '0':
						flags |= FLAG_ZERO_PAD;
						c = *(format++); 
						continue;
					case ' ':
						flags |= FLAG_SPACE_PAD;
						c = *(format++); 
						continue;
				}
				break;
			}
			/* Width.  */
			switch(c) {
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					temp = 0;
					while(1) {
						if((c >= '0') && (c <= '9')) {
							temp = ((temp * 10) + (c - '0'));
						} else {
							break;
						}
						c = *(format++); 
					}
					width |= (temp << 16);
					break;
			}
			/* Precision.  */
			if(c == '.') {
				c = *(format++); 
				switch(c) {
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
						temp = 0;
						while(1) {
							if((c >= '0') && (c <= '9')) {
								temp = ((temp * 10) + (c - '0'));
							} else {
								break;
							}
							c = *(format++); 
						}
						width |= temp;
						break;
				}
			}
			/* Length.  */
			switch(c) {
				case 'h':
					if(*(format) == 'h') {
						flags |= LENGTH_KUINT8;
						format++;
					} else { 
						flags |= LENGTH_KUINT16;
					}
					c = *(format++); 
					break;
				case 'l':
					if(*(format) == 'l') {
						flags |= LENGTH_KUINT64;
						format++;
					} else { 
						flags |= LENGTH_KUINT32;
					}
					c = *(format++); 
					break;
			}
			/* Specifier.  */
			switch(c) {
				case '%':
					write_character(c, &local_buffer);
					continue;
				case 'c':
					c = (kuint8)va_arg(args, kuint);
					write_character(c, &local_buffer);
					continue;
				case 's':
					t = (void*)va_arg(args, kuint8*);
					if(t) {
						write_string((kuint8*)t, &local_buffer);
					} else {
						write_string("(null)", &local_buffer);
					}
					continue;
				case 'p':
					t = (void*)va_arg(args, void*);
					if(t) {
						write_number_kuint32((kuint32)t, (flags | FLAG_ALTERNATE | FLAG_HEX | FLAG_UNSIGNED), width, &local_buffer);
					} else {
						write_string("(nil)", &local_buffer);
					}
					continue;
				case 'X':
					flags |= FLAG_UPPERCASE;
				case 'x':
					flags |= FLAG_HEX;
				case 'o':
					if(c == 'o') {
						flags |= FLAG_OCTAL;
					}
				case 'u':
					flags |= FLAG_UNSIGNED;
				case 'd':
				case 'i':
					if(flags & LENGTH_KUINT64) {
						write_number_kuint64((kuint64)va_arg(args, kuint64), flags, width, &local_buffer);
					} else {
						write_number_kuint32((kuint32)va_arg(args, kuint32), flags, width, &local_buffer);
					}
					continue;
				case 'f':
				case 'F':
					write_number_kfloat((kfloat)va_arg(args, kfloat), flags, width, &local_buffer);
					continue;
			}
		}

		write_string("<format error,", &local_buffer);
		write_character(c, &local_buffer);
		write_character('>', &local_buffer);
		break;
	}

	(*local_buffer) = 0;

	return ((kuint)local_buffer - (kuint)buffer);
}

void write_character(kuint c, kuint8** buffer) {
	(*((*buffer)++)) = c;
}

void write_string(kuint8* s, kuint8** buffer) {
	kuint c;

	while((c = *(s++))) {
		write_character(c, buffer);
	}
}

void write_number_kuint32(kuint32 number, kuint flags, kuint width, kuint8** buffer) {
	kuint c;
	kuint8 array[68];
	kuint8* p = array;
	const kuint8* digits = lower_digits;
	kuint base = 10;
	kint pad = (width & 0x3F); /* Don't allow the pad size to be greater then 64.  */

	if(flags & FLAG_HEX) {
		base = 16;
		if(flags & FLAG_UPPERCASE) {
			digits = upper_digits;
		}
	} else if(flags & FLAG_OCTAL) {
		base = 8;
	} else if((!(flags & FLAG_UNSIGNED)) && (number & 0x80000000)) {
		number = (~number) + 1;
		flags |= FLAG_MINUS;
	} 

	if(flags & LENGTH_KUINT8) {
		number = (kuint32)((kuint8)number);
	} else if(flags & LENGTH_KUINT16) {
		number = (kuint32)((kuint16)number);
	}

	*(p++) = '\0';

	do {
		pad--;
		*(p++) = digits[number % base];
	} while((number /= base) > 0);

	if(flags & FLAG_ZERO_PAD) {
		while(pad-- > 0) {
			*(p++) = '0';
		}
	}

	if(flags & FLAG_ALTERNATE) {
		if(flags & FLAG_HEX) {
			if(flags & FLAG_UPPERCASE) {
				*(p++) = 'X';
			} else {
				*(p++) = 'x';
			}
			*(p++) = '0';
		} else if((flags & FLAG_OCTAL) & (!(flags & FLAG_ZERO_PAD))) {
			*(p++) = '0';
		}
	}

	if(flags & FLAG_MINUS) {
		*(p++) = '-';
	}

	while(pad-- > 0) {
		*(p++) = ' ';
	}

	while((c = *(--p))) {
		write_character(c, buffer);
	}
}

void write_number_kuint64(kuint64 number, kuint flags, kuint width, kuint8** buffer) {
}

void write_number_kfloat(kfloat number, kuint flags, kuint width, kuint8** buffer) {
//	kuint32* array = (kuint32*)&number;
//
//	write_number_kuint32(array[2], (HEX | UNSIGNED | SIZE_KUINT16 | PAD_ZERO | 4), buffer);
//	write_number_kuint32(array[1], (HEX | UNSIGNED | SIZE_KUINT32 | PAD_ZERO | 8), buffer);
//	write_number_kuint32(array[0], (HEX | UNSIGNED | SIZE_KUINT32 | PAD_ZERO | 8), buffer);

	kint count = 0;
	kint decimal_places;
	kuint written_one = 0;

	decimal_places = width & 0xFFFF;

	if (0 == decimal_places) {
		decimal_places = 6;
	}

	if (number < 0.0) {
		number *= -1.0;
		write_character ('-', buffer);
	}

	while (number >= 1.0) {
		count++;
		number /= 10.0;
	}

	while ((count > 0) || (decimal_places > 0)) {
		kuint tmp;

		if (0 == count) {
			if (!written_one) {
				write_character ('0', buffer);
			}
			write_character ('.', buffer);
		}

		number *= 10.0;
		tmp = (kuint)number;
		number -= tmp;

		write_character (tmp + '0', buffer);
		written_one = 1;

		if (count >= 0) {
			count--;
			if (count < 0) decimal_places--;
		} else {
			decimal_places--;
		}
	}
}

