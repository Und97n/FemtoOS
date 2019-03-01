#include <stdio.h>

#include <kernel/drivers/tty.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/timer.h>

#include <kernel/shell.h>

#include <kernel/kheap.h>
#include <kernel/arch.h>
#include <kernel/multiboot.h>

extern uint32_t kheap_placement_address;

#if defined(__cplusplus)
extern "C" /* Use C linkage for kernel_main. */
#endif
void kernel_main(Multiboot_Header* header) {
	// Temporally 
	uint32_t memory_size = 128 * 1024 * 1024;

	if(!tty_install_default_driver()) {
		// No display there
		return;
	}
	
	arch_init_memory(memory_size);
	arch_init_interrupts();

	timer_init();

	arch_init_paging(memory_size);

	if(!keyboard_install_default_driver()) {
		puts("No keyboard avalible.");
	}

	if(!timer_init()) {
		puts("No timer avalible.");
	}

	memory_init(1024 * 1024 * 2);

	enter_shell();
}