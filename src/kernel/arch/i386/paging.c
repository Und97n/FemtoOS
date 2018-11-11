#include "paging.h"
#include <kernel/kheap.h>
#include <kernel/control.h>
#include "idt.h"

#include "asm_utils.h"

#include <string.h>
#include <stdio.h>

// The kernel's page directory
PageDirectory* kernel_directory = NULL;

// The current page directory;
PageDirectory* current_page_directory = NULL;

uint32_t *frames;
uint32_t nframes;

// End of kernel non-heap memory
extern uint32_t kheap_placement_address;

// Defined in kheap.c
extern uint32_t placement_address;

// Macros used in the bitset algorithms.
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

// Static function to set a bit in the frames bitset
static void set_frame(uint32_t frame_addr) {
	uint32_t frame = frame_addr/0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] |= (0x1 << off);
}

// Static function to clear a bit in the frames bitset
static void clear_frame(uint32_t frame_addr) {
	uint32_t frame = frame_addr/0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	frames[idx] &= ~(0x1 << off);
}

// Static function to test if a bit is set.
static uint32_t test_frame(uint32_t frame_addr) {
	uint32_t frame = frame_addr/0x1000;
	uint32_t idx = INDEX_FROM_BIT(frame);
	uint32_t off = OFFSET_FROM_BIT(frame);
	return (frames[idx] & (0x1 << off));
}

// Static function to find the first free frame.
static uint32_t first_frame() {
	for (uint i = 0; i < INDEX_FROM_BIT(nframes); ++i) {
		// nothing free, exit early.
		if (frames[i] != 0xFFFFFFFF) {

			// at least one bit is free here.
			for (uint j = 0; j < 32; j++) {
				uint32_t toTest = 0x1 << j;
				
				if ( !(frames[i]&toTest) ) {
					return i*4*8+j;
				}
			}
		}
	}

	return (uint32_t)(-1);
}

/**
	Allocate some page frame and return it address.
**/
static uint32_t alloc_first_frame() {
	uint32_t idx = first_frame();

	if (idx == (uint32_t) -1) {
		kernel_panic("No free page frames.");
	}

	set_frame(idx * 0x1000);

	return idx * 0x1000;
}

// Function to deallocate a frame.
void free_frame(Page *page) {
	uint32_t frame;
	if (!(frame=page->frame)) {
		return;
	} else {
		clear_frame(frame);
		page->frame = 0x0;
	}
}

// Function to allocate a frame.
void alloc_some_frame(Page *page, int is_kernel, int is_writeable) {
	uint32_t idx = first_frame();

	if (idx == (uint32_t) -1) {
		kernel_panic("No free page frames.");
	}

	set_frame(idx * 0x1000);

	page->present = 1;
	page->rw = (is_writeable)?1:0;
	page->user = (is_kernel)?0:1;
	page->frame = idx;
	page->unused = 0;
}

// Function to allocate a frame.
// returns false and do nothing if frame is allready allocated
bool alloc_frame(Page *page, int is_kernel, int is_writeable, uint32_t physicalID) {
	if(test_frame(physicalID * 0x1000)) {
		return false;
	}

	set_frame(physicalID * 0x1000);

	page->present = 1;
	page->rw = (is_writeable) ? 1 : 0;
	page->user = (is_kernel) ? 0 : 1;
	page->frame = physicalID;
	page->unused = 0;

	return true;
}

// // Function to deallocate a frame.
// void free_frame(Page *page) {
// 	uint32_t frame;

// 	if (!(frame=page->frame)) {
// 		return;
// 	} else {
// 		clear_frame(frame);
// 		page->frame = 0x0;
// 	}
// }

static void page_fault(Registers* regs);

