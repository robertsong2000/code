/*  exception-mips.c  - mips specific part of Exception Monitor
 *
 */

/* With non GPL files, use following license */
/*
 * Copyright 2006,2008,2009 Sony Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
/* Otherwise with GPL files, use following license */
/*
 *  Copyright 2006,2008,2009 Sony Corporation.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  version 2 of the  License.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/ctype.h>
#include <linux/elf.h>
#include <linux/interrupt.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <linux/pci.h>
#include <linux/console.h>
#include <linux/fs.h>
#include "autoconf.h"
#include <sony/exception.h>
#ifdef CONFIG_EM_DEMANGLE
#include "demangle.h"
#endif
#ifdef CONFIG_SNSC_ALT_BACKTRACE
#include <asm/snsc/snsc_backtrace.h>
#endif

#define CP0_STATUS_NMI			0x00080000
#define CP0_CAUSE_BD			0x80000000

extern struct pt_regs *em_regs;

/*
 * for demangle
 */
#ifdef CONFIG_EM_DEMANGLE
/* static int demangle_flag = DMGL_PARAMS | DMGL_ANSI | DMGL_VERBOSE | DMGL_TYPES; */
static int demangle_flag = DMGL_PARAMS | DMGL_ANSI | DMGL_VERBOSE;
#endif

void em_dump_regs(int argc, char **argv)
{
	em_dump_write("\n[register dump]\n");

	em_dump_write(" 0: r0: 0x%08x at: r1: 0x%08x "
		      "v0: r2: 0x%08x v1: r3: 0x%08x\n",
	    (unsigned long)em_regs->regs[0],(unsigned long)em_regs->regs[1],
	    (unsigned long)em_regs->regs[2],(unsigned long)em_regs->regs[3]);
	em_dump_write("a0: r4: 0x%08x a1: r5: 0x%08x "
		      "a2: r6: 0x%08x a3: r7: 0x%08x\n",
	    (unsigned long)em_regs->regs[4],(unsigned long)em_regs->regs[5],
	    (unsigned long)em_regs->regs[6],(unsigned long)em_regs->regs[7]);
	em_dump_write("t0: r8: 0x%08x t1: r9: 0x%08x "
		      "t2:r10: 0x%08x t3:r11: 0x%08x\n",
	    (unsigned long)em_regs->regs[8],(unsigned long)em_regs->regs[9],
	    (unsigned long)em_regs->regs[10],(unsigned long)em_regs->regs[11]);
	em_dump_write("t4:r12: 0x%08x t5:r13: 0x%08x "
		      "t6:r14: 0x%08x t7:r15: 0x%08x\n",
	    (unsigned long)em_regs->regs[12],(unsigned long)em_regs->regs[13],
	    (unsigned long)em_regs->regs[14],(unsigned long)em_regs->regs[15]);
	em_dump_write("s0:r16: 0x%08x s1:r17: 0x%08x "
		      "s2:r18: 0x%08x s3 r19: 0x%08x\n",
	    (unsigned long)em_regs->regs[16],(unsigned long)em_regs->regs[17],
	    (unsigned long)em_regs->regs[18],(unsigned long)em_regs->regs[19]);
	em_dump_write("s4:r20: 0x%08x s5:r21: 0x%08x "
		      "s6:r22: 0x%08x s7:r23: 0x%08x\n",
	    (unsigned long)em_regs->regs[20],(unsigned long)em_regs->regs[21],
	    (unsigned long)em_regs->regs[22],(unsigned long)em_regs->regs[23]);
	em_dump_write("t8:r24: 0x%08x t9:r25: 0x%08x "
		      "k0:r26: 0x%08x k1:r27: 0x%08x\n",
	    (unsigned long)em_regs->regs[24],(unsigned long)em_regs->regs[25],
	    (unsigned long)em_regs->regs[26],(unsigned long)em_regs->regs[27]);
	em_dump_write("gp:r28: 0x%08x sp:r29: 0x%08x "
		      "s8:r30: 0x%08x ra:r31: 0x%08x\n",
	    (unsigned long)em_regs->regs[28],(unsigned long)em_regs->regs[29],
	    (unsigned long)em_regs->regs[30],(unsigned long)em_regs->regs[31]);

	em_dump_write("hi:   : 0x%08x lo:   : 0x%08x\n",
	    (unsigned long)em_regs->hi, (unsigned long)em_regs->lo);

	em_dump_write("cp0 status:   0x%08x\n"
		      "cp0 badvaddr: 0x%08x\n"
		      "cp0 cause:    0x%08x\n"
		      "cp0 epc:      0x%08x\n\n",
		      (unsigned long)em_regs->cp0_status,
		      (unsigned long)em_regs->cp0_badvaddr,
		      (unsigned long)em_regs->cp0_cause,
		      (unsigned long)em_regs->cp0_epc);
}


#if 0 /* This is mips1 specific */
struct _reg_stat {
	char *desc;
	unsigned mask;
};

static const struct _reg_stat reg_c0_status[] = {
	{"Coprocessor0 Usable", 0x80000000},
	{"Coprocessor1 Usable", 0x40000000},
	{"Coprocessor2 Usable", 0x20000000},
	{"Coprocessor3 Usable", 0x10000000},
	{"Bootstrap-Exception Vectors", 0x00400000},
	{"TLB Shutdown", 0x00200000},
	{"Parity Error", 0x00100000},
	{"Cache Miss", 0x00080000},
	{"Parity Zero", 0x00040000},
	{"Swap Caches", 0x00020000},
	{"Isolate Cache", 0x00010000},
	{"Interrupt Mask", 0x0000ff00},
	{"Kernel/User Mode Old", 0x00000020},
	{"Interrupt Enable Old", 0x00000010},
	{"Kernel/User Mode Previous", 0x00000008},
	{"Interrupt Enable Previous", 0x00000004},
	{"Kernel/User Mode Current", 0x00000002},
	{"Interrupt Enable Current", 0x00000001},
};

