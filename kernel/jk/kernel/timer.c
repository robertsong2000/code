/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

struct timer {
	struct timer* next;
	kuint timer;
	void (*function)(void* data);
	void* data;
	kint64 second;
	kint32 nanosecond;
};

void timer_thread(void* data);

volatile kuint ktimer_spinlock;
struct timer timer_list;
kuint timer_count;

void ktimer_init(void) {
	kthread_create(timer_thread, KNULL, 0, 0);
}

kfunction kuint ktimer_add(void (*function)(void* data), void* data, kint64 second, kint32 nanosecond) {
	struct timer* p;
	struct timer* c;
	struct timer* n;
	kuint timer = 0;
	kint64 local_second;
	kint32 local_nanosecond;

	kspinlock_lock(&ktimer_spinlock);

	ktime_get(&local_second, &local_nanosecond);
	local_second += second;
	local_nanosecond += nanosecond;

	p = &timer_list;
	c = p->next;

	while(c) {
		if((local_second < c->second) || ((local_second == c->second) && (local_nanosecond <= c->nanosecond))) {
			break;
		} else {
			p = c;
			c = c->next;
		}
	}

	if((n = kmemory_virtual_page_allocate(1, 0))) {
		p->next = n;
		n->next = c;

		timer = n->timer = ++timer_count;
		n->function = function;
		n->data = data;
		n->second = local_second;
		n->nanosecond = local_nanosecond;
	}

	kspinlock_unlock(&ktimer_spinlock);

	return timer;
}

kfunction void ktimer_remove(kuint timer) {
}

void timer_thread(void* data) {
	struct timer* p;
	struct timer* c;
	kint64 local_second;
	kint32 local_nanosecond;
	typedef void (*func)(void*);
	void* data;
	
	while(1) {
		kspinlock_lock(&ktimer_spinlock);

		p = &timer_list;
		c = p->next;

		ktime_get(&local_second, &local_nanosecond);

		if(
			(c)
			&& (
				(c->second < local_second)
				|| ((c->second == local_second) && (c->nanosecond <= local_nanosecond))
			)
		) {
			func function = c->function;
			data = c->data;

			p->next = c->next;
			kmemory_virtual_page_unallocate(c);

			kspinlock_unlock(&ktimer_spinlock);

			function(data);
		} else {
			kspinlock_unlock(&ktimer_spinlock);
			kthread_yield();
		}
	}
}

