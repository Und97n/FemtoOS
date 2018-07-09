#pragma once

#include <kernel/driver.h>

typedef struct {
	void (*tty_set_data)(uint x, uint y, char data, uint color);

	void (*tty_scroll)();

	void (*tty_putchar)(char ch);

	void (*tty_putstring)(const char* s, int length);
	void (*tty_puts)(const char* s);

	void (*tty_set_color)(uint bcg, uint frg);

	void (*tty_gotoxy)(int x, int y);
	void (*tty_getxy)(int* x, int* y);

	uint (*tty_get_width)();
	uint (*tty_get_height)();

	void (*tty_clear)();

	void (*tty_enable_cursor)();
	void (*tty_disable_cursor)();
	void (*tty_move_cursor)(uint x, uint y);
	void (*tty_move_cursor_cur)();
} TerminalDriverFunctions;

typedef struct {
	GeneralDriverData general_data;
	TerminalDriverFunctions functions;
} TerminalDriver;

enum Color {
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};

/**
	Try to install simplest driver for current system.
	If succes - return true.
**/
bool tty_install_default_driver();

/**
	Return true if driver is successfully setted.
	If some problems - return false.
**/
bool tty_set_driver(TerminalDriver driver);

/**
	Try to recieve current terminal driver.
	Return true if it is success. If no terminal - return false.
	If driver == NULL, no assignment.
**/
bool tty_get_current_driver(TerminalDriver* driver);

void tty_set_data(uint x, uint y, char data, uint color);

void tty_scroll();

void tty_putchar(char ch);

void tty_putstring(const char* s, int length);
void tty_puts(const char* s);

void tty_set_color(uint bcg, uint frg);

void tty_gotoxy(int x, int y);
void tty_getxy(int* x, int* y);

uint tty_get_width();
uint tty_get_height();

void tty_clear();

void tty_enable_cursor();
void tty_disable_cursor();
void tty_move_cursor(uint x, uint y);

/**
 * Move crusor to the current position on the screen.
 */
void tty_move_cursor_cur();