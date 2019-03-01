#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
// #include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <kernel/drivers/tty.h>

int putchar(int ch) {
	tty_putchar(ch);
	
	return ch;
}

int puts(const char* string) {
	tty_puts(string);

	return 1;
}
