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
#include "kinterface.h"

kuint8* kernel_info[] = {
	(kuint8 *)"kernel_name = JK",
	(kuint8 *)"kernel_version = 0.0.0",
	(kuint8 *)"kernel_author = Jimchyun Huang <jimchyun@ccns.ncku.edu.tw>,"
		  		  "Robert Fitzsimons",
	(kuint8 *)"kernel_license = GPL",
	KNULL,

};

struct kinterface _kinterface __attribute__ ((aligned(KPAGESIZE))) = {
	/*
	 * Kernel Interface
	 */

	kinterface_check_version,
	kinterface_kernel_info,

	/*
	 * Kernel Interface: Modules
	 */

	kmodule_get,
	kmodule_free,

	/*
	 * Kernel Interface: Atomic Operation's
	 */

	katomic_get,
	katomic_set,

	katomic_inc,
	katomic_dec,
	katomic_add,
	katomic_sub,

	katomic_dec_and_test,
	katomic_inc_and_test,

	katomic_compare,
	katomic_compare_and_set_if_equal,

	katomic_mask_set,
	katomic_mask_clear,

	katomic_bit_get,
	katomic_bit_set,
	katomic_bit_reset,

	katomic_bit_test_and_set,
	katomic_bit_test_and_reset,

	/*
	 * Kernel Interface: Printing
	 */

	kprintf,
	kvprintf,
	kvsprintf,

	/*
	 * Kernel Interface: IRQ's
	 */

	kirq_assign_irq,
	kirq_unassign_irq,
	kirq_enable_irq,
	kirq_disable_irq,
	kirq_enable_all,
	kirq_disable_all,

	/*
	 * Kernel Interface: Input Output Port's
	 */

	kioport_in_kuint,
	kioport_in_kuint8,
	kioport_in_kuint16,
	kioport_in_kuint32,
	kioport_in_kuint64,

	kioport_out_kuint,
	kioport_out_kuint8,
	kioport_out_kuint16,
	kioport_out_kuint32,
	kioport_out_kuint64,

	kioport_in_kuint_array,
	kioport_in_kuint8_array,
	kioport_in_kuint16_array,
	kioport_in_kuint32_array,
	kioport_in_kuint64_array,

	kioport_out_kuint_array,
	kioport_out_kuint8_array,
	kioport_out_kuint16_array,
	kioport_out_kuint32_array,
	kioport_out_kuint64_array,

	/*
	 * Kernel Interface: Spinlock
	 */

	kspinlock_lock,
	kspinlock_unlock,
	kspinlock_lock_irqsave,
	kspinlock_unlock_irqrestore,

	/*
	 * Kernel Interface: Read/Write Lock
	 */

	krwlock_read_lock,
	krwlock_read_unlock,
	krwlock_write_lock,
	krwlock_write_unlock,

	/*
	 * Kernel Interface: Thread's
	 */

	kthread_create,

	kthread_current,

	kthread_yield,

	kthread_kill,
	kthread_suspend,
	kthread_resume,
	kthread_sleep,

	kthread_priority_set,
	kthread_priority_get,
	kthread_key_set,
	kthread_key_get,

	/*
	 * Kernel Interface: Time
	 */

	ktime_get,
	ktime_set,

	/*
	 * Kernel Interface: Timer's
	 */

	ktimer_add,
	ktimer_remove,

	/*
	 * Kernel Interface: Liner Memory
	 */

	kmemory_linear_page_allocate,
	kmemory_linear_page_unallocate,

	kmemory_linear_page_map,
	kmemory_linear_page_unmap,

	kmemory_linear_read_kuint,
	kmemory_linear_read_kuint8,
	kmemory_linear_read_kuint16,
	kmemory_linear_read_kuint32,
	kmemory_linear_read_kuint64,

	kmemory_linear_write_kuint,
	kmemory_linear_write_kuint8,
	kmemory_linear_write_kuint16,
	kmemory_linear_write_kuint32,
	kmemory_linear_write_kuint64,

	kmemory_linear_read_kuint_array,
	kmemory_linear_read_kuint8_array,
	kmemory_linear_read_kuint16_array,
	kmemory_linear_read_kuint32_array,
	kmemory_linear_read_kuint64_array,

	kmemory_linear_write_kuint_array,
	kmemory_linear_write_kuint8_array,
	kmemory_linear_write_kuint16_array,
	kmemory_linear_write_kuint32_array,
	kmemory_linear_write_kuint64_array,

	/*
	 * Kernel Interface: Virtual Memory
	 */

	kmemory_virtual_page_allocate,
	kmemory_virtual_page_unallocate,

	kmemory_virtual_copy,
	kmemory_virtual_fill,

	/*
	 * Kernel Interface: Processor
	 */

	kprocessor_flags_save,
	kprocessor_flags_restore,

	kprocessor_byteswap_kuint,
	kprocessor_byteswap_kuint8,
	kprocessor_byteswap_kuint16,
	kprocessor_byteswap_kuint32,
	kprocessor_byteswap_kuint64,

	/*
	 * Kernel Interface: Mutex's
	 */

	kmutex_create,
	kmutex_destroy,
	kmutex_lock,
	kmutex_unlock,
	kmutex_test,

	/*
	 * Kernel Interface: Conditional Variables
	 */

	kcondition_create,
	kcondition_destroy,
	kcondition_signal,
	kcondition_broadcast,
	kcondition_wait,
	kcondition_timedwait,

};

kfunction kuint kinterface_check_version(kuint interface_version, kuint kernel_version) 
{
	if(
			(interface_version == KERNEL_INTERFACE_VERSION)
			&& (kernel_version == KERNEL_IMPLEMENTATION_VERSION)
	  ) {
		return 1;
	}

	return 0;
}

kfunction kuint8** kinterface_kernel_info(void) {
	return kernel_info;
}

