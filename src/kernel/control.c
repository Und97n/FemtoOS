#include <kernel/control.h>

#include <stdio.h>

void kernel_panic(const char* message) {
	puts("Kernel panic: ");
	puts(message);

	while(1);
}