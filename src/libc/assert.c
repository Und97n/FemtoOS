#include <assert.h>

#include <stdio.h>

void __assertPrintUtil(char* string, int line) {
	printf(string, line);

	while(1);
}