void em_dump_regs_detail(int argc, char **argv)
{
	int i;
	em_dump_regs(1, NULL);

	em_dump_write("Investigation of Status Register: 0x%08x\n",
		      em_regs->cp0_status);
	for (i = 0; i < sizeof(reg_c0_status)/sizeof(reg_c0_status[0]); i++) {
		switch (i) {
		case 0:	case 1:	case 2:	case 3:	case 6:
		case 7: /* set by hardware */
			em_dump_write("\t%s: %s\n",reg_c0_status[i].desc,
			      (em_regs->cp0_status & reg_c0_status[i].mask) ?
				"True" : "False");
			break;
		case 4:
			em_dump_write("\t%s: %s\n",reg_c0_status[i].desc,
			      (em_regs->cp0_status & reg_c0_status[i].mask) ?
				"Reset State, VA=0xbfc00180 (kseg1),"
				      " PA=0x1fc00180"
				      : "VA=0x80000080(kseg0), PA=00000080");
			break;
		case 5:
			em_dump_write("\t%s: %s\n",reg_c0_status[i].desc,
			      (em_regs->cp0_status & reg_c0_status[i].mask) ?
				"address does match more than 1 TLB" : 
				"address does *not* match more than 1 TLB");
			break;
		case 8:	case 9:
		case 10: /* set by software */
		case 13: case 15: case 17:
			em_dump_write("\t%s: %s\n",reg_c0_status[i].desc,
			      (em_regs->cp0_status & reg_c0_status[i].mask) ?
				"Enabled":"Disabled");
			break;
		case 11:
			em_dump_write("\t%s: 0x%02x\n",reg_c0_status[i].desc,
			      (em_regs->cp0_status & reg_c0_status[i].mask)
				      >> 8 & 0xff);
			break;
		case 12: case 14: case 16:
			em_dump_write("\t%s: %s\n",reg_c0_status[i].desc,
			      (em_regs->cp0_status & reg_c0_status[i].mask) ?
				"User":"Kernel");
			break;
		default:
			em_dump_write("\t%s: %s\n",reg_c0_status[i].desc,
			      (em_regs->cp0_status & reg_c0_status[i].mask) ?
				"1":"0");
			break;
		break;
		}
	}
}
#endif /* mips1 */

/* EMMA3 specific c0 status for the moment */
struct _reg_stat {
	char *desc;
	unsigned mask;
};
static const struct _reg_stat reg_c0_status[] = {
	{"MIPS IV instruction in user mode Usable", 0x80000000},
	{"Coprocessor1 Usable", 0x40000000},
	{"Coprocessor2 Usable", 0x20000000},
	{"Coprocessor3 Usable", 0x10000000},
	{"Floating Registers",  0x04000000},
	{"Bootstrap Exception Vectors", 0x00400000},
	{"TLB Shutdown occured", 0x00200000},
	{"Soft Reset occured", 0x00100000},
	{"Cache Hit", 0x00040000},
	{"Cache Error", 0x00020000},
	{"Disable cache exception on parity Error", 0x00010000},
	{"Interrupt Mask", 0x0000ff00},
	{"Kernel eXpand 64 bit address", 0x00000080},
	{"Supervisor eXpand 64 bit address", 0x00000040},
	{"User eXpand 64 bit address", 0x00000020},
	{"Kernel/Supervisor/User", 0x00000018},
	{"ErroR Level", 0x00000004},
	{"EXception Level", 0x00000002},
	{"Interrupt Enable", 0x00000001}
};

static const char* cause[] = {
	"Interrupt",			"TLB modification",
	"TLB(load or fetch)",		"TLB(store)",
	"Address error(load or fetch)",	"Address error(store)",
	"Bus error(instruction)",	"Bus error(data)",
	"Syscall",			"Breakpoint",
	"Reserved instruction",		"Coprocessor Unusable",
	"Arithmetic Overflow",		"Trap",
	NULL,				"Floating-Point",
	NULL,				NULL,
	NULL,				NULL,
	NULL,				NULL,
	NULL,				"Watch",
	NULL,				NULL,
	NULL,				NULL,
	NULL,				NULL,
	NULL,				NULL
};

