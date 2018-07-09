#pragma once

enum Architectures {
	arch_i386 = 1,
};

/**
	Returns current architecture from Architectures enum.
**/
int get_architecture();

// First action
void arch_init_memory(uint32_t memory_size);

// Second action
void arch_init_interrupts();

// Third action
void arch_init_paging(uint32_t memory_size);

/**
	Returns NULL if paging is not initialized.
**/
void* arch_get_current_page_directory();

/**
  Causes the specified page directory to be used.
  Return true if replacing is ok.
**/
bool arch_switch_page_directory(void* new_page_directory);

/**
  Allocate new page frame with needed virual adress.
  Returns true if needed page is successfully allocated.
**/
bool arch_allocate_page(void* page_directory, void* virtual_address, bool userspace);

/**
  Find some free page in needed virtual address range.
  Return NULL if no free pages in needed range. If no free memory - kernel panic
**/
void* arch_allocate_some_page(void* page_directory, bool userspace, void* min_address, void* max_address);

/**
	Get virtual memory position of kernel.
**/
uint32_t arch_get_kernel_offset();