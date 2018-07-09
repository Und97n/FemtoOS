#pragma once

/**
   Simple pseudo-heap implementation for allocating alignment data. Alignment is 0x1000(one page).
   Returns NULL after main heap initialization.
**/
void* kalloc_alignment(size_t size, uint32_t* physAddr);

/**
   Simple pseudo-heap implementation for allocating data. No alignment.
   Returns NULL after main heap initialization.
**/
void* kmalloc(size_t size);

void memory_init(size_t heap_size);
void* malloc(size_t size);
void kfree(void* mem);