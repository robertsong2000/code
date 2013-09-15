/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

#define bcd_to_binary(v) ({ \
	kuint8 __v = (v); \
	((__v & 0xF) + ((__v >> 4) * 10)); \
})

kint64 current_second;
kint32 current_nanosecond;
kuint thread_ticks;
kuint time_ticks;
volatile kuint ktime_spinlock;

void read_rtc(kint64* second, kint32* nanosecond) {
	kuint value;
	kuint local_year;
	kuint local_month;
	kuint local_day;
	kuint local_hour;
	kuint local_minute;
	kuint local_second;

	do {
		kioport_out_kuint8(0x70, 0x0A);
		value = kioport_in_kuint8(0x71);
	} while(value & 0x80);

	kioport_out_kuint8(0x70, 0x32); /* Century */
	local_year = (100 * bcd_to_binary(kioport_in_kuint8(0x71)));
	kioport_out_kuint8(0x70, 0x09); /* Year */
	local_year += bcd_to_binary(kioport_in_kuint8(0x71));
	kioport_out_kuint8(0x70, 0x08); /* Month */
	local_month = bcd_to_binary(kioport_in_kuint8(0x71));
	kioport_out_kuint8(0x70, 0x07); /* Day */
	local_day = bcd_to_binary(kioport_in_kuint8(0x71));
	kioport_out_kuint8(0x70, 0x04); /* Hour */
	local_hour = bcd_to_binary(kioport_in_kuint8(0x71));
	kioport_out_kuint8(0x70, 0x02); /* Minute */
	local_minute = bcd_to_binary(kioport_in_kuint8(0x71));
	kioport_out_kuint8(0x70, 0x00); /* Second */
	local_second = bcd_to_binary(kioport_in_kuint8(0x71));

	if(0 >= (kint)(local_month -= 2)) {
		local_month += 12;
		local_year -= 1;
	}

	*second = ((kint64)(((((local_year / 4) - (local_year / 100) + (local_year / 400) + ((367 * local_month) / 12) + local_day) + (local_year * 365) - 719499) * 24 + local_hour) * 60 + local_minute) * 60 + local_second);
	*nanosecond = 0;
}

kuint rtc_irq_handler(kuint number) {
	kuint8 value;

	kioport_out_kuint8(0x70, 0x0C);
	value = kioport_in_kuint8(0x71); /* Acknowledge interrupt */

	kspinlock_lock(&ktime_spinlock);

	if(value & 0x40) { /* Periodic interrupt */
		current_nanosecond += (1000000000 / 1024);
	}

	if(value & 0x10) { /* Update ended interrupt */
		if(time_ticks--) {
			current_second++;
			current_nanosecond = 0;
		} else {
			read_rtc(&current_second, &current_nanosecond);
			time_ticks = 37;
		}
	}

	kspinlock_unlock(&ktime_spinlock);

	if(!(thread_ticks--)) {
		thread_ticks = 11;
		return 1;
	}

	return 0;
}

void ktime_init(void) {
	kuint8 value;

	do {
		kioport_out_kuint8(0x70, 0x0A);
		value = kioport_in_kuint8(0x71);
	} while(value & 0x80);

	kioport_out_kuint8(0x70, 0x0A);
	value = kioport_in_kuint8(0x71);
	kioport_out_kuint8(0x70, 0x0A);
	kioport_out_kuint8(0x71, ((value & 0xF0) | 0x06)); /* 1024 interrupts per second */

	kioport_out_kuint8(0x70, 0x0B);
	value = kioport_in_kuint8(0x71);
	kioport_out_kuint8(0x70, 0x0B);
	kioport_out_kuint8(0x71, ((value & 0x0F) | 0x52)); /* periodic interrupt, update interrupt, 24 hour mode */

	kioport_out_kuint8(0x70, 0x0C);
	value = kioport_in_kuint8(0x71); /* acknowledge interrupt */

	read_rtc(&current_second, &current_nanosecond);

	kirq_assign_irq(8, rtc_irq_handler);
}

kfunction void ktime_get(kint64* second, kint32* nanosecond) {
	kuint irqsave;

	kspinlock_lock_irqsave(&ktime_spinlock, &irqsave);

	*second = current_second;
	*nanosecond = current_nanosecond;

	kspinlock_unlock_irqrestore(&ktime_spinlock, &irqsave);
}

kfunction void ktime_set(kint64 second, kint32 nanosecond) {
	kuint irqsave;

	kspinlock_lock_irqsave(&ktime_spinlock, &irqsave);

	current_second = second;
	current_nanosecond = nanosecond;

	kspinlock_unlock_irqrestore(&ktime_spinlock, &irqsave);
}

