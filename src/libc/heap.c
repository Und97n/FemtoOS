#include <heap.h>

#include <stdio.h>

#define GET_META(ptr) ((Metadata*)(ptr))
#define NEXT_BLOCK(ptr) ((Metadata*)(((uint32_t)(ptr)) + ((GET_META(ptr)->size) << 3)))
#define PREVIOUS_BLOCK(ptr) ((Metadata*)(((uint32_t)(ptr)) - ((GET_META(ptr)->previous_block_size) << 3)))

#define IS_CORRECT_BLOCK(ptr, heap) ((void*)(ptr) < ((heap)->start + (heap)->current_size) && (void*)(ptr) >= (heap)->start)

#define ALIGNMENT 8

#define MIN_BLOCK_SIZE (ALIGNMENT)

#define ALIGN_TO_LESS(ptr) (((uint32_t)(ptr)) & (~(ALIGNMENT - 1)))
#define ALIGN_TO_BIG(ptr) ((((uint32_t)(ptr)) + ALIGNMENT - 1) & (~(ALIGNMENT - 1)))

int heap_init(Heap* heap, void* _heap_start, size_t max_size, Heap_expand_fnc heap_expand) {
	if(max_size <= ALIGNMENT * 2) {
		return 2;
	}

	void* heap_start = (void*)ALIGN_TO_BIG(_heap_start);
	
	size_t offset = heap_start - _heap_start;

	heap->expand = heap_expand;
	heap->start = heap_start;
	heap->addr_max = heap_start + max_size - offset;

	// By default our heap have minimal size.
	size_t initial_size = heap_expand(heap_start, sizeof(Metadata));

	if(initial_size == 0) {
		return 1;
	}

	heap->current_size = initial_size;
	Metadata* m = GET_META(heap_start);
	m->size = initial_size >> 3;
	m->additional_data1 = 0;
	m->additional_data2 = 0;
	m->previous_block_size = 0;
	m->free = true;

	return 0;
}

static bool optimize_block(Metadata* ptr, Heap* heap) {
	if(ptr->free) {
		Metadata* next = NEXT_BLOCK(ptr);

		if(IS_CORRECT_BLOCK(next, heap) && next->free) {
			// Concat blocks
			ptr->size += next->size;

			Metadata* nextnext = NEXT_BLOCK(next);

			// If we have some another block forward, update it data
			if(IS_CORRECT_BLOCK(nextnext, heap)) {
				nextnext->previous_block_size = ptr->size;
			}

			return true;
		}
	}

	return false;
}

static void* direct_malloc(Metadata* free_block, size_t size, Heap* heap) {
	size_t s = (free_block->size << 3); 

	if(s < size) {
		return NULL;
	}

	// If our block is too big
	if((s - size) > MIN_BLOCK_SIZE) {
		free_block->size = size >> 3;
	
		// Add a new Block!
		Metadata* next = NEXT_BLOCK(free_block);

		next->size = (s - size) >> 3;
		next->additional_data1 = 0;
		next->additional_data2 = 0;
		next->previous_block_size = free_block->size;
		next->free = true;

		Metadata* nextnext = NEXT_BLOCK(next);

		// If we have some another block forward, update it data
		if(IS_CORRECT_BLOCK(nextnext, heap)) {
			nextnext->previous_block_size = next->size;
		}
		
		optimize_block(next, heap);
	}

	free_block->free = false;

	return (void*)(((uint32_t)free_block) + sizeof(Metadata));
}

void* hmalloc(Heap* heap, size_t size) {
	size += sizeof(Metadata);

	size = ALIGN_TO_BIG(size);

	size = size < MIN_BLOCK_SIZE ? MIN_BLOCK_SIZE : size;

	Metadata* current = GET_META(heap->start), *prev = NULL;

	size_t ssize = size >> 3;

	while(IS_CORRECT_BLOCK(current, heap) && (!(current->free) || current->size < ssize)) {
		prev = current;
		current = NEXT_BLOCK(prev);

	}
	
	// If we  found some free block 
	if(IS_CORRECT_BLOCK(current, heap)) {
		return direct_malloc(current, size, heap);
	} else {
		uint avalible_expansion = (uint)(heap->addr_max - heap->current_size);
		uint max_size = (uint)(heap->addr_max - heap->start);

		// Try to expand memory.
		if(avalible_expansion >= size) {
			size_t additional_memory = heap->expand(heap->current_size + heap->start, size);

			heap->current_size += additional_memory;

			// If expand function is too generous
			if(heap->current_size >= max_size) {
				heap->current_size = max_size;
			}

			current->size = additional_memory;
			current->free = true;
			current->additional_data1 = 0;
			current->additional_data2 = 0;
			
			if(prev) {
				current->previous_block_size = prev->size;
			} else {
				current->previous_block_size = 0;
			}

			if(prev) {
				if(optimize_block(prev, heap)) {
					return direct_malloc(prev, size, heap);
				}
			}

			return direct_malloc(current, size, heap);
		} else {
			// Heap is over!
			return NULL;
		}
	}
}

void hfree(Heap* heap, void* ptr) {
	Metadata* m = GET_META(ptr - sizeof(Metadata));

	if(!IS_CORRECT_BLOCK(m, heap)) {
		return;
	}

	if(m->free) {
		return;
	}

	m->free = true;

	optimize_block(m, heap);

	Metadata* prev = PREVIOUS_BLOCK(m);

	if(IS_CORRECT_BLOCK(prev, heap)) {
		optimize_block(prev, heap);
	}
}

void print_heap_struct(Heap* heap) {
	Metadata* m = GET_META(heap->start);

	bool run = IS_CORRECT_BLOCK(m, heap);

	while(run) {
		printf("(%d,%d,%c)", m->previous_block_size, m->size, m->free ? '.': '#');
		m = NEXT_BLOCK(m);

		run = IS_CORRECT_BLOCK(m, heap);

		if(run) {
			printf(" ");
		}
	}

	puts("\n");
}