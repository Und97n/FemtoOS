#include <stdio.h>

#include <kernel/drivers/tty.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/timer.h>

#include <kernel/shell.h>

#include <kernel/kheap.h>
#include <kernel/arch.h>
#include <kernel/multiboot.h>

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif

extern uint32_t kheap_placement_address;

void kernel_main(Multiboot_Header* header) {
	// Temporally 
	uint32_t memory_size = 128 * 1024 * 1024;

	arch_init_memory(memory_size);
	arch_init_interrupts();
	arch_init_paging(memory_size);

	timer_init();

	if(!tty_install_default_driver()) {
		// No display there
		return;
	}

	if(!keyboard_install_default_driver()) {
		puts("No keyboard avalible.");
	}

	if(!timer_init()) {
		puts("No timer avalible.");
	}

	// if(header->mods_addr) {
	// 	uint32_t initrd_location = *((uint32_t*)(header->mods_addr + KERNEL_OFFSET)) + KERNEL_OFFSET;

	// 	uint32_t initrd_end = *(uint32_t*)(header->mods_addr + 4 + KERNEL_OFFSET) + KERNEL_OFFSET;
	// 	// Don't trample our module with placement accesses, please!
 //   		kheap_placement_address = initrd_end;

	// 	// Initialise the initial ramdisk, and set it as the filesystem root.
	// 	fs_root = initialise_initrd(initrd_location);

	// 	// list the contents of /
	// 	int i = 0;
	// 	Dirent* node = 0;
	
	// 	int level = 0;

	// 	while ((node = readdir_fs(fs_root, i)) != 0) {
	// 		FS_Node* fsnode = finddir_fs(fs_root, node->name);
			
	// 		// Hierarhy!
	// 		for(int i = 0; i < level; ++i) {
	// 			puts(" ");
	// 		}

	// 		if ((fsnode->flags&0x7) == FS_DIRECTORY) {
	// 			printf("%s: \n", node->name);

	// 			++level;
	// 		} else {				
	// 			printf("%s\n", node->name);
	// 		}

	// 		i++;
	// 	}
	// }

	memory_init(1024 * 1024 * 2);

	// uint32_t* tests[10];

	// for(int i = 1; i <= 10; ++i) {
	// 	int size = i * 4;

	// 	tests[i] = malloc(size * sizeof(uint32_t));
		

	// 	for(int j = 0; j < size; ++j) {
	// 		tests[i][j] = j + i * 100;
	// 	}
	// }

	// for(int i = 1; i <= 10; i += 2) {
	// 	int size = i * 4;

	// 	kfree(tests[i]);
	// 	//kfree(0);
	// }

	// kfree(0);

	// uint32_t* p = (uint32_t*)malloc(20 * sizeof(uint32_t));

	// kfree(0);

	// for(int i = 0; i < 20; ++i) {
	// 	printf("%d ", p[i]);
	// }

	enter_shell();
}