void em_dump_regs_detail(int argc, char **argv)
{
	int i, ksu = 42;

        em_dump_regs(1, NULL);
	em_dump_write("Assuming SS1000 Architecture\n");
	em_dump_write("Investigation of Status Register: 0x%08x\n",
		      em_regs->cp0_status);
	for (i = 0; i < sizeof(reg_c0_status)/sizeof(reg_c0_status[0]); i++) {
		switch (i) {
		case 0: /* True or False */
		case 1: case 2: case 3: case 6: case 7: case 8:
			em_dump_write("\t%s: %s\n",reg_c0_status[i].desc,
			      (em_regs->cp0_status & reg_c0_status[i].mask) ?
				"True" : "False");
			break;
		case 10: /* Disabled or Enabled */
			em_dump_write("\t%s: %s\n",reg_c0_status[i].desc,
			      (em_regs->cp0_status & reg_c0_status[i].mask) ?
				"Disabled":"Enabled");
			break;
		case 12: /* Enabled or Disabled */
		case 13: case 14: case 17: case 18:
			em_dump_write("\t%s: %s\n",reg_c0_status[i].desc,
			      (em_regs->cp0_status & reg_c0_status[i].mask) ?
				"Enabled":"Disabled");
			break;
		case 4:
			em_dump_write("\t%s: %s\n",reg_c0_status[i].desc,
			      (em_regs->cp0_status & reg_c0_status[i].mask) ?
				"32":"16");
			break;
		case 5:
			em_dump_write("\t%s: %s\n",reg_c0_status[i].desc,
			      (em_regs->cp0_status & reg_c0_status[i].mask) ?
				"(kseg1)":"(kseg0)");
			break;
		case 11:
			em_dump_write("\t%s: 0x%02x\n",reg_c0_status[i].desc,
			      (em_regs->cp0_status & reg_c0_status[i].mask)
				>> 8 &0xff);
			break;
		case 15:
			em_dump_write("\t%s: ",reg_c0_status[i].desc);
			ksu = ((em_regs->cp0_status &reg_c0_status[i].mask)
			       >> 3 ) & 0x3;
			switch (ksu) {
			case 0:
				em_dump_write("Kernel\n"); break;
			case 1:
				em_dump_write("Supervisor\n"); break;
			case 2:
				em_dump_write("User\n"); break;
			default:
				em_dump_write("???\n"); break;
			}
			break;
		default: /* case 9, case 16 */
			em_dump_write("\t%s: %s\n",reg_c0_status[i].desc,
			      (em_regs->cp0_status & reg_c0_status[i].mask) ?
				"1":"0");
			break;
		}
	}

	if (2 == ksu &&
	    !(em_regs->cp0_status & reg_c0_status[16].mask) &&
	    !(em_regs->cp0_status & reg_c0_status[17].mask))
		em_dump_write("Operating Mode: User Mode\n");
	else if (1 == ksu &&
		 !(em_regs->cp0_status & reg_c0_status[16].mask) &&
		 !(em_regs->cp0_status & reg_c0_status[17].mask))
		em_dump_write("Operating Mode: Supervisor Mode\n");
	else if (0 == ksu ||
		 (em_regs->cp0_status & reg_c0_status[16].mask) ||
		 (em_regs->cp0_status & reg_c0_status[17].mask))
		em_dump_write("Operating Mode: Kernel Mode\n");
	else
		em_dump_write("Operating Mode: Unknown\n");

	em_dump_write("\nInvestigation of Cause Register: 0x%08x\n\t%s\n\n",
		      em_regs->cp0_cause,
		      cause[(em_regs->cp0_cause >> 2) & 0x1f]);
}

#if 0 /* previous impl */
void em_show_syndrome(void);
void em_dump_regs_detail(int argc, char **argv)
{
        em_dump_regs(1, NULL);
	em_show_syndrome();
}
#endif

void em_dump_stack(int argc, char **argv)
{
	int i = 0;
	unsigned long *stack;
	unsigned long *tail;
	unsigned long stackdata;

	unsigned long *pc = (unsigned long *)em_regs->cp0_epc;
	unsigned long insn;
	int normal_stack = 1;

	if (__get_user(insn, pc)) {
		normal_stack = 0;
	}

	if (normal_stack) {
		unsigned long *sp = (unsigned long *)em_regs->regs[29];
		stack = sp - ((unsigned int)sp %16) / 4;
		tail  = stack + PAGE_SIZE / 4;
	} else {
#define dump_stack_addr 0x7fff6900
#define dump_stack_size 0x00001700
		stack = (unsigned long *)dump_stack_addr;
		tail  = stack + dump_stack_size / 4;
	}

	while ((unsigned long) tail > (unsigned long)stack) {
		if (__get_user(stackdata, tail - 1)) {
			if ((unsigned long)tail % PAGE_SIZE != 0) {
			tail = (unsigned long *)
			(((unsigned long)tail / PAGE_SIZE) * PAGE_SIZE);
			} else {
				tail -= PAGE_SIZE / 4;
			}
		} else {
			break;
		}
	}

	em_dump_write("\n[stack dump]\n");

	while ((unsigned long) stack < (unsigned long)tail) {

		if ((i % 4) == 0) {
			em_dump_write("%08x : ", (unsigned long)stack);
		}
		if (__get_user(stackdata, stack++)) {
			em_dump_write(" (Bad stack address)\n");
			break;
		}
		em_dump_write(" 0x%08x", (unsigned long)stackdata);

		if ((i % 4) == 3) {
			em_dump_write("\n");
		}

		++i;
	}
	em_dump_write("\n");
}

#ifdef CONFIG_EM_VERSION_FILE
void em_dump_file(struct file* f)
{
    int buf_size = 4096;
	unsigned char * buf = (unsigned char *)kmalloc(buf_size, GFP_ATOMIC);
    int ret=1, i;
	if (buf == NULL){
		return;
	}
    while((ret=f->f_op->read(f,
						    buf,
						    buf_size,
						    &f->f_pos))){
        unsigned char prchunk[WRITE_BUF_SZ];
        unsigned char *pp = buf;
        int remain;
        for (; pp < (buf+ret)-WRITE_BUF_SZ; pp += (WRITE_BUF_SZ-1)) {
            snprintf(prchunk,WRITE_BUF_SZ,pp);
            em_dump_write(prchunk);
        }
        remain=(buf+ret)-pp+1;
        if(remain>0) {
            snprintf(prchunk,remain,pp);
            prchunk[remain+1]='\0';
            em_dump_write(prchunk);
        }
    }
    kfree(buf);
}

void em_dump_version(int argc, char **argv)
{
    struct file * vfile = filp_open(CONFIG_EM_VERSION_FILE, O_RDONLY, 0444);
    em_dump_write("[software version]\n");
    em_dump_file(vfile);
    filp_close(vfile, NULL);
}

#endif

void em_show_syndrome(void)
{
	unsigned long cp0_status = (unsigned long)em_regs->cp0_status;
	unsigned long cp0_cause = (unsigned long)em_regs->cp0_cause;

	em_dump_write("\n\n[Exception Syndrome]\n");

	em_dump_write("  status:$12 0x%08x, cause:$13 0x%08x ",
		      cp0_status, cp0_cause);
	if (cp0_status & CP0_STATUS_NMI) {
		em_dump_write("NMI or Bus error\n");
	} else {
		em_dump_write("%s\n", cause[(cp0_cause >> 2) & 0x1f]);
	}

	if (cp0_cause & CP0_CAUSE_BD) {
		em_dump_write("badvaddr: $8 0x%08x,  epc:$14 0x%08x "
			      "branch delay -> 0x%08x\n\n",
			      (unsigned long)em_regs->cp0_badvaddr,
			      (unsigned long)em_regs->cp0_epc,
			      (unsigned long)em_regs->cp0_epc+4);
	} else {
		em_dump_write("badvaddr: $8 0x%08x,   epc:$14 0x%08x\n\n",
			      (unsigned long)em_regs->cp0_badvaddr,
			      (unsigned long)em_regs->cp0_epc);
	}
}

