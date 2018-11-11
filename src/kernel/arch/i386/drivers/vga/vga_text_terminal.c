#include <kernel/drivers/tty.h>

#include "vga_text_terminal.h"

#include "../../asm_utils.h"

#include <kernel/arch.h>
#include <string.h>

static void driver_init();

static void driver_cleanup();

static int driver_diagnoze(char** out_message);
static int driver_is_supported();

static void set_data(uint x, uint y, char data, uint color);

static void scroll();

static void putchar(char ch);

static void putstring(const char* s, int length);
static void puts(const char* s);

static void set_color(uint bcg, uint frg);

static void gotoxy(int x, int y);
static void getxy(int* x, int* y);

static uint get_width();
static uint get_height();

static void clear();

static void enable_cursor();
static void disable_cursor();
static void move_cursor(uint x, uint y);

static void move_cursor_cur();

const TerminalDriver vga_text_terminal_driver = {
	{
		driver_init,
		driver_cleanup,
		driver_diagnoze,
		driver_is_supported,

		"VGA text mode driver(80x25).",
		"1.1",
	},

	{
		set_data,
		scroll,
		putchar,
		putstring,
		puts,
		set_color,
		gotoxy,
		getxy,
		get_width,
		get_height,
		clear,
		enable_cursor,
		disable_cursor,
		move_cursor,
		move_cursor_cur
	},
}; 

static int width = 80;
static int height = 25;
static uint16_t* VGA_MEMORY = (uint16_t*) (0xB8000 + KERNEL_OFFSET);

static uint16_t color;

static int pos;
#define VGA_ENTRY(x) ((x) | (color << 8))

static void driver_init() {
	width = 80;
	height = 25;
	
	pos = 0;
	
	color = 0x0f;
	
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			const int index = y * width + x;
			VGA_MEMORY[index] = VGA_ENTRY(' ');
		}
	}
}

static void driver_cleanup() {
	// Just for nice look
	clear();
}

static int driver_diagnoze(char** out_message) {
	if(!driver_is_supported()) {
		if(out_message) {
			*out_message = "Error: Wrong platform(not i386).";
		}

		return VGA_TEXT_ERROR_CODE_WRONG_PLATFORM;
	}

	if(out_message) {
		*out_message = "All is normal.";
	}

	return 0;
}

static int driver_is_supported() {
	return get_architecture() == arch_i386;
}

static void set_data(uint x, uint y, char data, uint color) {
	VGA_MEMORY[x + y * width] = (data) | (color << 8);
}

static void scroll() {
	pos = width * (height - 1);
	
	for(int yy = 0; yy < height - 1; ++yy) {
		int yOffs = yy * width;
		
		for(int xx = 0; xx < width; ++xx) {
			VGA_MEMORY[xx + yOffs] = VGA_MEMORY[xx + yOffs + width];
		}
	}
		
	int offset = (height - 1) * width;
		
	for(int xx = 0; xx < width; ++xx) {
		VGA_MEMORY[xx + offset] =  VGA_ENTRY(' ');
	}
}

static inline void putchar_inline(char c) {
	int currentLine = pos / width;

	switch(c) {
	case '\0': break;
	case '\b':
		if(pos > 0) {
			--pos;
		}

		break;
	case '\n':
		pos = (currentLine + 1) * width;
		
		if (pos >= width * height) {
			scroll();
		}
		break;
	default:
		VGA_MEMORY[pos] = VGA_ENTRY(c);
	
		if (++pos >= width * height) {
			scroll();
		}
		
		break;
	}
	
	move_cursor_cur();
}

static void putchar(char c) {
	putchar_inline(c);
}

static void putstring(const char* s, int length) {
	for(int i = (!s ? 1 : 0); i < length; ++i) {
		putchar_inline(s[i]);
	}
}

static void puts(const char* s) {
	int i = 0;

	while(s[i]) {
		putchar_inline(s[i++]);
	}
}

static void set_color(uint bcg, uint frg) {
	frg &= 0xf;
	bcg &= 0xf;
	
	color = frg | (bcg << 4);
}

static void gotoxy(int x, int y) {
	if(x < 0 || y < 0 || x >= width || y >= height) {
		return;
	}

	pos = x + y * width;

	move_cursor_cur();
}

static void getxy(int* x, int* y) {
	if(x) {
		*x = pos % width;
	}

	if(y) {
		*y = pos / width;
	}
}

static uint get_width() {
	return width;
}

static uint get_height() {
	return height;
}

static void clear() {
	uint16_t empty = VGA_ENTRY(' ');

	int length = width * height;

	for(int i = 0; i < length; ++i) {
		VGA_MEMORY[i] = empty;
	}

	pos = 0;
}

static void enable_cursor() {
	uint scanlineStart = 14;
	uint scanlineEnd = 15;;

	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | (uint8_t)scanlineStart);
 
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3E0) & 0xE0) | (uint8_t)scanlineEnd);
}

static void disable_cursor() {
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

static void move_cursor(uint x, uint y) {
	uint16_t pos = y * width + x;
 
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

static void move_cursor_cur() {
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));	
}