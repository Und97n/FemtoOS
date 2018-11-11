#include "simpleCommands.h"

#include <errno.h>
#include <kernel/drivers/tty.h>
#include <stdio.h>
#include <stdlib.h>
#include <kernel/3dTest.h>
#include <kernel/shell.h>

int cls(int argc, char** argv) {
	UNUSED(argc);
	UNUSED(argv);

	tty_clear();

	return 0;
}

int echo(int argc, char** argv) {
	for(int i = 1; i < argc; ++i) {
		printf("%s\n", argv[i]);
	}

	return 0;
}

int game3D(int argc, char** argv) {
	UNUSED(argc);
	UNUSED(argv);

	test_3d();

	return 0;
}

// extern PageDirectory* current_page_directory;

// int printPages(int argc, char** argv) {
// 	UNUSED(argc);
// 	UNUSED(argv);

// 	for(int i = 0; i < 1024; ++i) {
// 		if(current_page_directory->tables[i]) {
// 			printf("Page table 0x%x:\n", i);
// 			uint32_t* pages = (uint32_t*)(current_page_directory->tables[i]->pages);

// 			for(int j = 0; j < 1024; ++j) {
// 				if(pages[j]) {
// 					printf("    Page 0x%x: 0x%x\n", j, pages[j]);
// 				}
// 			}
// 		}
// 	}

// 	return 0;
// }

int memdw(int argc, char** argv) {
	if(argc < 2) {
		printf("Not enought arguments.\n");

		return 1;
	}

	uint32_t addr = strtoul(argv[1], 0, 0);

	if(errno || addr == 0) {
		printf("Wrong address format(unsigned non-zero 32-bit integer).\n");

		return 2;
	}

	uint32_t count = 1;

	if(argc > 2) {
		count = strtoul(argv[2], 0, 0);

		if(errno || count == 0) {
			printf("Wrong elements count format(unsigned non-zero 32-bit integer).\n");

			return 3;
		}
	}

	uint32_t* ptr = (uint32_t*)addr;

	for(uint i = 0; i < count; ++i) {
		printf("%08X: %08X\n", &(ptr[i]), ptr[i]);
	}

	return 0;
}