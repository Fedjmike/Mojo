#include "kprintf.h"

#include "std.h"
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

static int kprintf_putInt (int n, unsigned int base);
static int kprintf_putUint (unsigned int n, unsigned int base);

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

    break;
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
        kprintf_scroll(cursor.y-height+1);
}

static int kprintf_putInt (int n, unsigned int base) {
    int printed = 0;
    
    /*Negative?*/
    if (n < 0) {
        kprintf_put('-');
        printed++;
        n = -n;
    }
    
    return printed + kprintf_putUint(n, base);
}

static int kprintf_putUint (unsigned int n, unsigned int base) {
    int printed = 0;
    
    /*Work out how many digits there are*/
    
    int digits = 1;
    
    for (unsigned int copy = n; copy >= base; copy /= base)
        digits++;
        
    printed += digits;
    
    /* */
        
    int divisor = 1;
    
    /*divisor = base^(digits-1)*/
    for (int i = 1; i < digits; i++)
        divisor *= base;
        
    do {
        /*Quotient is the digit to be printed,
          remainder is the number to use next*/
        int lastdigit = n/divisor;
        n %= divisor;
        
        divisor /= base;
        
        char digits[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                           'A', 'B', 'C', 'D', 'E', 'F'};
        
        if (lastdigit < 16)
            kprintf_put(digits[lastdigit]);
            
        else
            kprintf("KPRINTF ERROR: putInt borked");
    } while (--digits);
        
    return printed;
}

int kprintf (const char* format, ...) {
    int printed = 0;
    
    va_list args;
    va_start(args, format);
    
    for (int i = 0; format[i] != 0; i++) {
        if (format[i] == '%') {
            i++;
            
            switch (format[i]) {
            case 'd':
            case 'i':
                printed += kprintf_putInt(va_arg(args, int), 10);
                
            break;
            case 'p':
                printed += kprintf_putUint(va_arg(args, int), 16);
            
            break;
            default:
                kprintf_put('%');
                kprintf_put(format[i+1]);
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