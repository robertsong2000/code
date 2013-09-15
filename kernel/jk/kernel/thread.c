/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

#define STATUS_ALIVE   0x00000001
#define STATUS_SLEEP   0x00000002
#define STATUS_SUSPEND 0x00000004

struct thread {
	struct thread* next;
	kuint tid;
	kuint status;
	kint priority;
	void* data;
	void* key;
	void* stack;
	kuint stack_size;
	kuint esp;
	kint64 second;
	kint32 nanosecond;
	kuint8 fpu[108];
};

struct thread* schedule_thread(void);
void cleanup_thread(void);
void thread_switch(struct processor_regs* regs);
struct thread* find_thread(kuint tid);

struct thread* current_thread;
struct thread* thread_list;
volatile kuint kthread_spinlock;

kuint tid_counter;

kuint kthread_setup(void) {
	current_thread = (void*)kmemory_linear_page_allocate(1, 1);

	current_thread->tid = ++tid_counter;
	current_thread->status = STATUS_ALIVE;
	current_thread->priority = -1;
	current_thread->key = KNULL;
	current_thread->stack = 0;
	current_thread->stack_size = 0;
	current_thread->next = current_thread;
	current_thread->esp = ((kuint)current_thread + KPAGESIZE);

	return current_thread->esp;
}

kfunction kuint kthread_create(void (*function)(void* data), void* data, kuint stack_size, kuint flags) {
	struct thread* ntp;
	kuint* stack;
	kuint irqsave;

	if(!(ntp = kmemory_virtual_page_allocate(1, 1))) {
		goto failed;
	}

	if(stack_size) {
		if(!(ntp->stack = kmemory_virtual_page_allocate(stack_size, 0))) {
			goto failed;
		}
		stack = (kuint*)((kuint)ntp->stack + (stack_size * KPAGESIZE));
	} else {
		stack = (kuint*)((kuint)ntp + KPAGESIZE);
	}

	ntp->status = STATUS_ALIVE;
	ntp->data = data;
	ntp->key = KNULL;
	ntp->stack_size = stack_size;
	ntp->esp = ((kuint)stack - ((1 + 1 + 3 + 1 + 7) * 4));

	stack[-1] = (kuint)ntp->data;
	stack[-2] = (kuint)kthread_kill_current;
	stack[-3] = 0x200; /* eflags */
	stack[-4] = 0x10; /* cs */
	stack[-5] = (kuint)function; /* eip */
	stack[-6] = 0; /* error */
	stack[-7] = 0; /* eax */
	stack[-8] = 0; /* ebx */
	stack[-9] = 0; /* ecx */
	stack[-10] = 0; /* edx */
	stack[-11] = 0; /* edi */
	stack[-12] = 0; /* esi */
	stack[-13] = 0; /* ebp */
	stack[-14] = 0; /* esp */

	kspinlock_lock_irqsave(&kthread_spinlock, &irqsave);

	asm volatile(
		"fnsave %0 ;"
		"fnsave %1 ;"
		"frstor %0 ;"
		: "=m" (current_thread->fpu), "=m" (ntp->fpu)
	);

	ntp->next = current_thread->next;
	current_thread->next = ntp;

	ntp->tid = ++tid_counter;

	kspinlock_unlock_irqrestore(&kthread_spinlock, &irqsave);

	asm volatile(
		"int $0x30 ;"
	);

	return ntp->tid;

failed:
	if(ntp) {
		if(ntp->stack) {
			kmemory_virtual_page_unallocate(ntp->stack);
		}
		kmemory_virtual_page_unallocate(ntp);
	}
	return 0;
}

kfunction kuint kthread_current(void) {
	return current_thread->tid;
}

kfunction void kthread_yield(void) {
	asm volatile(
		"int $0x30 ;"
	);
}

kfunction void kthread_sleep(kuint tid, kint64 second, kint32 nanosecond) {
	struct thread* local_thread;

	kspinlock_lock(&kthread_spinlock);

	local_thread = find_thread(tid);
	ktime_get(&local_thread->second, &local_thread->nanosecond);

	local_thread->second += second;
	local_thread->nanosecond += nanosecond;

	local_thread->status |= STATUS_SLEEP;

	kspinlock_unlock(&kthread_spinlock);

	if(local_thread == current_thread) {
		asm volatile(
			"int $0x30 ;"
		);
	}
}

void kthread_kill_current(void) {
	kthread_kill(current_thread->tid);
}

