#include "paging.h"

#include "stdint.h"
#include "stdlib.h"
#include "kprintf.h"
#include "isr.h"
#include "kmalloc.h"
#include "frame.h"

typedef struct paging_page {
    bool present: 1;
    bool writeable: 1;
    bool user: 1;
    bool accessed: 1;
    bool dirty: 1;
    uint32_t unused: 7;
    uint32_t frame: 20;
} paging_page;

typedef struct paging_table {
    paging_page pages[tablesize];
} paging_table;

typedef struct paging_dir {
    /*Array of table ptrs, and their physical addresses*/
    paging_table* tables[tablesize];
    uint32_t tablesPhysical[tablesize];
    
    /*Physical address of tablesPhysics*/
    uint32_t* tablesPhysicalAddress;
} paging_dir;

paging_dir *kernelDir, *currentDir;

static void paging_enable ();

static void paging_mapPage (paging_page* page, uint32_t frame, bool writeable, bool user);

static void paging_faultHandler (isr_args args);

void paging_init () {
    frame_init();
    
    isr_registerHandler(ISR_PAGEFAULT, paging_faultHandler);

    /*Kernel page directory*/
    kernelDir = kmalloc_aligned(sizeof(paging_dir), pagesize);
    memset(kernelDir, 0, sizeof(paging_dir));
    
    /*Identity map the kernel memory*/
    for (void* i = 0; i < kmalloc_last(); i += pagesize)
        /*Accessible but not writeable from usermode*/
        paging_mapPage(paging_getPage(kernelDir, i, true), frame_alloc(), false, true);
        
    paging_setDir(kernelDir);
    paging_enable();
 }

void paging_setDir (paging_dir* dir) {
    currentDir = dir;
    __asm__ volatile ("mov %0, %%cr3":: "r"(&currentDir->tablesPhysical));
}

static void paging_enable () {
    uint32_t cr0;
    __asm__ volatile ("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 1 << 31;
    __asm__ volatile ("mov %0, %%cr0":: "r"(cr0));
}

paging_page* paging_getPage (paging_dir* dir, void* address, bool make) {
    uintptr_t page = (uintptr_t) address / pagesize;
    uint32_t firstIndex = page / tablesize;
    uint32_t secondIndex = page % tablesize;
    
    if (dir->tables[firstIndex])
        return &dir->tables[firstIndex]->pages[secondIndex];
    
    else if (make) {
        paging_table* table = kmalloc_aligned(sizeof(paging_table), pagesize);
        memset(table, 0, sizeof(paging_table));
        
        dir->tables[firstIndex] = table;
        /*Present, writeable, user*/
        dir->tablesPhysical[firstIndex] = (uint32_t) table | 0x7;
        
        return &dir->tables[firstIndex]->pages[secondIndex];
    
    } else
        return 0;
}

static void paging_mapPage (paging_page* page, uint32_t frame, bool writeable, bool user) {
    if (page->frame)
        kprintf("PAGING ERROR: page already mapped");

    page->present = true;
    page->writeable = writeable;
    page->user = user;
    page->frame = frame;
}

static void paging_faultHandler (isr_args args) {
    (void) args;

    uint32_t faultingAddress;
    __asm__ volatile ("mov %%cr2, %0" : "=r"(faultingAddress));

    kprintf("PAGE FAULT: at 0x%p\n", faultingAddress);
    
    for (int i;;) {i++;}
}