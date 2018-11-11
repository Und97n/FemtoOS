#pragma once
 
void gdt_init();
void gdt_set_descriptor(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);