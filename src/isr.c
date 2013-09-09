#include "stdint.h"

#include "kprintf.h"

/**
 * Arguments given to handlers
 */
typedef struct registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t intNo, errCode;
    uint32_t eip, cs, eflags, userESP, ss;
} registers;

void isr_testHandler (registers regs) {
    kprintf("recieved interrupt: %d\n", regs.intNo);
}