#include <kernel/arch.h>
#include "gdt.h"
#include "idt.h"
#include "paging.h"

#include "asm_utils.h"


int get_architecture() {
	return arch_i386;
}

void arch_init_memory(uint32_t memory_size) {
	UNUSED(memory_size);

	// Of course.
	gdt_set_descriptor(0, 0, 0, 0, 0);

	/* The second entry is our Code Segment. The base address
	*  is 0, the limit is 4GBytes, it uses 4KByte granularity,
	*  uses 32-bit opcodes, and is a Code Segment descriptor.
	*  Please check the table above in the tutorial in order
	*  to see exactly what each value means. */
	gdt_set_descriptor(1, 0, 0x1FFFFFFF, 0x9A, 0xCF);

	/* The third entry is our Data Segment. It's EXACTLY the
	*  same as our code segment, but the descriptor type in
	*  this entry's access byte says it's a Data Segment. */
	gdt_set_descriptor(2, 0, 0x1FFFFFFF, 0x92, 0xCF);

	gdt_set_descriptor(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
	gdt_set_descriptor(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment
	
	gdt_init();
}

void arch_init_interrupts() {
	idt_init();
}

extern PageDirectory* current_page_directory;

void* arch_get_current_page_directory() {
	return current_page_directory;
}

void arch_init_paging(uint32_t memory_size) {
	initialise_paging(memory_size);
}

bool arch_switch_page_directory(void* new_page_directory) {
	switch_page_directory((PageDirectory*)new_page_directory);

	return true;
}

bool arch_allocate_page(void* page_directory, void* virtual_address, bool userspace) {
	return allocate_page((uint32_t)virtual_address, (PageDirectory*)page_directory, userspace);
}

void* arch_allocate_some_page(void* page_directory, bool userspace, void* min_address, void* max_address) {
	return allocate_some_page((PageDirectory*)page_directory, userspace, (uint32_t)min_address, (uint32_t)max_address);
}

uint32_t arch_get_kernel_offset() {
	return KERNEL_OFFSET;
}
