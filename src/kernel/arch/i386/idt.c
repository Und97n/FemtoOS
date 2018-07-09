#include "idt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/control.h>

#include "asm_utils.h"

typedef struct {
   uint16_t base_lo;             // Младшие 16 битов адреса, куда происходи переход в случае возникновения прерывания.
   uint16_t sel;                 // Переключатель сегмента ядра.
   uint8_t  always0;             // Это значение всегда должно быть нулевым.
   uint8_t  flags;               // More flags. See documentation.
   uint16_t base_hi;             // Старшие 16 битов адреса, куда происходи переход.
} __attribute__((packed)) IDT_Entry;

// Структура, описывающая указатель на массив обработчиков прерываний.
// Этот формат подходит для загрузки с помощью 'lidt'.
typedef struct {
   uint16_t limit;
   uint32_t base;                // Адрес первого элемента нашего массива idt_entry_t.
} __attribute__((packed)) IDT_Data;

extern void idt_flush();

IDT_Entry idt_entries[256];
IDT_Data idt_ptr;

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30(); 
extern void isr31(); 

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

/* This array is actually an array of function pointers. We use
*  this to handle custom IRQ handlers for a given IRQ */
Handler irq_routines[16] = {0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

Handler idt_routines[32] = {};

/* This installs a custom IRQ handler for the given IRQ */
void irq_install_handler(int irq, void (*handler)(Registers* regs)) {
    irq_routines[irq] = handler;
}

void idt_install_interrupt_handler(int interrupt, Handler handler) {
	idt_routines[interrupt] = handler;
}

/* Normally, IRQs 0 to 7 are mapped to entries 8 to 15. This
*  is a problem in protected mode, because IDT entry 8 is a
*  Double Fault! Without remapping, every time IRQ0 fires,
*  you get a Double Fault Exception, which is NOT actually
*  what's happening. We send commands to the Programmable
*  Interrupt Controller (PICs - also called the 8259's) in
*  order to make IRQ0 to 15 be remapped to IDT entries 32 to
*  47 */
void irq_remap(void) {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}

const char const* exception_messages[32] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
	"Breakpoint Exception",
	"Into Detected Overflow Exception",
	"Out of Bounds Exception",
	"Invalid Opcode Exception",
	"No Coprocessor Exception",
	"Double Fault Exception",
	"Coprocessor Segment Overrun Exception",
	"Bad TSS Exception",
	"Segment Not Present Exception",
	"Stack Fault Exception",
	"General Protection Fault Exception",
	"Page Fault Exception",
	"Unknown Interrupt Exception",
	"Coprocessor Fault Exception",
	"Alignment Check Exception (486+)",
    "Machine Check Exception (Pentium/586+)",
    "Unknow(Reserved)",
	"Unknow(Reserved)",
	"Unknow(Reserved)",
	"Unknow(Reserved)",
	"Unknow(Reserved)",
	"Unknow(Reserved)",
	"Unknow(Reserved)",
	"Unknow(Reserved)",
	"Unknow(Reserved)",
	"Unknow(Reserved)",
	"Unknow(Reserved)",
	"Unknow(Reserved)",
	"Unknow(Reserved)",
};

static void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags) {
    /* The interrupt routine's base address */
    idt_entries[num].base_lo = (base & 0xFFFF);
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

    /* The segment or 'selector' that this IDT entry will use
    *  is set here, along with any access flags */
    idt_entries[num].sel = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags = flags;
}

void idt_init() {
	idt_ptr.limit = (sizeof(IDT_Entry) * 256) - 1;
	idt_ptr.base = (uint32_t)idt_entries;
	memset(&idt_entries, 0, sizeof(IDT_Entry) * 256);
	
#define ISR_LOAD(x)	idt_set_gate(x, (uint32_t)isr ## x , 0x08, 0x8E);
	
	ISR_LOAD(0)
	ISR_LOAD(1)
	ISR_LOAD(2)
	ISR_LOAD(3)
	ISR_LOAD(4)
	ISR_LOAD(5)
	ISR_LOAD(6)
	ISR_LOAD(7)
	ISR_LOAD(8)
	ISR_LOAD(9)
	ISR_LOAD(10)
	ISR_LOAD(11)
	ISR_LOAD(12)
	ISR_LOAD(13)
	ISR_LOAD(14)
	ISR_LOAD(15)
	ISR_LOAD(16)
	ISR_LOAD(17)
	ISR_LOAD(18)
	ISR_LOAD(19)
	ISR_LOAD(20)
	ISR_LOAD(21)
	ISR_LOAD(22)
	ISR_LOAD(23)
	ISR_LOAD(24)
	ISR_LOAD(25)
	ISR_LOAD(26)
	ISR_LOAD(27)
	ISR_LOAD(28)
	ISR_LOAD(29)
	ISR_LOAD(30)
	ISR_LOAD(31)
	
#undef ISR_LOAD
	
	irq_remap();
	
#define IRQ_LOAD(x)	idt_set_gate(x + 32, (uint32_t)irq ## x , 0x08, 0x8E);
	
	IRQ_LOAD(0)
	IRQ_LOAD(1)
	IRQ_LOAD(2)
	IRQ_LOAD(3)
	IRQ_LOAD(4)
	IRQ_LOAD(5)
	IRQ_LOAD(6)
	IRQ_LOAD(7)
	IRQ_LOAD(8)
	IRQ_LOAD(9)
	IRQ_LOAD(10)
	IRQ_LOAD(11)
	IRQ_LOAD(12)
	IRQ_LOAD(13)
	IRQ_LOAD(14)
	IRQ_LOAD(15)
	
#undef IRQ_LOAD
	 
	idt_flush();
}

void isr_handler(Registers* regs) {
	uint32_t id = regs->int_no;
	
	if(idt_routines[id]) {
		idt_routines[id](regs);
	} else if(id < 32) {
		printf("Unhandled interrupt: %s(%d).\n", exception_messages[id], id, regs->eip);
		printf("Registers:\n    EIP = 0x%X\n    EAX = 0x%X,\n    EBX = 0x%X\n    ECX = 0x%X\n    EDX = 0x%X\n    ESP = 0x%X\n    EBP = 0x%X\n    ESI = 0x%X\n    EDI = 0x%X\n",
			regs->eip, regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esp, regs->ebp, regs->esi, regs->edi);
		
		kernel_panic("Unhandled interrupt.");
	}
	
} 

/* Each of the IRQ ISRs point to this function, rather than
*  the 'fault_handler' in 'isrs.c'. The IRQ Controllers need
*  to be told when you are done servicing them, so you need
*  to send them an "End of Interrupt" command (0x20). There
*  are two 8259 chips: The first exists at 0x20, the second
*  exists at 0xA0. If the second controller (an IRQ from 8 to
*  15) gets an interrupt, you need to acknowledge the
*  interrupt at BOTH controllers, otherwise, you only send
*  an EOI command to the first controller. If you don't send
*  an EOI, you won't raise any more IRQs */
void irq_handler(Registers* regs) {
    /* This is a blank function pointer */
    Handler handler;

    /* Find out if we have a custom handler to run for this
    *  IRQ, and then finally, run it */
    handler = irq_routines[regs->int_no - 32];
    
	if (handler) {
        handler(regs);
    }

    /* If the IDT entry that was invoked was greater than 40
    *  (meaning IRQ8 - 15), then we need to send an EOI to
    *  the slave controller */
    if (regs->int_no >= 40) {
        outb(0xA0, 0x20);
    }

    /* In either case, we need to send an EOI to the master
    *  interrupt controller too */
    outb(0x20, 0x20);
}