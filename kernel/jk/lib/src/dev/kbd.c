#include "internal.h"

#include <dev/cons.h>
#include <intr.h>
#include <sys.h>
#include <sys/i8255.h>
#include <sys/i8259.h>
#include <sys/types.h>


#define SCAN_CODES	0x60
#define SCAN_CODE_LSHFT	0x2a
#define SCAN_CODE_RSHFT	0x36
#define SCAN_CODE_LSREL	0xaa
#define SCAN_CODE_RSREL	0xb6
#define SCAN_CODE_LCTRL	0x1d
#define SCAN_CODE_RCTRL	0x5a
#define SCAN_CODE_LCREL	0x9d
#define SCAN_CODE_RCREL	0xda

#define KBDQ_LENGTH	16
#define ESC		0x1b


#define KBD_SHIFT	0x01
#define KBD_CTRL	0x02
static int      flags = 0;

static char     q[KBDQ_LENGTH];
static int      h = 0, t = 0;

static char     asciishift[SCAN_CODES] = {
	'\0', ESC, '!', '@', '#', '$', '%', '^',
	'&', '*', '(', ')', '_', '+', '\b', '\t',
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I',
	'O', 'P', '{', '}', '\n', '\0', 'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',
	'"', '~', '\0', '|', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', '\0', '*',
	'\0', ' ', '\0', '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '7',
	'8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};

static char     asciinormal[SCAN_CODES] = {
	'\0', ESC, '1', '2', '3', '4', '5', '6',
	'7', '8', '9', '0', '-', '=', '\b', '\t',
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
	'o', 'p', '[', ']', '\n', '\0', 'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
	'\'', '`', '\0', '\\', 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', '/', '\0', '*',
	'\0', ' ', '\0', '\0', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '7',
	'8', '9', '-', '4', '5', '6', '+', '1',
	'2', '3', '0', '.', '\0', '\0', '\0', '\0',
	'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};


static void kbdisr()
{
	u_char          code;

	ENTER_CRITICAL_SECTION;
	code = inb(I8255_PORT_A);

	if (code == SCAN_CODE_LCTRL || code == SCAN_CODE_RCTRL)
		flags |= KBD_CTRL;

	else if (code == SCAN_CODE_LCREL || code == SCAN_CODE_RCREL)
		flags &= ~KBD_CTRL;

	else if (code == SCAN_CODE_LSHFT || code == SCAN_CODE_RSHFT)
		flags |= KBD_SHIFT;

	else if (code == SCAN_CODE_LSREL || code == SCAN_CODE_RSREL)
		flags &= ~KBD_SHIFT;

	else if (code > 0 && code < SCAN_CODES) {
		if ((t + 1) % KBDQ_LENGTH != h) {
			if (flags & KBD_SHIFT)
				q[t] = asciishift[code];
			else
				q[t] = asciinormal[code];
			t = (t + 1) % KBDQ_LENGTH;
		}
	}
	/* Issue keyboard eoi */
	outb(I8259_MSTR_CTRL, I8259_EOI_KBD);

	LEAVE_CRITICAL_SECTION;
}


void kbdinit()
{
    isr_install(INTR_KBD, kbdisr);
    intr_unmask(INTR_KBD);

    /* Flush keyboard buffer */
    inb(I8255_PORT_A);
    inb(I8255_PORT_A);

    /* Issue keyboard eoi */
    outb(I8259_MSTR_CTRL, I8259_EOI_KBD);
}


void kbdflush()
{
	ENTER_CRITICAL_SECTION;
	h = t = 0;
	LEAVE_CRITICAL_SECTION;
}


int get()
{
	char            c;

	ENTER_CRITICAL_SECTION;
	while (h == t) {
		// pthread_yield();	       /* enables interrupts */
		ENTER_CRITICAL_SECTION;
	}
	c = q[h];
	h = (h + 1) % KBDQ_LENGTH;
	LEAVE_CRITICAL_SECTION;
	return (int) c;
}
