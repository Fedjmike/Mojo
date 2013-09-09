#include "gdt.h"

#include "stdint.h"

/**
 * A segment
 */
typedef struct gdt_entry {
    /*base and limit define the extent of the segment*/
    uint16_t limit_low, base_low;
    uint8_t base_middle;
    uint8_t access, granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry;
 
/**
 * Describes where to find the GDT
 */
typedef struct gdt_ptr {
    /*base and limit define the extent of the table*/
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) gdt_ptr;

gdt_entry gdt_entries[5];
gdt_ptr gdt;

static gdt_entry gdt_entry_init (uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

/**
 * Sets the new GDT
 * Found in gdt.s
 */
extern void gdt_flush (uint32_t gdt);

void gdt_init () {
    gdt_entries[0] = gdt_entry_init(0, 0, 0, 0);
    gdt_entries[1] = gdt_entry_init(0, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt_entries[2] = gdt_entry_init(0, 0xFFFFFFFF, 0x92, 0xCF);
    gdt_entries[3] = gdt_entry_init(0, 0xFFFFFFFF, 0xFA, 0xCF);
    gdt_entries[4] = gdt_entry_init(0, 0xFFFFFFFF, 0xF2, 0xCF);

    gdt.limit = sizeof(gdt_entries) - 1;
    gdt.base = (uint32_t) &gdt_entries;
    gdt_flush((uint32_t) &gdt);
}

static gdt_entry gdt_entry_init (uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entry entry;
    entry.base_low = (base & 0xFFFF);
    entry.base_middle = (base >> 16) & 0xFF;
    entry.base_high = (base >> 24) & 0xFF;

    entry.limit_low = (limit & 0xFFFF);
    entry.granularity = (limit >> 16) & 0x0F;

    entry.granularity |= gran & 0xF0;
    entry.access = access;
    return entry;
}