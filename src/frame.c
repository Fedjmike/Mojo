#include "frame.h"

#include "stdint.h"
#include "stdlib.h"
#include "kmalloc.h"
#include "paging.h"

typedef uint32_t pack_t;
enum {packing = 8*sizeof(pack_t)};

pack_t* frames;
unsigned int packNo;

void frame_init () {
    uint32_t lastFrame = 16*1024*1024;
    packNo = lastFrame / (pagesize*packing);
    
    frames = kmalloc(packNo);
    memset(frames, 0, packNo);
}

uint32_t frame_alloc () {
    for (uint32_t i = 0; i < packNo; i++) {
        /*Any free in this pack?*/
        if (frames[i] != ~(pack_t) 1) {
            /*For every bit*/
            for (int j = 0; j < 32; j++) {
                pack_t bit = 1 << j;
                
                if (!(frames[i] & bit)) {
                    frames[i] |= bit;
                    return i*packing + j;
                }
            }
        }
    }
    
    return ~(uint32_t) 1;
}

void frame_free (uint32_t frame) {
    uint32_t index = frame / packing;
    uint32_t offset = frame % packing;
    frames[index] &= ~(1 << offset);
}