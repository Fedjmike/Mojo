#include "timer.h"

#include "hwio.h"
#include "kprintf.h"
#include "isr.h"

uint32_t tick = 0;

static void timer_handler (isr_args args) {
    (void) args;
    if (tick++ % 10 == 0)
        kprintf("Tick: %d\n", tick);
}

void timer_init (uint32_t freq) {
    isr_registerHandler(IRQ0, &timer_handler);
    
    uint32_t divisor = 1193180/freq;
    uint8_t low = (uint8_t) (divisor & 0xFF);
    uint8_t high = (uint8_t) ((divisor >> 8) & 0xFF);
    
    hwio_out8(0x43, 0x36);
    hwio_out8(0x40, low);
    hwio_out8(0x40, high);
}