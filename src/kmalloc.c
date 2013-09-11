#include "kmalloc.h"

#include "stdint.h"

/*From the linker script*/
extern uint32_t end;
void* address = &end;

static uintptr_t kmalloc_align (uintptr_t ptr, int alignment);
static void* kmalloc_impl (size_t size, int alignment, void** out);

static uintptr_t kmalloc_align (uintptr_t ptr, int alignment) {
    uintptr_t wipeoutmask = ~(alignment-1);
    
    /*Not aligned already?*/
    if ((ptr & wipeoutmask) != ptr) {
        ptr &= wipeoutmask;
        ptr += alignment;
    }
    
    return ptr;
}

static void* kmalloc_impl (size_t size, int alignment, void** out) {
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
    return kmalloc_impl(size, 1, &tmp);
}

void* kmalloc_last () {
    return address;
}