#define NOP8() \
        __asm__ __volatile__( \
                ".set\tnoreorder\n\t" \
                "nop;nop;nop;nop;nop;nop;nop;nop\n\t" \
                ".set\treorder");
#define ALIGN4(x) (((x) + 0x3) & 0xfffffffc)

void em_dump_tlb(int argc, char **argv)
{
	int i, start = 0, end = current_cpu_data.tlbsize - 1;
	unsigned int mask, asid, flags;
	unsigned long long entrylo0, entrylo1;
	unsigned long entryhi, addr;

	switch (argc) {
	case 2:
		if ((argv[1][0] == '0') && (toupper(argv[1][1]) == 'X')) {
			argv[1] = &argv[1][2];
		}
		addr =
		    (unsigned long)ALIGN4(simple_strtoul(argv[1], NULL, 16));
		break;
	case 1:
		goto dump_tlb;
		break;
	default:
		em_dump_write("usage: tlb [<address>]\n");
		return;
	}

	em_dump_write("\n[TLB Dump]\n");

	local_irq_save(flags);
	asid = read_c0_entryhi() & 0xff;
	NOP8();
	write_c0_entryhi((addr & PAGE_MASK) | asid);
	NOP8();
	tlb_probe();
	NOP8();
	i = read_c0_index();
	write_c0_entryhi(asid);
	local_irq_restore(flags);
	if (i < 0) {
		em_dump_write("No entry for addr: 0x%08x in TLB.\n", addr);
		return;
	}
	em_dump_write("Entry: %02d, map addr: 0x%08x\n", i, addr);
	start = end = i;

 dump_tlb:
	em_dump_write("TLB  size: %d\n", current_cpu_data.tlbsize);
	em_dump_write("    wired: %d\n", read_c0_wired());

	asid = read_c0_entryhi() & 0xff;

	for (i = start; i <= end; i++) {
		write_c0_index(i);
		NOP8();
		tlb_read();
		NOP8();
		entryhi = read_c0_entryhi();

		if (((entryhi & 0xf0000000) != 0x80000000) &&
		    ((entryhi & 0xff) == asid)) {
			mask = read_c0_pagemask();
			entrylo0 = read_c0_entrylo0();
			entrylo1 = read_c0_entrylo1();

			em_dump_write("Index: %2d, pagemask: 0x%08x, ",
				      i, mask);
			em_dump_write("va: 0x%08lx, asid: 0x%08lx\n",
				      (entryhi & 0xffffe000), asid);

			em_dump_write("  [pa: 0x%08lx, cp0: %d, d: %d, "
				      "v: %d, g: %d]\n",
				      (entrylo0 << 6) & PAGE_MASK,
				      (entrylo0 >> 3) & 7,
				      (entrylo0 & 4) ? 1 : 0,
				      (entrylo0 & 2) ? 1 : 0,
				      (entrylo0 & 1));
			em_dump_write("  [pa: 0x%08lx, cp1: %d, d: %d, "
				      "v: %d, g: %d]\n\n",
				      (entrylo1 << 6) & PAGE_MASK,
				      (entrylo1 >> 3) & 7,
				      (entrylo1 & 4) ? 1 : 0,
				      (entrylo1 & 2) ? 1 : 0,
				      (entrylo1 & 1));
		}
	}
	write_c0_entryhi(asid);
}

#ifdef EMLEGACY_CALLSTACK

/*
 * for callstack
 */
#define LIB_NAME_SIZE 64
char libname[LIB_MAX][LIB_NAME_SIZE];

#define ARRAY_NUM(x) (sizeof(x)/sizeof((x)[0]))
#define ELF_INFO_MAX (1 + LIB_MAX)
static struct _elf_info elf_info[ELF_INFO_MAX];

static struct file *kernel_nm_file = NULL;
static mm_segment_t kernel_nm_fs;
static ssize_t kernel_nm_size;
static unsigned char *kernel_nm = NULL;

#define MAX_CALLSTACK    100
struct callstack callstack[MAX_CALLSTACK];

static void em_get_entry_name(struct file *fp, Elf32_Off offset, char *str)
{
#ifdef CONFIG_EM_DEMANGLE
	char *demangle = NULL;
#endif
	vfs_llseek(fp, offset, 0);
	fp->f_op->read(fp, str, CALLSTACK_STR_SZ, &fp->f_pos);
#ifdef CONFIG_EM_DEMANGLE
	demangle = cplus_demangle_v3(str, demangle_flag);
	if (demangle) {
		memcpy(str, demangle, CALLSTACK_STR_SZ);
		kfree(demangle);
	}
#endif
	str[CALLSTACK_STR_SZ - 1] = '\0';
}

static int
em_get_entry_near(unsigned int index, Elf32_Shdr * sym, Elf32_Shdr * str,
		  struct callstack *cs)
{
	int n, j;
	struct file *fp;
	Elf32_Sym ent;
	Elf32_Off offset = 0x0;
	Elf32_Addr value = 0x0;
	Elf32_Word name = 0x0;
	unsigned long addr_base;
	int flag = 0;

	fp = elf_info[index].file;
	n = sym->sh_size / sizeof(Elf32_Sym);
	addr_base = (index) ?
		elf_info[index].addr_offset - elf_info[index].vaddr : 0;

