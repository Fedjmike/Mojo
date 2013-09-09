#include "kprintf.h"

#include "stdint.h"
#include "stdarg.h"
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

static int kprintf_putInt (int n) {
    const int base = 10;

    int printed = 0;
    
    /*Negative?*/
    if (n < 0) {
        kprintf_put('-');
        printed++;
        n = -n;
    }
    
    /*Work out how many digits there are*/
    
    int digits = 0;
    
    for (int copy = n; copy != 0; copy /= base)
        digits++;
        
    printed += digits;
    
    /* */
        
    int divisor = 1;
    
    /*divisor = base^(digits-1)*/
    for (int i = 1; i < digits; i++)
        divisor *= base;
        
    while (n != 0) {
        /*Quotient is the digit to be printed,
          remainder is the number to use next*/
        int lastdigit = n/divisor;
        n %= divisor;
        
        divisor /= base;
        
        switch (lastdigit) {
        case 0: kprintf_put('0'); break;
        case 1: kprintf_put('1'); break;
        case 2: kprintf_put('2'); break;
        case 3: kprintf_put('3'); break;
        case 4: kprintf_put('4'); break;
        case 5: kprintf_put('5'); break;
        case 6: kprintf_put('6'); break;
        case 7: kprintf_put('7'); break;
        case 8: kprintf_put('8'); break;
        case 9: kprintf_put('9'); break;
        case 10: kprintf_put('A'); break;
        case 11: kprintf_put('B'); break;
        case 12: kprintf_put('C'); break;
        case 13: kprintf_put('D'); break;
        case 14: kprintf_put('E'); break;
        case 15: kprintf_put('F'); break;
        default: kprintf("KPRINTF ERROR: putInt borked");
        }
    }
        
    return printed;
}

int kprintf (const char* format, ...) {
    int printed = 0;
    
    va_list args;
    va_start(args, format);
    
    for (int i = 0; format[i] != 0; i++) {
        if (format[i] == '%') {
            switch (format[++i]) {
            case 'd': case 'i':
                printed += kprintf_putInt(va_arg(args, int));
            
            break;
            default:
                kprintf_put('%');
                kprintf_put(format[i]);
                printed += 2;
            }
        
        } else {
            kprintf_put(format[i]);
            printed++;
        }
    }
    
    va_end(args);
    
    kprintf_updateCursor();
    
    return printed;
}