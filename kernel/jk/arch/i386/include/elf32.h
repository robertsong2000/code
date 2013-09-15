/*
 * Copyright (C) 2001 Robert Fitzsimons
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 */

#ifndef _rjk_elf32_h
#define _rjk_elf32_h

#include "internal.h"

#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_PAD 7
#define EI_NIDENT 16

#define ET_EXEC 2
#define EM_386 3
#define ELFCLASS32 1
#define ELFDATA2LSB 1
#define EV_CURRENT 1

#define PT_LOAD 1

struct elf32_header {
	kuint8  e_ident[EI_NIDENT];
	kuint16 e_type;
	kuint16 e_machine;
	kuint32 e_version;
	kuint32 e_entry;
	kuint32 e_phoff;
	kuint32 e_shoff;
	kuint32 e_flags;
	kuint16 e_ehsize;
	kuint16 e_phentsize;
	kuint16 e_phnum;
	kuint16 e_shentsize;
	kuint16 e_shnum;
	kuint16 e_ststrndx;
};

struct elf32_program_header {
	kuint32 p_type;
	kuint32 p_offset;
	kuint32 p_vaddr;
	kuint32 p_paddr;
	kuint32 p_filesz;
	kuint32 p_memsz;
	kuint32 p_flags;
	kuint32 p_align;
};

kuint read_elf(kuint8* buffer, kuint* entry);

#endif /* _rjk_elf32_h */

