#include <kernel/kheap.h>
#include <kernel/control.h>
#include <kernel/arch.h>
#include <heap.h>

static bool heapInitialized = false;

// kernel_virtual_end is defined in the linker script.
extern uint32_t kernel_virtual_end;

uint32_t kheap_placement_address = (uint32_t)&kernel_virtual_end;

void* kalloc_alignment(size_t size, uint32_t* physAddr) {
	if(heapInitialized) {
		// Now you can use normal malloc
		return NULL;
	}

	 // This will eventually call malloc() on the kernel heap.
	// For now, though, we just assign memory at placement_address
	// and increment it by sz. Even when we've coded our kernel
	// heap, this will be useful for use before the heap is initialised.
	if (kheap_placement_address & 0xFFFFF000) {
		// Align the placement address;
		kheap_placement_address &= 0xFFFFF000;
		kheap_placement_address += 0x1000;
	}

	if (physAddr) {
		*physAddr = kheap_placement_address - arch_get_kernel_offset();
	}

	uint32_t tmp = kheap_placement_address;
	kheap_placement_address += size;
	
	return (void*)tmp;
}

void* kmalloc(size_t size) {
	if(heapInitialized) {
		return NULL;
	}

	uint32_t tmp = kheap_placement_address;
	kheap_placement_address += size;
	
	return (void*)tmp;
}

static size_t heap_expand(void* start_adress, size_t min_needed_size) {
	int num_pages = (min_needed_size + 0xFFF) / 0x1000;

	for(int i = 0; i < num_pages; ++i) {
		if(!arch_allocate_page(arch_get_current_page_directory(), (void*)((uint32_t)start_adress + i * 0x1000), false)) {
			return 0x1000 * i;
		}	
	}

	return num_pages * 0x1000;
}

Heap h;

void memory_init(size_t heap_size) {
	int err_code = heap_init(&h, (void*)kheap_placement_address, heap_size, heap_expand);

	if(err_code) {
		kernel_panic("Can`t create heap.");
	}

	heapInitialized = true;
}

void* malloc(size_t size) {
	return hmalloc(&h, size);
}

void kfree(void* mem) {
	hfree(&h, mem);
}