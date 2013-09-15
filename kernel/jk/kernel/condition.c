/*
 * Copyright (C) 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

#define MAX_CONDITION_ENTRY_COUNT ((KPAGESIZE - sizeof(kcondition)) / sizeof(struct kcondition_entry))
#define CONDITION_FLAG_SIGNALLED 0x00000001

kfunction kcondition* kcondition_create(void) {
	return (kcondition*)kmemory_virtual_page_allocate(1, 0);
}

kfunction void kcondition_destroy(kcondition* condition) {
	kmemory_virtual_page_unallocate(condition);
}

kfunction kuint kcondition_signal(kcondition* condition) {
	struct kcondition_entry* entry;

	kspinlock_lock(&condition->lock);

	if(condition->entry_next) {
		entry = &condition->entry_array[0];

		entry->flags |= CONDITION_FLAG_SIGNALLED;
		kthread_resume(entry->thread);
	}

	kspinlock_unlock(&condition->lock);

	return 0;
}

kfunction kuint kcondition_broadcast(kcondition* condition) {
	struct kcondition_entry* entry;
	kuint index = 0;

	kspinlock_lock(&condition->lock);

	while(index < condition->entry_next) {
		entry = &condition->entry_array[index];

		entry->flags |= CONDITION_FLAG_SIGNALLED;
		kthread_resume(entry->thread);
		index++;
	}

	kspinlock_unlock(&condition->lock);

	return 0;
}

kfunction kuint kcondition_wait(kcondition* condition, kmutex* mutex) {
	struct kcondition_entry* entry;
	kuint index;
	kuint current;

	if(!kmutex_test(mutex)) {
		return -1;
	}

	kspinlock_lock(&condition->lock);

	current = kthread_current();

	entry = &condition->entry_array[condition->entry_next++];
	entry->thread = current;
	entry->mutex_count = mutex->count;
	entry->flags = 0;

	mutex->count = 0;
	katomic_set(&mutex->thread, 0);

	kspinlock_unlock(&condition->lock);

	while(1) {
		index = 0;

		kthread_suspend(current);

		kspinlock_lock(&condition->lock);

		while(index < condition->entry_next) {
			entry = &condition->entry_array[index];

			if((entry->thread == current) && (entry->flags & CONDITION_FLAG_SIGNALLED)) {
				kmutex_lock(mutex);
				mutex->count = entry->mutex_count;

				goto done;
			}

			index++;
		}

		kspinlock_unlock(&condition->lock);
	}

done:

	while((index + 1) < condition->entry_next) {
		condition->entry_array[index] = condition->entry_array[index + 1];

		index++;
	}

	condition->entry_next--;

	kspinlock_unlock(&condition->lock);

	return 0;
}

kfunction kuint kcondition_timedwait(kcondition* condition, kmutex* mutex, kint64 second, kint32 nanosecond) {
	return -1;
}