void initialise_paging(uint32_t memory_size) {
	// Before we do some trics, we must register our page fault handler.
	idt_install_interrupt_handler(14, page_fault);

	// The size of physical memory.
	uint32_t mem_end_page = memory_size;
	nframes = mem_end_page / 0x1000;
	frames = (uint32_t*)kalloc_alignment(INDEX_FROM_BIT(nframes), NULL);

	uint32_t kernel_dir_phys_addr = 0;
	
	// Let's make a page directory.
	kernel_directory = (PageDirectory*)kalloc_alignment(sizeof(PageDirectory), &kernel_dir_phys_addr);
	

	for(int i = 0; i < 1024; ++i) {
		kernel_directory->tables[i] = NULL;
		kernel_directory->tablesPhysical[i] = (uint32_t)NULL;
	}

	kernel_directory->physicalAddr = kernel_dir_phys_addr;

	//current_page_directory = kernel_directory;

	// We setup paging for all kernel.
	uint32_t addr = KERNEL_OFFSET;

	while(addr < kheap_placement_address) {
		// Kernel code is readable but not writeable from userspace.
		alloc_frame(get_page(addr, true, kernel_directory), false, false, (addr - KERNEL_OFFSET) / 0x1000);

		addr += 0x1000;
	}

	// Now, enable paging!
	printf("KD: 0x%x\n", (&kernel_directory->tablesPhysical));
	switch_page_directory(kernel_directory);
}

void switch_page_directory(PageDirectory *dir) {
	current_page_directory = dir;

	asm volatile("mov %0, %%cr3\n\rinvlpg (%0)":: "r"(dir->physicalAddr));
}

typedef union {
	Page page;
	uint32_t data;
} Page_data;

Page* get_page(uint32_t address, int make, PageDirectory* dir) {
	// Turn the address into an index.
	address /= 0x1000;
	// Find the page table containing this address.
	uint32_t table_idx = address / 1024;

	// If this table is already assigned
	if (dir->tables[table_idx]) {
		return &dir->tables[table_idx]->pages[address % 1024];
	} else if(make) {
		uint32_t tmp;

		dir->tables[table_idx] = (PageTable*)kalloc_alignment(sizeof(PageTable), &tmp);

		Page_data zeroPage;
		zeroPage.data = 0;

		for(int i = 0; i < 1024; ++i) {
			dir->tables[table_idx]->pages[i] = zeroPage.page;		
		}

		dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.

		Page* ret = &(dir->tables[table_idx]->pages[address%1024]);

		return ret;
	} else {
		return 0;
	}
}

int allocate_page(uint32_t virtual_address, PageDirectory *dir, int userspace) {
	Page* page = get_page(virtual_address, true, dir);

	uint32_t val = *((uint32_t*)(page));

	if(val) {
		// This page is allready used and we need to find some free place.
		return false;
	} else {
		uint32_t physical_address = alloc_first_frame();
		
		page->present = true;
		page->rw = true;
		page->user = (userspace) ? true : false;
		page->frame = physical_address / 0x1000;
		page->unused = 0;

		return true;
	}
}


void* allocate_some_page(PageDirectory *dir, int userspace, uint32_t min_address, uint32_t max_address) {
	// If address is not aligned to 4KiB
	if(min_address & 0xFFF) {
		min_address += 0x1000;
		min_address &= ~0xFFF;
	}
	
	if(max_address & 0xFFF) {
		max_address &= ~0xFFF;
	}

	if(min_address > max_address) {
		return NULL;
	}

	uint32_t address = min_address;

	while(address <= max_address) {
		Page* p = get_page(address, true, dir);

		if((*((uint32_t*)(p)))) {
			// Is busy.

			address += 0x1000;
		} else {
			if(allocate_page(address, dir, userspace)) {
				return (void*)address;
			} else {
				// Continue
				address += 0x1000;
			}
		}
	}

	// No free pages!
	return NULL;
}

static void page_fault(Registers* regs) {
	// A page fault has occurred.
	// The faulting address is stored in the CR2 register.
	uint32_t faulting_address;
	asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
	
	// The error code gives us details of what happened.
	int present	 = !(regs->err_code & 0x1); // Page not present
	int rw = regs->err_code & 0x2;			 // Write operation?
	int us = regs->err_code & 0x4;			 // Processor was in user-mode?
	int reserved = regs->err_code & 0x8;	 // Overwritten CPU-reserved bits of page entry?
	int id = regs->err_code & 0x10;			// Caused by an instruction fetch?

	UNUSED(id);

	// Output an error message.
	puts("\nPage fault! ( ");
	
	if (present) {
		puts("present ");
	}

	if (rw) {
		puts("read-only ");
	}
	
	if (us) {
		puts("user-mode ");
	}
	
	if (reserved) {
		puts("reserved ");
	}
	
	printf(") at 0x%X\n", faulting_address);
	kernel_panic("Page fault.");
}
