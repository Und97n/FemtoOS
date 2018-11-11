typedef size_t (*Heap_expand_fnc)(void* start_adress, size_t min_needed_size);

typedef struct {
	Heap_expand_fnc expand;
	
	void* start;
	void* addr_max;
	size_t current_size;
} Heap;

typedef struct {
	uint32_t free : 1;
	uint32_t additional_data1 : 2;
	uint32_t size : 29;
	uint32_t additional_data2 : 3;
	uint32_t previous_block_size : 29;
} Metadata;

/**
	Create a new heap with start address, max size and some possible expand fnc.
	Returns 0 if all is normal, 1 if some problems with expand fnc, 2 if max_size is too small.
**/
int heap_init(Heap* heap, void* heap_start, size_t max_size, Heap_expand_fnc heap_expand);

void* hmalloc(Heap* heap, size_t size);
void hfree(Heap* heap, void* ptr);

void print_heap_struct(Heap* h);