kfunction void kthread_kill(kuint tid) {
	struct thread* local_thread;

	kspinlock_lock(&kthread_spinlock);

	local_thread = find_thread(tid);
	local_thread->status &= ~STATUS_ALIVE;

	kspinlock_unlock(&kthread_spinlock);

	if(local_thread == current_thread) {
		asm volatile(
			"int $0x30 ;"
		);
	}
}

kfunction void kthread_suspend(kuint tid) {
	struct thread* local_thread;

	kspinlock_lock(&kthread_spinlock);

	local_thread = find_thread(tid);
	local_thread->status |= STATUS_SUSPEND;

	kspinlock_unlock(&kthread_spinlock);

	if(local_thread == current_thread) {
		asm volatile(
			"int $0x30 ;"
		);
	}
}

kfunction void kthread_resume(kuint tid) {
	struct thread* local_thread;

	kspinlock_lock(&kthread_spinlock);

	local_thread = find_thread(tid);
	local_thread->status &= ~STATUS_SUSPEND;

	kspinlock_unlock(&kthread_spinlock);

	if(local_thread == current_thread) {
		asm volatile(
			"int $0x30 ;"
		);
	}
}

kfunction void kthread_priority_set(kuint tid, kint priority) {
	struct thread* local_thread;

	kspinlock_lock(&kthread_spinlock);

	local_thread = find_thread(tid);
	local_thread->priority = priority;

	kspinlock_unlock(&kthread_spinlock);
}

kfunction kint kthread_priority_get(kuint tid) {
	struct thread* local_thread;
	kint priority;

	kspinlock_lock(&kthread_spinlock);

	local_thread = find_thread(tid);
	priority = local_thread->priority;

	kspinlock_unlock(&kthread_spinlock);

	return priority;
}

kfunction void* kthread_key_set(kuint tid, void* value) {
	struct thread* local_thread;
	void* old_value;

	kspinlock_lock(&kthread_spinlock);

	local_thread = find_thread(tid);
	old_value = local_thread->key;
	local_thread->key = value;

	kspinlock_unlock(&kthread_spinlock);

	return old_value;
}

kfunction void* kthread_key_get(kuint tid) {
	struct thread* local_thread;
	void* value;

	kspinlock_lock(&kthread_spinlock);

	local_thread = find_thread(tid);
	value = local_thread->key;

	kspinlock_unlock(&kthread_spinlock);

	return value;
}

struct thread* schedule_thread(void) {
	struct thread* ntp;
	kint64 second;
	kint32 nanosecond;

	ktime_get(&second, &nanosecond);

	for(ntp = current_thread->next; ntp; ntp = ntp->next) {
		if(ntp->status & (STATUS_SLEEP)) {
			if(
				(ntp->second < second)
				|| ((ntp->second == second) && (ntp->nanosecond <= nanosecond))
			) {
				ntp->second = 0;
				ntp->nanosecond = 0;
				ntp->status &= ~STATUS_SLEEP;
				break;
			} else {
				continue;
			}
		} else if(ntp->status & (STATUS_SUSPEND)) {
			continue;
		} else if(ntp->status & (STATUS_ALIVE)) {
			break;
		}
	}

	return ntp;
}

void thread_switch(struct processor_regs* regs) {
	kspinlock_lock(&kthread_spinlock);

	asm volatile(
		"fnsave %0 ;"
		: "=m" (current_thread->fpu)
	);

	current_thread->esp = regs->esp;

	current_thread = schedule_thread();

	regs->esp = current_thread->esp;

	asm volatile(
		"frstor %0 ;"
		: "=m" (current_thread->fpu)
	);

	kspinlock_unlock(&kthread_spinlock);
}

void print_stack_trace(struct processor_regs* regs) {
	kuint* stack = (kuint*)regs->ebp;

	kprintf("Stack Trace\n");

	while(stack && stack[0]) {
		kprintf("%0.8x %0.8x %0.8x %0.8x %0.8x\n", stack[1], stack[2], stack[3], stack[4], stack[5]);

		stack = (kuint*)stack[0];
	}
}

struct thread* find_thread(kuint tid) {
	if(current_thread->tid == tid) {
		return current_thread;
	} else {
		struct thread* ntp;

		for(ntp = thread_list; ntp; ntp = ntp->next) {
			if(ntp->tid == tid) {
				return ntp;
			}
		}
	}

	return KNULL;
}

