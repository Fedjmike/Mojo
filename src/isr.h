#pragma once

#include "stdint.h"

typedef enum {
    IRQ0 = 32, IRQ1, IRQ2, IRQ3, IRQ4, IRQ5, IRQ6, IRQ7,
    IRQ8, IRQ9, IRQ10, IRQ11, IRQ12, IRQ13, IRQ14,
    interruptNo = 256
} isr_tag;

typedef struct isr_args {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t intNo, errCode;
    uint32_t eip, cs, eflags, userESP, ss;
} isr_args;

typedef void (*isr_handlerFn)(isr_args);

void isr_registerHandler (isr_tag n, isr_handlerFn handler);