	vfs_llseek(fp, sym->sh_offset, 0);
	for (j = 0; j < n; j++) {
		vfs_llseek(fp, sym->sh_offset + j*sizeof(Elf32_Sym), 0);
		fp->f_op->read(fp, (char *)&ent, sizeof(Elf32_Sym), &fp->f_pos);
		if ((ELF32_ST_TYPE(ent.st_info) == STT_FUNC) &&
		    ((ent.st_value + addr_base) <= cs->caller) &&
		    ((ent.st_value + addr_base) > value)) {
			offset = str->sh_offset;
			name = ent.st_name;
			em_get_entry_name(fp, offset + name, cs->entry_str);
			if(cs->entry_str[0] != '$'){
				value = ent.st_value + addr_base;
				flag = 1;
			}
		}
	}

	if (!flag) {
		return 0;
	}

	return 1;
}

static void em_find_symbol_near(int elf_cnt, struct callstack *cs)
{
	int i;

	if (cs->caller == 0x0)
		return;

	for (i = 0; i < elf_cnt; i++) {
		if (elf_info[i].file == NULL)
			continue;

		if (elf_info[i].addr_offset > cs->caller ||
		    elf_info[i].addr_end < cs->caller) {
			continue;
		}

		if (elf_info[i].strip) {
			strcpy(cs->entry_str, "## Stripped");
			return;
		}

		if (em_get_entry_near
		    (i, &elf_info[i].sh_symtab, &elf_info[i].sh_strtab, cs)) {
			goto found;
		}

		if (em_get_entry_near
		    (i, &elf_info[i].sh_dynsym, &elf_info[i].sh_dynstr, cs)) {
			goto found;
		}

	}

	/* cs->entry = 0x0; */
	strcpy(cs->entry_str, "## Unknown");
	return;

      found:
	strcat(cs->entry_str, "  ## GUESS");
}

static int
em_get_entry(unsigned int index, Elf32_Shdr * sym, Elf32_Shdr * str,
	     struct callstack *cs)
{
	int n, j;
	struct file *fp;
	Elf32_Sym ent;
	Elf32_Off offset = 0x0;
	Elf32_Word name = 0x0;
	unsigned long addr_base;

	fp = elf_info[index].file;
	n = sym->sh_size / sizeof(Elf32_Sym);
	addr_base = (index) ?
		elf_info[index].addr_offset - elf_info[index].vaddr : 0;

	vfs_llseek(fp, sym->sh_offset, 0);
	for (j = 0; j < n; j++) {
		vfs_llseek(fp, sym->sh_offset + j*sizeof(Elf32_Sym), 0);
		fp->f_op->read(fp, (char *)&ent, sizeof(Elf32_Sym), &fp->f_pos);
		if ((ELF32_ST_TYPE(ent.st_info) == STT_FUNC)){
			if((ent.st_value + addr_base) == cs->entry){
				offset = str->sh_offset;
				name = ent.st_name;
				em_get_entry_name(fp, offset + name, cs->entry_str);
				if(cs->entry_str[0] != '$')
					goto found;
			}
		}
	}
	return 0;

      found:
	return 1;
}

static int get_kernel_entry(unsigned long pc, struct callstack *cs);
static void em_get_entry_str(int elf_cnt, struct callstack *cs)
{
	int i;

	if (cs->entry == 0x0)
		return;

	if(get_kernel_entry(cs->entry, cs))
		return;
	
	for (i = 0; i < elf_cnt; i++) {
		if (elf_info[i].file == NULL)
			continue;

		if (elf_info[i].addr_offset > cs->entry ||
		    elf_info[i].addr_end < cs->entry) {
			continue;
		}

		if (em_get_entry
		    (i, &elf_info[i].sh_symtab, &elf_info[i].sh_strtab, cs)) {
			return;
		}
		if (em_get_entry
		    (i, &elf_info[i].sh_dynsym, &elf_info[i].sh_dynstr, cs)) {
			return;
		}
	}

	/* did not find entry */
	em_find_symbol_near(elf_cnt, cs);
	return;
}

static void em_close_elffile(unsigned int index)
{
	if (elf_info[index].file) {
		set_fs(elf_info[index].fs);
		filp_close(elf_info[index].file, NULL);
		elf_info[index].file = NULL;
	}
}

static int em_open_elffile(unsigned int index)
{
	int i;
	int strip = 2;
	Elf32_Ehdr ehdr;
	Elf32_Shdr shdr;
	Elf32_Shdr sh_shstrtab;
	Elf32_Phdr phdr;
	char *shstrtab;
	struct file *fp;

	/*
	 * open elf file
	 */
	elf_info[index].file =
	    filp_open(elf_info[index].filename, O_RDONLY, 0444);

	if (IS_ERR(elf_info[index].file)) {
		elf_info[index].file = NULL;
		goto fail;
	}
	fp = elf_info[index].file;

	elf_info[index].fs = get_fs();
	set_fs(KERNEL_DS);

	if (!fp->f_op || !fp->f_op->read)
		goto close_fail;

	/*
	 * read elf header
	 */
	fp->f_op->read(fp, (char *)&ehdr, sizeof(Elf32_Ehdr), &fp->f_pos);
	if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0)
		goto close_fail;
	if (!elf_check_arch(&ehdr))
		goto close_fail;

	/*
	 * read program header
	 */
	vfs_llseek(fp, ehdr.e_phoff, 0);
	elf_info[index].vaddr = 0;
	for (i = 0; i < ehdr.e_phnum; i++) {
		fp->f_op->read(fp, (char *)&phdr, sizeof(Elf32_Phdr),
			       &fp->f_pos);
		if (phdr.p_type == PT_LOAD) { /* first PT_LOAD segment */
			elf_info[index].vaddr = phdr.p_vaddr;
			break;
		}
	}
	

	/*
	 * read section header table
	 */
	vfs_llseek(fp,
		   ehdr.e_shoff + sizeof(Elf32_Shdr) * ehdr.e_shstrndx,
		   0);
	fp->f_op->read(fp, (char *)&sh_shstrtab, sizeof(Elf32_Shdr),
		       &fp->f_pos);
	vfs_llseek(fp, sh_shstrtab.sh_offset, 0);
	shstrtab = (char *)kmalloc(sh_shstrtab.sh_size, GFP_ATOMIC);
	fp->f_op->read(fp, shstrtab, sh_shstrtab.sh_size, &fp->f_pos);

	/*
	 * read shsymtab
	 */
	vfs_llseek(fp, ehdr.e_shoff, 0);
	for (i = 0; i < ehdr.e_shnum; i++) {
		fp->f_op->read(fp, (char *)&shdr, sizeof(Elf32_Shdr),
			       &fp->f_pos);
		if (strcmp(&shstrtab[shdr.sh_name], ".dynsym") == 0)
			elf_info[index].sh_dynsym = shdr;
		else if (strcmp(&shstrtab[shdr.sh_name], ".dynstr") == 0)
			elf_info[index].sh_dynstr = shdr;
		else if (strcmp(&shstrtab[shdr.sh_name], ".symtab") == 0) {
			elf_info[index].sh_symtab = shdr;
			strip--;
		} else if (strcmp(&shstrtab[shdr.sh_name], ".strtab") == 0) {
			elf_info[index].sh_strtab = shdr;
			strip--;
		}
	}

	if (!strip)
		elf_info[index].strip = strip;

	kfree(shstrtab);
	return 0;

      close_fail:
	em_close_elffile(index);
      fail:
	return -1;
}

