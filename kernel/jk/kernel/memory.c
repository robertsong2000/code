/*
 * Copyright (C) 2000, 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#include "internal.h"

#define VIRTUAL_ALLOCATED 0x00000001
#define VIRTUAL_READWRITE 0x00000002
#define VIRTUAL_USERSUPER 0x00000004
#define VIRTUAL_LINEAR    0x00000200

#define VIRTUAL_ASSIGNED  0x00000200
#define VIRTUAL_ZERO      0x00000400

#define LINEAR_ALLOCATED  0x00000100
#define LINEAR_AVAILABLE  0x00000200

#define ZONE_KERNEL 0x00000002
#define ZONE_DMA    0x00000001
#define ZONE_NORMAL 0x00000000
#define ZONE_COUNT  0x00000003

#define tlb_invalidate() ({ \
	asm volatile( \
		"movl %0, %%cr3 ;" \
		: \
		: "r" (virtual_page_table_directory) \
	); \
})

struct zone {
	kuint next;
	kuint start;
	kuint end;
};

struct vm_page {
	struct vm_page* next;
	kuint reserved;
	kuint count;
	kuint page;
};

#define VM_BLOCK_PAGES_COUNT (KPAGESIZE / sizeof(struct vm_page))

struct vm_block {
	struct vm_page* free;
	kuint free_count;
	struct vm_page* used;
	kuint used_count;
	struct vm_page* pages;
	kuint pages_index;
};

volatile kuint kmemory_spinlock;
kuint* virtual_page_table_directory;
kuint* linear_page_table_directory;
struct zone zones[ZONE_COUNT];
struct vm_block* virtual_block;
kuint virtual_address;

kuint get_free_pages(kuint count, kuint zone);
void set_virtual_entry(kuint vaddr, kuint laddr, kuint flags);
void set_linear_entry(kuint vaddr, kuint laddr, kuint flags);
void clear_pages(kuint address, kuint count);
void copy_pages(kuint destination, kuint source, kuint count);
void linear_block(kuint start, kuint end);
void print_page_table(void);
extern void cleanup_thread(void);

void kmemory_linear_setup(kuint mem_lower, kuint mem_upper, kuint mb_max) {
	kuint linear_address = (((mb_max - 1) & 0xFFFFF000) + KPAGESIZE);
	kuint mem_end = (mem_upper >> 22);
	kuint mem = 0;
	kuint i1;

	linear_page_table_directory = (kuint*)linear_address;
	linear_address += KPAGESIZE;
	clear_pages((kuint)linear_page_table_directory, 1);

	for(i1 = 0; i1 <= mem_end; i1++) {
		kuint* pt;
		kuint i2;

		pt = (kuint*)linear_address;
		linear_address += KPAGESIZE;
		clear_pages((kuint)pt, 1);

		linear_page_table_directory[i1] = ((kuint)pt | LINEAR_ALLOCATED);

		for(i2 = 0; i2 < 1024; i2++) {
			pt[i2] = (LINEAR_AVAILABLE);

			if((mem += KPAGESIZE) >= mem_upper) {
				break;
			}
		}
	}

	linear_block((kuint)linear_page_table_directory, ((kuint)linear_page_table_directory + KPAGESIZE));

	for(i1 = 0; i1 <= mem_end; i1++) {
		kuint addr = linear_page_table_directory[i1];
		if(addr & (LINEAR_ALLOCATED)) {
			addr &= 0xFFFFF000;
			linear_block(addr, (addr + KPAGESIZE));
		}
	}

	if(mem_upper > 0x01000000) {
		zones[ZONE_NORMAL].next = ((linear_address > 0x01000000) ? (linear_address) : (0x01000000));
		zones[ZONE_NORMAL].start = 0x01000000;
		zones[ZONE_NORMAL].end = mem_upper;
	}
	zones[ZONE_DMA].next = (((linear_address > 0x00100000) && (linear_address < 0x01000000)) ? (linear_address) : (0x00100000));
	zones[ZONE_DMA].start = 0x00100000;
	zones[ZONE_DMA].end = min(0x01000000, mem_upper);
	zones[ZONE_KERNEL].next = 0x00001000;
	zones[ZONE_KERNEL].start = 0x00001000;
	zones[ZONE_KERNEL].end = 0x00100000;

	virtual_page_table_directory = (kuint*)get_free_pages(1, ZONE_DMA);
	clear_pages((kuint)virtual_page_table_directory, 1);
	linear_block((kuint)virtual_page_table_directory, ((kuint)virtual_page_table_directory + KPAGESIZE));

	virtual_block = (struct vm_block*)get_free_pages(2, ZONE_DMA);
	clear_pages((kuint)virtual_block, 2);
	linear_block((kuint)virtual_block, ((kuint)virtual_block + (KPAGESIZE * 2)));
	virtual_block->pages = (void*)((kuint)virtual_block + KPAGESIZE);

	virtual_address = 0x10000000;
}

void kmemory_virtual_setup(void) {
	kuint i1;
	kuint i2;

	for(i1 = 0; i1 < 1024; i1++) {
		kuint* pt = (kuint*)linear_page_table_directory[i1];

		if((kuint)pt & (LINEAR_ALLOCATED)) {
			(kuint)pt &= 0xFFFFF000;
			set_virtual_entry((kuint)pt, (kuint)pt, (VIRTUAL_ALLOCATED | VIRTUAL_READWRITE | VIRTUAL_USERSUPER | VIRTUAL_LINEAR));
			set_linear_entry((kuint)pt, (kuint)pt, (LINEAR_ALLOCATED));

			for(i2 = 0; i2 < 1024; i2++) {
				kuint addr = pt[i2];

				if(addr & (LINEAR_ALLOCATED)) {
					addr &= 0xFFFFF000;
					set_virtual_entry(addr, addr, (VIRTUAL_ALLOCATED | VIRTUAL_READWRITE | VIRTUAL_USERSUPER | VIRTUAL_LINEAR));
					set_linear_entry(addr, addr, (LINEAR_ALLOCATED));
				}
			}
		}
	}
}

kfunction kuint kmemory_linear_page_allocate(kuint count, kuint dma) {
	kuint address;
	kuint local;
	kuint i;
	kuint irqsave;

	kspinlock_lock_irqsave(&kmemory_spinlock, &irqsave);

	local = address = get_free_pages(count, ((dma) ? (ZONE_DMA) : (ZONE_NORMAL)));

	if(address) {
		for(i = 0; i < count; i++) {
			set_virtual_entry(local, local, (VIRTUAL_ALLOCATED | VIRTUAL_READWRITE | VIRTUAL_USERSUPER | VIRTUAL_LINEAR));
			set_linear_entry(local, local, (LINEAR_ALLOCATED));
			local += KPAGESIZE;
		}
	}

	tlb_invalidate();

	kspinlock_unlock_irqrestore(&kmemory_spinlock, &irqsave);

	return address;
}

kfunction void kmemory_linear_page_unallocate(kuint address, kuint count) {
	return;
}

kfunction kuint kmemory_linear_page_map(kuint address, kuint count) {
	kuint local = address;
	kuint i;

	kuint irqsave;

	kspinlock_lock_irqsave(&kmemory_spinlock, &irqsave);

	for(i = 0; i < count; i++) {
		set_virtual_entry(local, local, (VIRTUAL_ALLOCATED | VIRTUAL_READWRITE | VIRTUAL_USERSUPER | VIRTUAL_LINEAR));
		set_linear_entry(local, local, (LINEAR_ALLOCATED));
		local += KPAGESIZE;
	}

	tlb_invalidate();

	kspinlock_unlock_irqrestore(&kmemory_spinlock, &irqsave);

	return address;
}

kfunction void kmemory_linear_page_unmap(kuint address, kuint count) {
	return;
}

kfunction void* kmemory_virtual_page_allocate(kuint count, kuint zero) {
	struct vm_page** prev;
	struct vm_page* current;

	kuint irqsave;

	kspinlock_lock_irqsave(&kmemory_spinlock, &irqsave);

	prev = &virtual_block->free;
	current = (*prev);

	while(current) {
		if(current->count >= count) {
			(*prev) = current->next;
			goto found;
		}

		prev = &current->next;
		current = (*prev);
	}

	if(virtual_block->pages_index >= VM_BLOCK_PAGES_COUNT) {
		struct vm_page* page = (struct vm_page*)get_free_pages(1, ZONE_NORMAL);
		if(!page) {
			goto end;
		}

		set_virtual_entry((kuint)page, (kuint)page, (VIRTUAL_ALLOCATED | VIRTUAL_READWRITE | VIRTUAL_USERSUPER | VIRTUAL_LINEAR));
		set_linear_entry((kuint)page, (kuint)page, (LINEAR_ALLOCATED));
		clear_pages((kuint)page, 1);

		virtual_block->pages = page;
		virtual_block->pages_index = 0;
	}

	current = &virtual_block->pages[virtual_block->pages_index++];
	current->count = count;
	current->page = virtual_address;

	virtual_address += (count * KPAGESIZE) + KPAGESIZE;

found:

	{
		kuint count_local = count;
		kuint i1 = (current->page >> 22);
		kuint i2 = ((current->page << 10) >> 22);
		kuint flags = ((zero) ? (VIRTUAL_ASSIGNED | VIRTUAL_ZERO) : (VIRTUAL_ASSIGNED));

		while(i1 < 1024) {
			kuint* pt = (kuint*)(virtual_page_table_directory[i1] & 0xFFFFF000);

			if(!pt) {
				pt = (kuint*)get_free_pages(1, ZONE_NORMAL);
				if(!pt) {
					goto end;
				}

				set_virtual_entry((kuint)pt, (kuint)pt, (VIRTUAL_ALLOCATED | VIRTUAL_READWRITE | VIRTUAL_USERSUPER | VIRTUAL_LINEAR));
				set_linear_entry((kuint)pt, (kuint)pt, (LINEAR_ALLOCATED));

				clear_pages((kuint)pt, 1); 

				virtual_page_table_directory[i1] = ((kuint)pt | VIRTUAL_ALLOCATED | VIRTUAL_READWRITE | VIRTUAL_USERSUPER | VIRTUAL_LINEAR);
			}

			while(i2 < 1024) {
				pt[i2] = flags;

				if(!(--count_local)) {
					goto done;
				}

				i2++;
			}

			i2 = 0;
			i1++;
		}
	}

done:

	current->next = virtual_block->used;
	virtual_block->used = current;

	tlb_invalidate();

	kspinlock_unlock_irqrestore(&kmemory_spinlock, &irqsave);

	return (void*)current->page;

end:

	kspinlock_unlock_irqrestore(&kmemory_spinlock, &irqsave);

	return KNULL;
}

kfunction void kmemory_virtual_page_unallocate(void* pointer) {
	struct vm_page** prev;
	struct vm_page* current;
	kuint irqsave;

	kspinlock_lock_irqsave(&kmemory_spinlock, &irqsave);

	prev = &virtual_block->used;
	current = (*prev);

	while(current) {
		if(current->page == (kuint)pointer) {
			(*prev) = current->next;

			goto found;
		}

		prev = &current->next;
		current = (*prev);
	}

	kprintf("unallocate not found %p\n", pointer);

	goto end;

found:

	{
		kuint count_local = current->count;
		kuint i1 = (current->page >> 22);
		kuint i2 = ((current->page << 10) >> 22);

		while(i1 < 1024) {
			kuint* pt = (kuint*)(virtual_page_table_directory[i1] & 0xFFFFF000);

			while(i2 < 1024) {
				kuint* entry = &pt[i2];

				if(*entry & VIRTUAL_ALLOCATED) {
					set_linear_entry((*entry & 0xFFFFF000), 0, (LINEAR_AVAILABLE));
				}

				*entry = 0;

				if(!(--count_local)) {
					goto done;
				}

				i2++;
			}

			i2 = 0;
			i1++;
		}
	}

done:

	current->next = virtual_block->free;
	virtual_block->free = current;

	tlb_invalidate();

end:

	kspinlock_unlock_irqrestore(&kmemory_spinlock, &irqsave);

	return;
}

kfunction kuint kmemory_linear_read_kuint(kuint address) {
	return *(kuint*)address;
}
kfunction kuint8 kmemory_linear_read_kuint8(kuint address) {
	return *(kuint8*)address;
}
kfunction kuint16 kmemory_linear_read_kuint16(kuint address) {
	return *(kuint16*)address;
}
kfunction kuint32 kmemory_linear_read_kuint32(kuint address) {
	return *(kuint32*)address;
}
kfunction kuint64 kmemory_linear_read_kuint64(kuint address) {
	return *(kuint64*)address;
}

kfunction void kmemory_linear_write_kuint(kuint address, kuint value) {
	*(kuint*)address = value;
}
kfunction void kmemory_linear_write_kuint8(kuint address, kuint8 value) {
	*(kuint8*)address = value;
}
kfunction void kmemory_linear_write_kuint16(kuint address, kuint16 value) {
	*(kuint16*)address = value;
}
kfunction void kmemory_linear_write_kuint32(kuint address, kuint32 value) {
	*(kuint32*)address = value;
}
kfunction void kmemory_linear_write_kuint64(kuint address, kuint64 value) {
	*(kuint64*)address = value;
}

#define read_array(d, s, c, opcode) ({ \
	kuint d0, d1, d2; \
	asm volatile( \
		"cld ;" \
		"rep ;" \
		"#opcode ;" \
		: "=&c" (d0), "=&D" (d1), "=&S" (d2) \
		: "0" (c), "1" (d), "2" (s) \
	); \
})

kfunction void kmemory_linear_read_kuint_array(kuint address, kuint* array, kuint length) {
	read_array(array, address, length, movsl);
}
kfunction void kmemory_linear_read_kuint8_array(kuint address, kuint8* array, kuint length) {
	read_array(array, address, length, movsb);
}
kfunction void kmemory_linear_read_kuint16_array(kuint address, kuint16* array, kuint length) {
	read_array(array, address, length, movsw);
}
kfunction void kmemory_linear_read_kuint32_array(kuint address, kuint32* array, kuint length) {
	read_array(array, address, length, movsl);
}
kfunction void kmemory_linear_read_kuint64_array(kuint address, kuint64* array, kuint length) {
	read_array(array, address, length * 2, movsl);
}

kfunction void kmemory_linear_write_kuint_array(kuint address, kuint* array, kuint length) {
	read_array(address, array, length, movsl);
}
kfunction void kmemory_linear_write_kuint8_array(kuint address, kuint8* array, kuint length) {
	read_array(address, array, length, movsb);
}
kfunction void kmemory_linear_write_kuint16_array(kuint address, kuint16* array, kuint length) {
	read_array(address, array, length, movsw);
}
kfunction void kmemory_linear_write_kuint32_array(kuint address, kuint32* array, kuint length) {
	read_array(address, array, length, movsl);
}
kfunction void kmemory_linear_write_kuint64_array(kuint address, kuint64* array, kuint length) {
	read_array(address, array, length * 2, movsl);
}

kfunction void* kmemory_virtual_copy(void* destination, void* source, kuint size) {
	void* destination_copy = destination;
	kuint d0, d1, d2;

	asm volatile(
		"cld ;"
		"rep ;"
		"movsl ;"
		"testb $0x02, %b3 ;"
		"je 1f ;"
		"movsw ;"
		"1: ;"
		"testb $0x01, %b3 ;"
		"je 2f ;"
		"movsb ;"
		"2: ;"
		: "=&c" (d0), "=&D" (d1), "=&S" (d2)
		: "q" (size), "0" (size / 4), "1" (destination), "2" (source)
	);

	return destination_copy;
}

kfunction void* kmemory_virtual_fill(void* pointer, kuint size, kuint8 value) {
	void* pointer_copy = pointer;
	kuint d0, d1;

	asm volatile(
		"xchgb %%al, %%ah ;"
		"cld ;"
		"rep ;"
		"stosw ;"
		"testb $0x01, %b2 ;"
		"je 1f ;"
		"stosb ;"
		"1: ;"
		: "=&c" (d0), "=&D" (d1)
		: "q" (size), "0" (size / 2), "1" (pointer), "a" (value)
	);

	return pointer_copy;
}

void set_virtual_entry(kuint vaddr, kuint laddr, kuint flags) {
	kuint* pt = &virtual_page_table_directory[(vaddr >> 22)];

	if(!(*pt)) {
		kuint temp = get_free_pages(1, ZONE_NORMAL);

		*pt = (temp | VIRTUAL_ALLOCATED | VIRTUAL_READWRITE | VIRTUAL_USERSUPER | VIRTUAL_LINEAR);
		set_virtual_entry(temp, temp, (VIRTUAL_ALLOCATED | VIRTUAL_READWRITE | VIRTUAL_USERSUPER | VIRTUAL_LINEAR));
		set_linear_entry(temp, temp, (LINEAR_ALLOCATED));

		tlb_invalidate();

		clear_pages(temp, 1); 
	}

	((kuint*)(*pt & 0xFFFFF000))[((vaddr << 10) >> 22)] = (laddr | flags);
}

void set_linear_entry(kuint vaddr, kuint laddr, kuint flags) {
	((kuint*)(linear_page_table_directory[(laddr >> 22)] & 0xFFFFF000))[((laddr << 10) >> 22)] = (vaddr | flags);
}

kuint get_free_pages(kuint count, kuint zone) {
	kuint* lptd = linear_page_table_directory;
	kuint i0;
	kuint i1;
	kuint i2;
	kuint outer_end;
	kuint outer;

	for(i0 = zone; i0 < ZONE_COUNT; i0++) {
		struct zone* zone = &zones[i0];
		kuint c = 0;
		kuint address = 0;

		if(!zone->next) {
			continue;
		}

		i1 = (zone->next >> 22);
		i2 = ((zone->next << 10) >> 22);
		outer_end = (zone->end >> 22);
		outer = 0;

		outer_loop:

		while(i1 < outer_end) {
			if(lptd[i1] & (LINEAR_ALLOCATED)) {
				kuint* lpt = (kuint*)(lptd[i1] & 0xFFFFF000);

				while(i2 < 1024) {
					if(lpt[i2] & (LINEAR_AVAILABLE)) {
						if(!(c++)) {
							address = ((i1 << 22) | (i2 << 12));
						}

						if(c == count) {
							// FIXME 2001-03-23:  Need to mark all the pages as LINEAR_ALLOCATED.
							lpt[i2] = (LINEAR_ALLOCATED);
							zone->next = (address + (count * KPAGESIZE));
							return address;
						}
					} else {
						c = 0;
						address = 0;

					}

					i2++;
				}
			}

			i2 = 0;
			i1++;
		}

		i1 = (zone->start >> 22);
		i2 = ((zone->start << 10) >> 22);
		outer_end = (zone->next >> 22);
		
		if(!outer) {
			outer = 1;
			goto outer_loop;
		}
	}

	kprintf("get_free_pages failed, %d %d", count, zone);

	return 0;
}

void clear_pages(kuint address, kuint count) {
	kuint d0, d1;

	asm volatile(
		"cld ;"
		"rep ;"
		"stosl ;"
		: "=&c" (d0), "=&D" (d1)
		: "0" (count * 1024), "1" (address), "a" (0)
	);
}

void copy_pages(kuint destination, kuint source, kuint count) {
	kuint d0, d1, d2;

	asm volatile(
		"cld ;"
		"rep ;"
		"movsl ;"
		: "=&c" (d0), "=&D" (d1), "=&S" (d2)
		: "0" (count * 1024), "1" (destination), "2" (source)
	);
}

void print_page_table(void) {
	kuint* ptd;
	kuint i1;

	kprintf("Virtual Memory Info\n%#0.8x %#0.8x %#0.8x\n", virtual_address, virtual_block->free_count, virtual_block->used_count);
	kprintf("Zones\n");
	for(i1 = 0; i1 < ZONE_COUNT; i1++) {
		kprintf("%0.8x %0.8x %0.8x\n", zones[i1].next, zones[i1].start, zones[i1].end);
	}
	kprintf("Linear Page Tables\n");
	ptd = linear_page_table_directory;
	for(i1 = 0; i1 < 1024; i1++) {
		if(ptd[i1] & (LINEAR_ALLOCATED)) {
			kuint* pt = (kuint*)(ptd[i1] & 0xFFFFF000);
			kuint i2;

			kprintf("%0.8x %0.8x %0.8x %0.8x\n", i1, (i1 << 22), ptd[i1], &ptd[i1]);

			for(i2 = 0; i2 < 1024; i2++) {
				if(pt[i2] & (LINEAR_ALLOCATED)) {
					kprintf(" %0.8x %0.8x %0.8x %0.8x %0.8x\n", i1, i2, ((i1 << 22) | (i2 << 12)), pt[i2], &pt[i2]);
				}
			}
		}
	}
	kprintf("Virtual Page Tables\n");
	ptd = virtual_page_table_directory;
	for(i1 = 0; i1 < 1024; i1++) {
		if(ptd[i1] & (VIRTUAL_ALLOCATED)) {
			kuint* pt = (kuint*)(ptd[i1] & 0xFFFFF000);
			kuint i2;

			kprintf("%0.8x %0.8x %0.8x %0.8x\n", i1, (i1 << 22), ptd[i1], &ptd[i1]);

			for(i2 = 0; i2 < 1024; i2++) {
				if(pt[i2] & (VIRTUAL_ALLOCATED | VIRTUAL_ASSIGNED)) {
					kprintf(" %0.8x %0.8x %0.8x %0.8x %0.8x\n", i1, i2, ((i1 << 22) | (i2 << 12)), pt[i2], &pt[i2]);
				}
			}
		}
	}
}

void linear_block(kuint start, kuint end) {
	kuint address;

	start = (start & 0xFFFFF000);
	end = (((end - 1) & 0xFFFFF000) + KPAGESIZE);

	address = start;

	while(address < end) {
		kuint i1 = (address >> 22);
		kuint i2 = ((address << 10) >> 22);

		((kuint*)(linear_page_table_directory[i1] & 0xFFFFF000))[i2] = (address | LINEAR_ALLOCATED);

		address += KPAGESIZE;
	}
}

#include "elf32.h"

kuint read_elf(kuint8* buffer, kuint* entry) 
{
	struct elf32_header* elf_h;

	elf_h = (void*)buffer;

#ifdef DEBUG_ELF
	kprintf("elf %x\n", elf_h);
#endif

	if(
		(elf_h->e_ident[EI_MAG0] == 0x7F)
		&& (elf_h->e_ident[EI_MAG1] == 'E')
		&& (elf_h->e_ident[EI_MAG2] == 'L')
		&& (elf_h->e_ident[EI_MAG3] == 'F')
		&& (elf_h->e_ident[EI_CLASS] == ELFCLASS32)
		&& (elf_h->e_ident[EI_DATA] == ELFDATA2LSB)
		&& (elf_h->e_ident[EI_VERSION] == EV_CURRENT)
		&& (elf_h->e_type == ET_EXEC)
		&& (elf_h->e_machine == EM_386)
		&& (elf_h->e_version == EV_CURRENT)
		&& (elf_h->e_entry > 0)
		&& (elf_h->e_phoff > 0)
	  ) {
		kuint i;

#ifdef DEBUG_ELF
		kprintf("elf entry=%x, phnum=%x\n", 
				elf_h->e_entry, elf_h->e_phnum);
#endif
		for(i = 0; i < elf_h->e_phnum; i++) {
			struct elf32_program_header* elf_ph;

			elf_ph = (void*)(buffer + elf_h->e_phoff + (i * elf_h->e_phentsize));

			if(elf_ph->p_type == PT_LOAD) {
				kuint start = (elf_ph->p_vaddr & 0xFFFFF000);
				kuint end = (((elf_ph->p_vaddr + elf_ph->p_memsz - 1) & 0xFFFFF000) + KPAGESIZE);
				kuint count = ((end - start) / KPAGESIZE);
				kuint address = get_free_pages(count, ZONE_NORMAL);

#ifdef DEBUG_ELF
				kprintf("elf %x %x %x %x\n", start, end, count, address);
#endif

				if(address) {
					kuint laddr = address;
					kuint vaddr = start;
					kuint j;

					for(j = 0; j < count; j++) {
						set_virtual_entry(vaddr, laddr, (VIRTUAL_ALLOCATED | VIRTUAL_READWRITE | VIRTUAL_USERSUPER));
						set_linear_entry(vaddr, laddr, (LINEAR_ALLOCATED));
						clear_pages(vaddr, 1);
						laddr += KPAGESIZE;
						vaddr += KPAGESIZE;
					}
				}

#ifdef DEBUG_ELF
				kprintf("elf %d:\n", i);
				kprintf("elf  %x\n", elf_ph->p_type);
				kprintf("elf  %x\n", elf_ph->p_offset);
				kprintf("elf  %x\n", elf_ph->p_vaddr);
				kprintf("elf  %x\n", elf_ph->p_filesz);
				kprintf("elf  %x\n", elf_ph->p_memsz);
#endif
			}
		}

		for(i = 0; i < elf_h->e_phnum; i++) {
			struct elf32_program_header* elf_ph;

			elf_ph = (void*)(buffer + elf_h->e_phoff + (i * elf_h->e_phentsize));

			if(elf_ph->p_type == PT_LOAD) {
				kmemory_virtual_copy(
						(void*)elf_ph->p_vaddr,
						(void*)((kuint)buffer + elf_ph->p_offset),
						elf_ph->p_filesz
						);
#ifdef DEBUG_ELF
				kprintf("elf %x %x\n", 
						(void*)((kuint)buffer + elf_ph->p_offset),
						*(kuint*)((kuint)buffer + elf_ph->p_offset));
				kprintf("elf %x %x\n", elf_ph->p_vaddr, *(kuint*)elf_ph->p_vaddr);
#endif
			}
		}

		tlb_invalidate();

		*entry = elf_h->e_entry;

		return 0;
	}

	return 1;
}

void handle_page_fault(kuint number, struct processor_regs* regs) {
	kuint cr2 = 0;
	kuint* page_table;

	kspinlock_lock(&kmemory_spinlock);

	asm volatile(
		"movl %%cr2, %0 ;"
		: "=r" (cr2)
		: "0" (cr2)
	);

	page_table = (kuint*)(virtual_page_table_directory[(cr2 >> 22)] & 0xFFFFF000);

	if(page_table) {
		kuint* page = (kuint*)&page_table[((cr2 << 10) >> 22)];
		kuint page_local = *page;

		if(page_local & VIRTUAL_ASSIGNED) {
			kuint address = get_free_pages(1, ZONE_NORMAL);
			kuint cr2_local = (cr2 & 0xFFFFF000);

			*page = (address | VIRTUAL_ALLOCATED | VIRTUAL_READWRITE | VIRTUAL_USERSUPER | VIRTUAL_LINEAR);
			set_linear_entry(cr2_local, address, (LINEAR_ALLOCATED));

			if(page_local & VIRTUAL_ZERO) {
				clear_pages(cr2_local, 1); 
			}

			tlb_invalidate();

			kspinlock_unlock(&kmemory_spinlock);

			return;
		}
	}

	kspinlock_unlock(&kmemory_spinlock);

	kprintf(
		"Page Fault\ncr2: %0.8x\neip: %0.8x\nerror: %0.8x\n",
		cr2,
		regs->eip,
		regs->error
	);

	print_stack_trace(regs);

	kthread_kill_current();
}

