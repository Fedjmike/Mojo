/*See gdt.c, it's pretty similar*/

#include "idt.h"

#include "std.h"
#include "stdint.h"
#include "stdlib.h"

typedef struct idt_entry {
    /*base is the address of the interrupt handler*/
    uint16_t base_lo;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_hi;
 } __attribute__((packed)) idt_entry;

typedef struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr;

idt_entry idt_entries[256];
idt_ptr idt;

static idt_entry idt_entry_init (uint32_t base, uint16_t sel, uint8_t flags, bool usermode);

extern void idt_flush (uint32_t idt);

extern void isr0 (), isr1 (), isr2 (), isr3 (), isr4 (), isr5 (), isr6 (), isr7 (),
            isr8 (), isr9 (), isr10 (), isr11 (), isr12 (), isr13 (), isr14 (), isr15 (),
            isr16 (), isr17 (), isr18 (), isr19 (), isr20 (), isr21 (), isr22 (), isr23 (),
            isr24 (), isr25 (), isr26 (), isr27 (), isr28 (), isr29 (), isr30 (), isr31 ();

void idt_init () {
    memset(&idt_entries, 0, sizeof(idt_entries));

    void (*isrs[])(void) = {isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7,
                            isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15,
                            isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23,
                            isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31};

    for (int i = 0; i < 32; i++)
        idt_entries[i] = idt_entry_init((uint32_t) isrs[i], 0x08, 0x8E, false);

    idt.limit = sizeof(idt_entries) - 1;
    idt.base = (uint32_t) &idt_entries;
    idt_flush((uint32_t) &idt);
}

static idt_entry idt_entry_init (uint32_t base, uint16_t sel, uint8_t flags, bool usermode) {
    idt_entry entry;
    entry.base_lo = base & 0xFFFF;
    entry.base_hi = (base >> 16) & 0xFFFF;

    entry.sel = sel;
    entry.always0 = 0;
    entry.flags = flags | (usermode ? 0x60 : 0);
    return entry;
 }