static void em_close_kernel_nm(void)
{
	if (kernel_nm_file) {
		set_fs(kernel_nm_fs);
		filp_close(kernel_nm_file, NULL);
		kernel_nm_file = NULL;
	}
}

static void em_open_kernel_nm(void)
{
	kernel_nm_file = filp_open("/root/nmvmlinux.bin", O_RDONLY, 0444);

	if (IS_ERR(kernel_nm_file)) {
		kernel_nm_file = NULL;
		return;
	}

	kernel_nm_fs = get_fs();
	set_fs(KERNEL_DS);

	if (!kernel_nm_file->f_op || !kernel_nm_file->f_op->read)
		goto close_fail;

	kernel_nm_size = kernel_nm_file->f_dentry->d_inode->i_size;
	kernel_nm = (unsigned char *)vmalloc(kernel_nm_size);
	if (kernel_nm == NULL){
		goto close_fail;
	}

	kernel_nm_size = kernel_nm_file->f_op->read(kernel_nm_file,
						    kernel_nm,
						    kernel_nm_size,
						    &kernel_nm_file->f_pos);

      close_fail:
	em_close_kernel_nm();
}

static int get_kernel_entry(unsigned long pc, struct callstack *cs)
{
	int i, j;
	unsigned long addr = 0;
	unsigned long pre_addr = 0;
	int pre_name_index = 0;

	if (pc < 0x80000000UL || pc > 0x85000000UL)
		return 0;

	if (kernel_nm == NULL)
		return -1;

	cs->entry = 0x0;
	cs->entry_str[0] = '\0';

	for (i = 0; i < kernel_nm_size; ) {
		pre_addr = addr;
		memcpy(&addr, &kernel_nm[i], sizeof(addr));
		i += sizeof(addr);

		if (addr >= pc)
			goto found;

		pre_name_index = i;
		while (kernel_nm[i] != '\0') {
			i++;
			if (i >= kernel_nm_size)
				break;
		}
		i++;
	}

 found:
	if ((addr > pc) || (i >= kernel_nm_size)) {
		cs->entry = pre_addr;
		i = pre_name_index;
	} else {
		cs->entry = addr;
	}

	j = 0;
	while (kernel_nm[i] != '\n' && j < CALLSTACK_STR_SZ) {
		cs->entry_str[j++] = kernel_nm[i];
		if (i >= kernel_nm_size - 1)
			break;
		i++;
	}
	cs->entry_str[j] = '\0';

	return 0;
}

