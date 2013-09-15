#include "internal.h"

#include <ctype.h>
#include <dev/vga.h>
#include <errno.h>
#include <string.h>
#include <sys.h>


#define LINES		30
#define COLS		80

#define CURSOR		'_'


extern int      screen_width, screen_height;
extern int      font_width, font_height;


static char     screen[LINES * COLS];
static char    *curs, *lastline;


void            put(char c);


void
clear()
{
    int             i;

    for (i = 0; i < LINES * COLS; i++)
	screen[i] = ' ';
    lastline = screen + (LINES - 1) * COLS;
    curs = screen;

    VGA_SET_COLOR(BLACK);
    vga_drawbox(0, 0, screen_width, screen_height);
}


static void
scroll()
{
	int             i, j, x, y;

	for (curs = screen; curs < lastline; curs++)
		*curs = *(curs + COLS);
	for (i = 0; i < COLS; i++)
		lastline[i] = ' ';

	for (i = 0; i < LINES; i++) {
		VGA_SET_COLOR(BLACK);
		vga_drawbox(0, i * font_height, screen_width, (i + 1) * font_height);

		if (i < LINES - 1) {
			VGA_SET_COLOR(WHITE);
			y = i * font_height;
			for (j = 0; j < COLS; j++) {
				x = j * font_width;
				vga_drawchar(screen[i * COLS + j], x, y);
			}
		}
	}
	curs = lastline;
}


void
put(char c)
{
	int             x, y;

	if (isprint(c)) {

		VGA_SET_COLOR(BLACK);
		x = ((curs - screen) % COLS) * font_width;
		y = ((curs - screen) / COLS) * font_height;
		vga_drawbox(x, y, x + font_width, y + font_height);

		VGA_SET_COLOR(WHITE);
		vga_drawchar(c, x, y);

		*curs = c;
		if (++curs == (screen + (LINES * COLS)))
			scroll();

	} else if (c == '\n') {

		VGA_SET_COLOR(BLACK);
		x = ((curs - screen) % COLS) * font_width;
		y = ((curs - screen) / COLS) * font_height;
		vga_drawbox(x, y, x + font_width, y + font_height);

		if (curs < lastline) {
			char           *newcurs = screen;

			while (newcurs <= curs)
				newcurs += COLS;
			curs = newcurs;
		} else
			scroll();
	}
}


int
cons_ioctl(int cmd, void *args)
{
    return ENOSYS;
}


int
cons_get(char *c)
{
    return ENOSYS;
}


int
cons_put(char c)
{
    ENTER_CRITICAL_SECTION;
    put(c);
    LEAVE_CRITICAL_SECTION;
    return 0;
}
