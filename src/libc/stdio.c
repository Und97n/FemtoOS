#include <stdio.h>
#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
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

static bool print(const char* data, size_t length) {
	tty_putstring(data, length);
	return true;
}

int old_printf(const char* restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);

	int written = 0;

	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;

		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%') {
				format++;
			}
			
			size_t amount = 1;
			
			while (format[amount] && format[amount] != '%') {
				amount++;
			}
			
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			
			if (!print(format, amount)) {
				return -1;
			}
			
			format += amount;
			written += amount;
			
			continue;
		}

		const char* format_begun_at = format++;

		if (*format == 'c') {
			format++;
			char c = (char) va_arg(parameters, int /* char promotes to int */);
			if (!maxrem) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			
			if (!print(&c, 1)) {
				return -1;
			}
			
			written++;
		} else if (*format == 's') {
			format++;
			const char* str = va_arg(parameters, const char*);
			size_t len = strlen(str);
			
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			
			if (!print(str, len)) {
				return -1;
			}
			
			written += len;
		} else if (*format == 'd') {
			format++;
			int data = va_arg(parameters, int);

			if(data < 0) {
				putchar('-');
				
				data = -data;
			}
			
			// Max length = 10
			char out[10];			
			
			uint _data = data;
			
			size_t charsCount = 0;
			
			do {
				out[charsCount++] = (_data % 10) + '0';
				_data /= 10;
				
			} while(_data);
			
			for(int i = charsCount - 1; i >= 0; --i) {
				putchar(out[i]);
			}
			
			if (maxrem < charsCount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
	
			written += charsCount;
		} else if (*format == 'u') {
			format++;
			uint32_t data = va_arg(parameters, uint32_t);
			
			// Max length = 10
			char out[10];			
			
			int _data = data;
			
			size_t charsCount = 0;
			
			do {
				out[charsCount++] = (_data % 10) + '0';
				_data /= 10;
				
			} while(_data);
			
			for(int i = charsCount - 1; i >= 0; --i) {
				putchar(out[i]);
			}
			
			if (maxrem < charsCount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
	
			written += charsCount;
		} else if (*format == 'h') {
			// Hex
			
			format++;
			uint32_t data = va_arg(parameters, uint32_t);			
			
			// Max length = 8
			char out[8];			
			
			int _data = data;
			
			size_t charsCount = 0;
			
			do {
				char c = (_data % 0x10) + '0';
				
				if(c > '9') {
					c -= '9' + 1;
					c += 'a';
				}
				
				out[charsCount++] = c;
				

				_data >>= 4;
				
			} while(_data);
			
			for(int i = charsCount - 1; i >= 0; --i) {
				putchar(out[i]);
			}
			
			if (maxrem < charsCount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
	
			written += charsCount;
		} else {
			format = format_begun_at;
			size_t len = strlen(format);
			
			if (maxrem < len) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			
			if (!print(format, len)) {
				return -1;
			}
			
			written += len;
			format += len;
		}
	}

	va_end(parameters);
	return written;
}

