#include "kprintf.h"

#include "stdint.h"
#include "hwio.h"

typedef struct {
    int x, y;
} position;

uint16_t *const video_memory = (uint16_t*) 0xFFFF8000000B8000;
/*Dimensions of the screen*/
const int width = 80, height = 25;
/*Current (logical) cursor position*/
position cursor;

/*Bit pattern for a black cell*/
const uint16_t blank = ' ';
uint8_t foreground = (0 << 4) | (15 & 0x0F);

/**
 * Update the position of the hardware cursor
 */
static void kprintf_updateCursor ();
/**
 * Scroll the screen n rows, updating the logical cursor position (not in hardware)
 */
static void kprintf_scroll (int n);

static void kprintf_put (char c);

static void kprintf_updateCursor () {
    uint16_t loc = cursor.y*width + cursor.x;
    /*Send the high bit*/
    hwio_out8(0x3D4, 14);
    hwio_out8(0x3D5, loc >> 8);
    /*And the low*/
    hwio_out8(0x3D4, 15);
    hwio_out8(0x3D5, loc);
}

static void kprintf_scroll (int n) {
    /*Copy height-n rows into their new position*/
    for (int i = 0; i < (height-n)*width; i++)
        video_memory[i] = video_memory[i+width*n];

    /*Black the following n rows*/
    for (int i = (height-n)*width; i < width*height; i++)
        video_memory[i] = blank;
    
    if (cursor.y >= n)
        cursor.y -= n;
}

static void kprintf_put (char c) {
    switch (c) {
    /*Backspace*/
    case '\b':
        if (cursor.x != 0)
            cursor.x--;
            
    break;
    /*Tab*/
    case '\t':
        cursor.x += 4;
        /*Wipe out lower 2 bits - round down to nearest multiple of 4*/
        cursor.x &= ~3;

    break;
    /*Carriage return*/
    case '\r':
        cursor.x = 0;

    /*Newline*/
    case '\n':
        cursor = (position) {0, cursor.y+1};
    
    break;
    /*Normal character*/
    default:
        video_memory[cursor.y*width + cursor.x] = (foreground << 8) | c;
        cursor.x++;
    }

    /*Wrap to new line?*/
    if (cursor.x >= width)
        cursor = (position) {0, cursor.y+1};
        
    /*Scroll?*/
    if (cursor.y >= height)
        kprintf_scroll(cursor.y-height);
}

int kprintf (const char* format, ...) {
    int i;
    
    for (i = 0; format[i] != 0; i++)
        kprintf_put(format[i]);
    
    kprintf_updateCursor();
    
    return i;
}