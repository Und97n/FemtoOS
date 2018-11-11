#pragma once

#include "asm_utils.h"

typedef struct {
   uint32_t present    : 1;   // Page present in memory
   uint32_t rw         : 1;   // Read-only if clear, readwrite if set
   uint32_t user       : 1;   // Supervisor level only if clear
   uint32_t accessed   : 1;   // Has the page been accessed since last refresh? 
   uint32_t dirty      : 1;   // Has the page been written to since last refresh?
   uint32_t unused     : 7;   // Amalgamation of unused and reserved bits
   uint32_t frame      : 20;  // Frame address (shifted right 12 bits)
} Page;

typedef struct {
   Page pages[1024];
} PageTable;

typedef struct {
   /**
      Array of pointers to the pagetables above, but gives their *physical*
      location, for loading into the CR3 register.
   **/
   uint32_t tablesPhysical[1024];
   /**
      Array of pointers to pagetables.
   **/
   PageTable* tables[1024];
   /**
      The physical address of tablesPhysical. This comes into play
      when we get our kernel heap allocated and the directory
      may be in a different location in virtual memory.
   **/
   uint32_t physicalAddr;
} PageDirectory;

/**
  Sets up the environment, page directories etc and
  enables paging.
**/
void initialise_paging(uint32_t memory_size);

/**
  Causes the specified page directory to be loaded into the
  CR3 register.
**/
void switch_page_directory(PageDirectory *new);

/**
  Retrieves a pointer to the page required.
  If make == 1, if the page-table in which this page should
  reside isn't created, create it!
**/
Page* get_page(uint32_t address, int make, PageDirectory* dir);

/**
  Allocate new page frame with needed virual adress.
  Returns true if needed page is successfully allocated.
**/
int allocate_page(uint32_t virtual_address, PageDirectory *dir, int userspace);

/**
  Find some free page in needed virtual address range.
  Return NULL if no free pages in needed range. If no free memory - kernel panic
**/
void* allocate_some_page(PageDirectory *dir, int userspace, uint32_t min_address, uint32_t max_address);