extern char *em_get_execname(void);
static int max_callstack = -1;
void em_get_callstack(void)
{
	int i;
	unsigned long insn;
	unsigned long *wk_p;
	char *short_name;
	char *path;
	int elf_cnt = 1;
	unsigned long cp0_cause = (unsigned long)em_regs->cp0_cause;
	unsigned long *pc = (unsigned long *)em_regs->cp0_epc;
	unsigned char *sp = (unsigned char *)em_regs->regs[29];
	unsigned long *ra = (unsigned long *)em_regs->regs[31];
	unsigned long *badvaddr = (unsigned long *)em_regs->cp0_badvaddr;
	unsigned long inst = 0x0;
	unsigned long *factor = 0x0;
	unsigned long *caller = 0x0;
	unsigned long *next = 0x0;
	unsigned long *entry = 0x0;
	char buf[LIB_NAME_SIZE];
	struct vm_area_struct *vm = NULL;

	for (i = 0; i < MAX_CALLSTACK; i++) {
		callstack[i].entry = 0x0;
		callstack[i].caller = 0x0;
		callstack[i].entry_str[0] = '\0';
	}

	for (i = 0; i < ELF_INFO_MAX; i++) {
		elf_info[i].filename = 0;
		elf_info[i].sh_dynsym.sh_size = 0;
		elf_info[i].sh_dynstr.sh_size = 0;
		elf_info[i].sh_symtab.sh_size = 0;
		elf_info[i].sh_strtab.sh_size = 0;
		elf_info[i].addr_offset = 0;
		elf_info[i].addr_end = 0;
		elf_info[i].strip = 1;
	}

	em_open_kernel_nm();

	/*
	 * set elf_info
	 */
	elf_info[0].filename = em_get_execname();
	short_name = elf_info[0].filename;
	for (i = 0; elf_info[0].filename[i]; i++)
		if (elf_info[0].filename[i] == '/')
			short_name = &elf_info[0].filename[i + 1];
	if (current->mm != NULL)
		vm = current->mm->mmap;
	for (; vm != NULL; vm = vm->vm_next) {
		if (vm->vm_flags & VM_WRITE)
			continue;
		if (vm->vm_file == NULL)
			continue;
		if (vm->vm_file->f_dentry) {
			if (strcmp
			    (vm->vm_file->f_dentry->d_name.name,
			     short_name) == 0) {
				elf_info[0].addr_offset = vm->vm_start;
				elf_info[0].addr_end = vm->vm_end;
			}
		}
	}

	if (current->mm != NULL)
		vm = current->mm->mmap;
	for (i = 0; i < ARRAY_NUM(libname) && vm != NULL; vm = vm->vm_next) {
		if ((vm->vm_flags & (VM_READ | VM_EXEC)) != (VM_READ | VM_EXEC))
			continue;
		if (vm->vm_flags & VM_WRITE)	/* assume text is r-x and text
						   seg addr is base addr */
			continue;
		if (vm->vm_flags & VM_EXECUTABLE)
			continue;
		if (vm->vm_file == NULL)
			continue;

		path = d_path(vm->vm_file->f_dentry, vm->vm_file->f_vfsmnt,
			      buf, sizeof(buf));
		buf[sizeof(buf) - 1] = '\0';

		if (strcmp(path, "/lib/ld.so.1") == 0)
			continue;
		if (strcmp(path, "/devel/lib/ld.so.1") == 0)
			continue;
		if (strcmp(path, "/lib/sonyld.so") == 0)
			continue;

		strcpy(libname[i], path);
		elf_info[elf_cnt].filename = libname[i];
		elf_info[elf_cnt].addr_offset = vm->vm_start;
		elf_info[elf_cnt].addr_end = vm->vm_end;
		elf_cnt++;
		i++;
	}

	for (i = 0; i < elf_cnt; i++) {
		if (em_open_elffile(i) == -1)
			em_dump_write("\n\tWARNING: file not found: %s\n",
				      elf_info[i].filename);
	}

	/*
	 * set callstack
	 */
	i = 0;
	max_callstack = 0;

	if (((((cp0_cause >> 2) & 0x1f) == 2) /* TLB load */
	     || (((cp0_cause >> 2) & 0x1f) == 4)) /* Address load error */
	    && (pc == badvaddr)) {
		pc = ra;
	}

	/* address which exception occured */
	factor = (cp0_cause & CP0_CAUSE_BD) ? pc + 1 : pc;

	callstack[0].caller = (unsigned long)factor;
	if (get_kernel_entry(callstack[0].caller, &callstack[0]) < 0)
		goto fail;

	caller = ra;

	if (__get_user(inst, pc)) {
		printk("WARNING bad epc: 0x%08x\n", (unsigned)pc);
		printk("\tNot going to get call stack.\n");
		goto fail;
	}

	while (1) {
		inst = *pc & 0xffff0000;
		if (inst == 0xafbf0000) { 	/* sw $ra,xx($sp) */
			wk_p = (unsigned long*)(sp + (*pc & 0x0000ffff));
			if (__get_user(insn, wk_p)) {
				goto fail;
			}
			caller = *((unsigned long **)wk_p);
		} else if (inst == 0x27bd0000) { /* addiu $sp,%sp,xx */
			if (*pc & 0x00008000) {
				sp += 0x10000 - (*pc & 0x0000ffff);
			}
		} else if (inst == 0x3c1c0000) { /* lui $gp,xx */
			entry = pc;
			break;
		} else if (*pc == 0x03e00008) { /* jr $ra */
			entry = pc + 2;
			break;
		}

		--pc;
		if (pc + 2 < entry) {
			callstack[0].entry = (unsigned long)entry;
			callstack[0].caller = (unsigned long)caller;
			goto fail;
		}
		if (__get_user(insn, pc)) {
			goto fail;
		}
	}

	callstack[0].entry = (unsigned long)entry;
	callstack[0].caller = (unsigned long)factor;
	em_get_entry_str(elf_cnt, &callstack[0]);
	max_callstack++;

	while (caller != 0x0) {
		if (get_kernel_entry((unsigned long)caller, 
				     &callstack[max_callstack]) < 0)
			goto fail;

		next = entry = 0x0;
		pc = caller - 2;
		if (__get_user(insn, pc)) {
			goto fail;
		}

		while (1) {
			inst = *pc & 0xffff0000;
			if (inst == 0xafbf0000) { /* sw $ra,xx($sp) */
				wk_p = (unsigned long *)
					(sp + (*pc & 0x0000ffff));
				if (__get_user(insn, wk_p)) {
					goto fail;
				}
				next = *((unsigned long **)wk_p);
			} else if (inst == 0x27bd0000) { /* addiu $sp,$sp,xx */
				if (*pc & 0x00008000) {
					sp += 0x10000 - (unsigned long)
						(*pc & 0x0000ffff);
				}
			} else if (inst == 0x3c1c0000) { /* lui $gp,xx */
				entry = pc;
				break;
			} else if (*pc == 0x03e00008) { /* jr $ra */
				entry = pc + 2;
				break;
			}

			--pc;
			if (pc + 2 < entry) {
				callstack[max_callstack].entry = 
					(unsigned long)entry;
				callstack[max_callstack].caller = 
					(unsigned long)caller;
				goto fail;
			}
			if (__get_user(insn, pc)) {
				goto fail;
			}
		}

		callstack[max_callstack].entry = (unsigned long)entry;
		callstack[max_callstack].caller = (unsigned long)(caller - 2);
		em_get_entry_str(elf_cnt, &callstack[max_callstack]);
		max_callstack++;

		if (max_callstack >= MAX_CALLSTACK)
			break;

		caller = next;
		if (__get_user(insn, caller)) {
			caller = 0x0;
		}
	}

 fail:
	/*
	 * close
	 */
	for (i = 0; i < elf_cnt; i++)
		em_close_elffile(i);
	vfree(kernel_nm);
}

