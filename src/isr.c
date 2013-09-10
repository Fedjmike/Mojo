#include "isr.h"

#include "stdint.h"
#include "hwio.h"
#include "kprintf.h"

isr_handlerFn handlers[interruptNo];

static void isr_dispatch (isr_tag n, isr_args args);

void isr_registerHandler (isr_tag n, isr_handlerFn handler) {
    if (n < interruptNo)
        handlers[n] = handler;
    
    else
        kprintf("ISR ERROR: attempted to register interrupt #%d\n", n);
}

static void isr_dispatch (isr_tag n, isr_args args) {
    if (handlers[n])
        handlers[n](args);
        
    else
        kprintf("ISR ERROR: no handler registered for interrupt #%d\n", n);
}

void isr_handler (isr_args args) {
    isr_dispatch(args.intNo, args);
}

void irq_handler (isr_args args) {
    /*Signal to the PICs (slave as well for the latter 8 IRQs) that it is handled*/
    
    if (args.intNo >= IRQ8)
        hwio_out8(0xA0, 0x20);
        
    hwio_out8(0x20, 0x20);

    /* */
    isr_dispatch(args.intNo, args);
}