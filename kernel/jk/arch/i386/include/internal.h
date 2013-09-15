/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#ifndef _rjk_kinternal_h
#define _rjk_kinternal_h

#include "kinterface.h"

struct multiboot_info {
	kuint flags;
	kuint mem_lower;
	kuint mem_upper;
	kuint boot_device;
	kuint cmdline;
	kuint mods_count;
	kuint mods_addr;
	kuint syms_0;
	kuint syms_1;
	kuint syms_2;
	kuint syms_3;
	kuint mmap_length;
	kuint mmap_addr;
};

struct multiboot_mod {
	kuint mod_start;
	kuint mod_end;
	kuint string;
	kuint reserved;
};

struct processor_regs {
	kuint esp;
	kuint ebp;
	kuint esi;
	kuint edi;
	kuint edx;
	kuint ecx;
	kuint ebx;
	kuint eax;
	kuint error;
	kuint eip;
	kuint cs;
	kuint eflags;
};

struct kmutex {
	volatile kuint thread;
	kuint count;
};

struct kcondition_entry {
	kuint thread;
	kuint mutex_count;
	kuint flags;
};

struct kcondition {
	volatile kuint lock;
	kuint entry_next;
	struct kcondition_entry entry_array[0];
};

#define max(a, b) (((kuint)(a) > (kuint)(b)) ? (kuint)(a) : (kuint)(b))
#define min(a, b) (((kuint)(a) < (kuint)(b)) ? (kuint)(a) : (kuint)(b))

#define LOCK "lock ;"

void kprintf_init(void);
void kirq_init(void);
void ktime_init(void);
void ktimer_init(void);
void kmemory_init(void);
void kthread_kill_current(void);
void print_stack_trace(struct processor_regs* regs);
void raw_print(kuint8* buffer);

extern struct kinterface _kinterface;

/*
 * Kernel Interface
 */

kfunction kuint kinterface_check_version(kuint interface_version, kuint kernel_version);
kfunction kuint8** kinterface_kernel_info(void);

/*
 * Kernel Interface: Modules
 */

kfunction kuint kmodule_get(kuint mod_index, kuint8** data, kuint* length);
kfunction void kmodule_free(void);

/*
 * Kernel Interface: Atomic Operation's
 */

kfunction kuint katomic_get(volatile kuint* atomic);
kfunction void katomic_set(volatile kuint* atomic, kuint value);

kfunction void katomic_inc(volatile kuint* atomic);
kfunction void katomic_dec(volatile kuint* atomic);
kfunction void katomic_add(volatile kuint* atomic, kuint value);
kfunction void katomic_sub(volatile kuint* atomic, kuint value);

kfunction kuint katomic_inc_and_test(volatile kuint* atomic);
kfunction kuint katomic_dec_and_test(volatile kuint* atomic);

kfunction kuint katomic_compare(volatile kuint* atomic, kuint compare);
kfunction kuint katomic_compare_and_set_if_equal(volatile kuint* atomic, kuint compare, kuint value);

kfunction void katomic_mask_set(volatile kuint* atomic, kuint mask);
kfunction void katomic_mask_clear(volatile kuint* atomic, kuint mask);

kfunction kuint katomic_bit_get(volatile kuint* atomic, kuint bit);
kfunction void katomic_bit_set(volatile kuint* atomic, kuint bit);
kfunction void katomic_bit_reset(volatile kuint* atomic, kuint bit);

kfunction kuint katomic_bit_test_and_set(volatile kuint* atomic, kuint bit);
kfunction kuint katomic_bit_test_and_reset(volatile kuint* atomic, kuint bit);

/*
 * Kernel Interface: Printing
 */

kfunction kint kprintf(kuint8* format, ...);
kfunction kint kvprintf(kuint8* format, va_list args);
kfunction kint kvsprintf(kuint8* buffer, kuint8* format, va_list args);

/*
 * Kernel Interface: IRQ's
 */