void em_dump_callstack_emlegacy(void)
{
	int i;

	em_dump_write("\n[call stack (emlegacy)]\n");

	/* dump kernel stack */
	if (!not_interrupt) {
		em_dump_write("Exception Monitor invoked from kernel mode:\n");
		dump_stack();
		return;
	}

	if (callstack[0].caller == 0x0) {
		em_dump_write("Could not get callstack info\n");
		return;
	}

	switch (monitor_mode) {
	case 1:
		em_dump_write
		    ("    function entry : 0x%08x [ factor : 0x%08x ]\n",
		     callstack[0].entry, callstack[0].caller);
		for (i = 1; callstack[i].caller != 0x0 &&
			 i < MAX_CALLSTACK; i++) {
			em_dump_write
			    (" <- function entry : 0x%08x [ caller : 0x%08x ]\n",
			     callstack[i].entry, callstack[i].caller);
		}
		break;

	case 2:
	case 3:
		em_dump_write("    function entry : 0x%08x "
			      "[ factor : 0x%08x ] -- %s \n",
			      callstack[0].entry,
			      callstack[0].caller, callstack[0].entry_str);
		for (i = 1; callstack[i].caller != 0x0 &&
			 i < MAX_CALLSTACK; i++) {
			em_dump_write(" <- function entry : 0x%08x "
				      "[ caller : 0x%08x ] -- %s \n",
				      callstack[i].entry,
				      callstack[i].caller,
				      callstack[i].entry_str);
		}
		break;

	default:
		printk("invalid mode for exception monitor\n");
		break;
	}

	em_dump_write("\n");
}
#endif /* EMLEGACY_CALLSTACK */

#ifdef CONFIG_SNSC_ALT_BACKTRACE
static void em_print_symbol(const char *str)
{
#ifdef CONFIG_EM_DEMANGLE
	char *demangle = cplus_demangle_v3(str, demangle_flag);
	if (demangle) {
		em_dump_write("%s", demangle);
		kfree(demangle);
		return;
	}
#endif
	em_dump_write("%s", str);
}

int em_bt_ustack_callback(struct bt_arch_callback_arg *cbarg, void *user)
{
	em_dump_write("[0x%08lx] ", cbarg->ba_addr);
	if (bt_status_is_error(cbarg->ba_status)) {
		em_dump_write("stop backtracing: %s\n", cbarg->ba_str);
		return 0;
	}
	if (cbarg->ba_str[0]) {
		em_print_symbol(cbarg->ba_str);
	}
	else {
		em_dump_write("0x%08lx", cbarg->ba_sym_start);
	}
	if (bt_hash_valid(cbarg)) {
		/* by symlist section */
		const unsigned char *hash = cbarg->ba_hash;
		em_dump_write("+%#lx (%s hash:%02x%02x%02x%02x adj:%ld)\n",
			      cbarg->ba_addr - cbarg->ba_sym_start,
			      bt_file_name(cbarg->ba_file),
			      hash[0], hash[1], hash[2], hash[3],
			      cbarg->ba_adjust);
	}
	else {
		/* by symtab section */
		em_dump_write("+%#lx/%#lx (%s)\n",
			      cbarg->ba_addr - cbarg->ba_sym_start,
			      cbarg->ba_sym_size,
			      bt_file_name(cbarg->ba_file));
	}
	return 0;
}


static void em_dump_callstack_ustack(const char *mode)
{
	em_dump_write("\n[call stack (ustack)]\n");
	bt_ustack(mode, !not_interrupt, em_regs, em_bt_ustack_callback, NULL);
	em_dump_write("\n");
}

int em_bt_kstack_callback(struct bt_arch_callback_arg *cbarg, void *user)
{
	em_dump_write("[0x%08lx] ", cbarg->ba_addr);
	if (!cbarg->ba_str) {
		em_dump_write("0x%08lx\n", cbarg->ba_addr);
		return 0;
	}
	em_dump_write("%s+%#lx/%#lx", cbarg->ba_str,
		      cbarg->ba_addr - cbarg->ba_sym_start, 
		      cbarg->ba_sym_size);
	if (cbarg->ba_modname)
		em_dump_write(" [%s]\n", cbarg->ba_modname);
	else
		em_dump_write("\n");
	return 0;
}

static void em_dump_callstack_kstack(const char *mode)
{
	em_dump_write("\n[call stack (kstack)]\n");
	bt_kstack_current(mode, em_bt_kstack_callback, NULL);
	em_dump_write("\n");
}

static void em_dump_callstack_kstack_regs(const char *mode)
{
	em_dump_write("\n[call stack (kstack_regs)]\n");
	bt_kstack_regs(current, em_regs, em_bt_kstack_callback, NULL, 1);
	em_dump_write("\n");
}
#endif

static const char *em_param_match(const char *param, const char *name)
{
	const char *from = strstr(param, name);
	const char *to;

	if (from == NULL)
		return NULL;
	if (from > param && isalnum(from[-1])) /* suffix match */
		return NULL;
	to = from + strlen(name);
	if (isalnum(*to))
		return NULL; /* prefix match */
	return to;
}

static int em_callstack_mode(const char *mode)
{
	int count = 0;
#ifdef EMLEGACY_CALLSTACK
	if (em_param_match(mode, "emlegacy")) {
		em_dump_callstack_emlegacy();
		count++;
	}
#endif
#ifdef CONFIG_SNSC_ALT_BACKTRACE
	if (em_param_match(mode, "kstack")) {
		em_dump_callstack_kstack(mode);
		count++;
	}
	if (!not_interrupt && em_param_match(mode, "kstack_regs")) {
		em_dump_callstack_kstack_regs(mode);
		count++;
	}
	if (em_param_match(mode, "ustack")) {
		em_dump_callstack_ustack(mode);
		count++;
	}
#endif
	return count;
}

void em_dump_callstack(int argc, char **argv)
{
	int count;
	if (!argv || argc <= 1)
		count = em_callstack_mode(em_callstack_param);
	else 
		count = em_callstack_mode(argv[1]);
	if (count == 0)
		em_dump_write("\n[call stack]\nno callstack selected\n\n");
}
