#include <kernel/drivers/timer.h>

#include "../idt.h"

#include "../asm_utils.h"

#define MAX_FREQUENCY 1193180.0

static uint32_t volatile ticks = 0;

static double frequency = 0;

/* Handles the timer. In this case, it's very simple: We
*  increment the 'timer_ticks' variable every time the
*  timer fires. By default, the timer fires 18.222 times
*  per second. Why 18.222Hz? Some engineer at IBM must've
*  been smoking something funky */
static void timer_handler(Registers* regs) {
	UNUSED(regs);

    ++ticks;
}

bool timer_init() {
	// uint32_t divisor = 1000;
	// frequency = MAX_FREQUENCY / divisor;
	
	// outb(0x43, 0x36);             /* Set our command byte 0x36 */
	// outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
	// outb(0x40, divisor >> 8); 
	
	// irq_install_handler(0, timer_handler);

	return true;
}

void timer_wait(double seconds) {
	uint64_t newTicks = seconds * frequency + ticks;
	
	while(newTicks > ticks);
}

double timer_get_time() {
	return ticks / frequency;
}