kfunction kuint kirq_assign_irq(kuint irq, kuint (*handler)(kuint irq));
kfunction void kirq_unassign_irq(kuint irq);
kfunction void kirq_enable_irq(kuint irq);
kfunction void kirq_disable_irq(kuint irq);
kfunction void kirq_enable_all(void);
kfunction void kirq_disable_all(void);

/*
 * Kernel Interface: Input Output Port's
 */

kfunction kuint kioport_in_kuint(kuint port);
kfunction kuint8 kioport_in_kuint8(kuint port);
kfunction kuint16 kioport_in_kuint16(kuint port);
kfunction kuint32 kioport_in_kuint32(kuint port);
kfunction kuint64 kioport_in_kuint64(kuint port);

kfunction void kioport_out_kuint(kuint port, kuint value);
kfunction void kioport_out_kuint8(kuint port, kuint8 value);
kfunction void kioport_out_kuint16(kuint port, kuint16 value);
kfunction void kioport_out_kuint32(kuint port, kuint32 value);
kfunction void kioport_out_kuint64(kuint port, kuint64 value);

kfunction void kioport_in_kuint_array(kuint port, kuint* array, kuint length);
kfunction void kioport_in_kuint8_array(kuint port, kuint8* array, kuint length);
kfunction void kioport_in_kuint16_array(kuint port, kuint16* array, kuint length);
kfunction void kioport_in_kuint32_array(kuint port, kuint32* array, kuint length);
kfunction void kioport_in_kuint64_array(kuint port, kuint64* array, kuint length);

kfunction void kioport_out_kuint_array(kuint port, kuint* array, kuint length);
kfunction void kioport_out_kuint8_array(kuint port, kuint8* array, kuint length);
kfunction void kioport_out_kuint16_array(kuint port, kuint16* array, kuint length);
kfunction void kioport_out_kuint32_array(kuint port, kuint32* array, kuint length);
kfunction void kioport_out_kuint64_array(kuint port, kuint64* array, kuint length);

/*
 * Kernel Interface: Spinlock
 */

kfunction void kspinlock_lock(volatile kuint* lock);
kfunction void kspinlock_unlock(volatile kuint* lock);
kfunction void kspinlock_lock_irqsave(volatile kuint* lock, kuint* irqsave);
kfunction void kspinlock_unlock_irqrestore(volatile kuint* lock, kuint* irqsave);

/*
 * Kernel Interface: Read/Write Lock
 */

kfunction void krwlock_read_lock(volatile kuint* lock);
kfunction void krwlock_read_unlock(volatile kuint* lock);
kfunction void krwlock_write_lock(volatile kuint* lock);
kfunction void krwlock_write_unlock(volatile kuint* lock);

/*
 * Kernel Interface: Thread's
 */

kfunction kuint kthread_create(void (*function)(void* data), void* data, kuint stack_size, kuint flags);

kfunction kuint kthread_current(void);

kfunction void kthread_yield(void);

kfunction void kthread_kill(kuint thread);
kfunction void kthread_suspend(kuint thread);
kfunction void kthread_resume(kuint thread);
kfunction void kthread_sleep(kuint thread, kint64 second, kint32 nanosecond);

kfunction void kthread_priority_set(kuint thread, kint priority);
kfunction kint kthread_priority_get(kuint thread);
kfunction void* kthread_key_set(kuint thread, void* value);
kfunction void* kthread_key_get(kuint thread);

/*
 * Kernel Interface: Time
 */

kfunction void ktime_get(kint64* second, kint32* nanosecond);
kfunction void ktime_set(kint64 second, kint32 nanosecond);

/*
 * Kernel Interface: Timer's
 */

kfunction kuint ktimer_add(void (*function)(void* data), void* data, kint64 second, kint32 nanosecond);
kfunction void ktimer_remove(kuint timer);

/*
 * Kernel Interface: Liner Memory
 */

