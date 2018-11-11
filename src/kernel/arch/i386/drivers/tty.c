#include <kernel/drivers/tty.h>
#include <kernel/drivers/keyboard.h>

#include "vga/vga_text_terminal.h"

#define TERMINAL_CALLBACK(fnc_name) if(has_driver) return current_driver.functions.fnc_name

static bool has_driver;
static TerminalDriver current_driver;

bool tty_install_default_driver() {
	vga_text_terminal_driver.general_data.init();
	return tty_set_driver(vga_text_terminal_driver);
}

bool tty_set_driver(TerminalDriver driver) {
	if(!driver.general_data.diagnoze(NULL)) {
		current_driver = driver;
		has_driver = true;

		return true;
	}

	return false;
}

bool tty_get_current_driver(TerminalDriver* driver) {
	if(has_driver) {
		if(driver) {
			*driver = current_driver;
		}

		return true;
	} else {
		return false;
	}
}

void tty_set_data(uint x, uint y, char data, uint color) {
	if(has_driver) {
		current_driver.functions.tty_set_data(x, y, data, color);
	} else {

	}
}

void tty_scroll() {
	if(has_driver) {
		current_driver.functions.tty_scroll();
	} else {

	}
}

void tty_putchar(char ch) {
	if(has_driver) {
		current_driver.functions.tty_putchar(ch);
	} else {

	}
}

void tty_putstring(const char* s, int length) {
	if(has_driver) {
		current_driver.functions.tty_putstring(s, length);
	} else {

	}
}

void tty_puts(const char* s) {
	if(has_driver) {
		current_driver.functions.tty_puts(s);
	} else {

	}
}

void tty_set_color(uint bcg, uint frg) {
	if(has_driver) {
		current_driver.functions.tty_set_color(bcg, frg);
	} else {

	}
}

void tty_gotoxy(int x, int y) {
	if(has_driver) {
		current_driver.functions.tty_gotoxy(x, y);
	} else {

	}
}

void tty_getxy(int* x, int* y) {
	if(has_driver) {
		current_driver.functions.tty_getxy(x, y);
	} else {
		
	}
}

uint tty_get_width() {
	if(has_driver) {
		return current_driver.functions.tty_get_width();
	} else {
		return 0;
	}
}

uint tty_get_height() {
	if(has_driver) {
		return current_driver.functions.tty_get_height();
	} else {
		return 0;
	}
}

void tty_clear() {
	if(has_driver) {
		current_driver.functions.tty_clear();
	} else {

	}
}

void tty_enable_cursor() {
	if(has_driver) {
		current_driver.functions.tty_enable_cursor();
	} else {

	}
}

void tty_disable_cursor() {
	if(has_driver) {
		current_driver.functions.tty_disable_cursor();
	} else {

	}
}

void tty_move_cursor(uint x, uint y) {
	if(has_driver) {
		current_driver.functions.tty_move_cursor(x, y);
	} else {

	}
}

void tty_move_cursor_cur() {
	if(has_driver) {
		current_driver.functions.tty_move_cursor_cur();
	} else {

	}
}