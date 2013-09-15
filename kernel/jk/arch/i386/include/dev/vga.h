#ifndef __VGA_H
#define __VGA_H


#define GRAPH_BASE 0xa0000

#define BLACK			0
#define BLUE			1
#define GREEN			2
#define LIGHT_BLUE		3
#define RED			4
#define VIOLET			5
#define BROWN			6
#define GRAY			7
#define DARK_GRAY		8
#define BRIGHT_BLUE		9
#define BRIGHT_GREEN		10
#define BRIGHT_LIGHT_BLUE	11
#define BRIGHT_RED		12
#define BRIGHT_VIOLET		13
#define YELLOW			14
#define WHITE			15

/* VGA index register ports */
#define CRT_IM			0x3b4  /* CRT index - mono */
#define CRT_IC			0x3d4  /* CRT index - color */
#define ATT_IW			0x3c0  /* Attr index & data write reg */
#define GRA_I			0x3ce  /* Graphics index */
#define SEQ_I			0x3c4  /* Sequencer index */
#define PEL_IW			0x3c8  /* PEL write index */
#define PEL_IR			0x3c7  /* PEL read index */

/* VGA data register ports */
#define CRT_DM			0x3b5  /* CRT data reg - mono */
#define CRT_DC			0x3d5  /* CRT data reg - color */
#define ATT_R			0x3c1  /* Attr data read reg */
#define GRA_D			0x3cf  /* Graphics data reg */
#define SEQ_D			0x3c5  /* Sequencer data reg */
#define MIS_R			0x3cc  /* Misc output read reg */
#define MIS_W			0x3c2  /* Misc output write reg */
#define IS1_RM			0x3ba  /* Input status reg 1 - mono */
#define IS1_RC			0x3da  /* Input status reg 1 - color */
#define PEL_D			0x3c9  /* PEL data reg */


#define VGA_SCREEN_OFF							\
    outb(SEQ_I, 1);							\
    outb(SEQ_D, inb(SEQ_D) | 0x20)

#define VGA_SCREEN_ON							\
    outb(SEQ_I, 1);							\
    outb(SEQ_D, inb(SEQ_D) | 0xdf)

#define VGA_SET_COLOR(COLOR)						\
    outb(GRA_I, 0);							\
    outb(GRA_D, (COLOR) % 16)


void            vga_init();
void            vga_font_init();
void            vga_clear(int color);
void            vga_drawbox(int x1, int y1, int x2, int y2);
void            vga_drawhline(int x1, int x2, int y);
void            vga_drawline(int x1, int y1, int x2, int y2);
void            vga_drawpixel(int x, int y);
void            vga_drawxpm(char **xpm, int x, int y);
void            vga_drawchar(char c, int x, int y);


#endif