kfunction kuint kmemory_linear_page_allocate(kuint count, kuint dma);
kfunction void kmemory_linear_page_unallocate(kuint address, kuint count);

kfunction kuint kmemory_linear_page_map(kuint address, kuint count);
kfunction void kmemory_linear_page_unmap(kuint address, kuint count);

kfunction kuint kmemory_linear_read_kuint(kuint address);
kfunction kuint8 kmemory_linear_read_kuint8(kuint address);
kfunction kuint16 kmemory_linear_read_kuint16(kuint address);
kfunction kuint32 kmemory_linear_read_kuint32(kuint address);
kfunction kuint64 kmemory_linear_read_kuint64(kuint address);

kfunction void kmemory_linear_write_kuint(kuint address, kuint value);
kfunction void kmemory_linear_write_kuint8(kuint address, kuint8 value);
kfunction void kmemory_linear_write_kuint16(kuint address, kuint16 value);
kfunction void kmemory_linear_write_kuint32(kuint address, kuint32 value);
kfunction void kmemory_linear_write_kuint64(kuint address, kuint64 value);

kfunction void kmemory_linear_read_kuint_array(kuint address, kuint* array, kuint length);
kfunction void kmemory_linear_read_kuint8_array(kuint address, kuint8* array, kuint length);
kfunction void kmemory_linear_read_kuint16_array(kuint address, kuint16* array, kuint length);
kfunction void kmemory_linear_read_kuint32_array(kuint address, kuint32* array, kuint length);
kfunction void kmemory_linear_read_kuint64_array(kuint address, kuint64* array, kuint length);

kfunction void kmemory_linear_write_kuint_array(kuint address, kuint* array, kuint length);
kfunction void kmemory_linear_write_kuint8_array(kuint address, kuint8* array, kuint length);
kfunction void kmemory_linear_write_kuint16_array(kuint address, kuint16* array, kuint length);
kfunction void kmemory_linear_write_kuint32_array(kuint address, kuint32* array, kuint length);
kfunction void kmemory_linear_write_kuint64_array(kuint address, kuint64* array, kuint length);

/*
 * Kernel Interface: Virtual Memory
 */

kfunction void* kmemory_virtual_page_allocate(kuint count, kuint zero);
kfunction void kmemory_virtual_page_unallocate(void* pointer);

kfunction void* kmemory_virtual_copy(void* destination, void* source, kuint size);
kfunction void* kmemory_virtual_fill(void* pointer, kuint size, kuint8 value);

/*
 * Kernel Interface: Processor
 */

kfunction void kprocessor_flags_save(kuint* flags);
kfunction void kprocessor_flags_restore(kuint* flags);

kfunction void kprocessor_byteswap_kuint(kuint* value);
kfunction void kprocessor_byteswap_kuint8(kuint8* value);
kfunction void kprocessor_byteswap_kuint16(kuint16* value);
kfunction void kprocessor_byteswap_kuint32(kuint32* value);
kfunction void kprocessor_byteswap_kuint64(kuint64* value);

/*
 * Kernel Interface: Mutex's
 */

kfunction kmutex* kmutex_create(void);
kfunction void kmutex_destroy(kmutex* mutex);
kfunction kuint kmutex_lock(kmutex* mutex);
kfunction kuint kmutex_unlock(kmutex* mutex);
kfunction kuint kmutex_test(kmutex* mutex);

/*
 * Kernel Interface: Conditional Variables
 */

kfunction kcondition* kcondition_create(void);
kfunction void kcondition_destroy(kcondition* condition);
kfunction kuint kcondition_signal(kcondition* condition);
kfunction kuint kcondition_broadcast(kcondition* condition);
kfunction kuint kcondition_wait(kcondition* condition, kmutex* mutex);
kfunction kuint kcondition_timedwait(kcondition* condition, kmutex* mutex, kint64 second, kint32 nanosecond);

#endif /* _rjk_kinternal_h */

