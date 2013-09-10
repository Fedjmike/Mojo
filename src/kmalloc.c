#include "kmalloc.h"

#include "stdint.h"

static uintptr_t kmalloc_align (uintptr_t ptr, int alignment);
static void* kmalloc_impl (size_t size, int alignment, void** out);

static uintptr_t kmalloc_align (uintptr_t ptr, int alignment) {
    uintptr_t wipeoutmask = ~((1 << alignment) - 1);
    
    /*Not aligned already?*/
    if ((ptr & wipeoutmask) != ptr) {
        ptr &= wipeoutmask;
        ptr += 1 << alignment;
    }
    
    return ptr;
}

static void* kmalloc_impl (size_t size, int alignment, void** out) {
    static void* address = (void*) 0x100000;
    
    /*Align it upwards to the 2^alignment byte*/
    address = (void*) kmalloc_align((uintptr_t) address, alignment);
    
    /*Save the address, move past this allocation*/
    *out = address;
    address += size;
    return *out;
}

void* kmalloc_aligned (size_t size, int alignment) {
    void* tmp;
    return kmalloc_impl(size, alignment, &tmp);
}

void* kmalloc (size_t size) {
    void* tmp;
    return kmalloc_impl(size, 0, &tmp);
}