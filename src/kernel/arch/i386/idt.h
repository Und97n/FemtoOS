#pragma once

/**
 * Used for data transfering from assembler interrupt handler to C handler.
 */
typedef struct {
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
} Registers;

typedef void (*Handler)(Registers*);

void idt_init();
void idt_install_interrupt_handler(int interrupt, Handler handler);

/* This installs a custom IRQ handler for the given IRQ */
void irq_install_handler(int irq, Handler handler);

