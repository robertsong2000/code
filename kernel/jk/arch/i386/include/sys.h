#ifndef __SYS_H
#define __SYS_H


#define ALIGN(V, SIZE) ((((V) + (SIZE) - 1) / SIZE) * SIZE)

#define disable __asm__ __volatile__("cli")
#define enable __asm__ __volatile__("sti");

#define ENTER_CRITICAL_SECTION disable
#define LEAVE_CRITICAL_SECTION enable

#define inb(port) \
	kioport_in_kuint8((kuint) port)

#define outb(port, data) \
	kioport_out_kuint8((kuint) port, (kuint8) data)


void            halt();
void            reboot();


#endif
