module kernel.main;

extern (C) void* _Dmodule_ref;

extern (C) int kprintf (const char* format, ...);

extern (C) void main (uint magic, uint addr) {
    ubyte* vidmem = cast(ubyte*) 0xFFFF8000000B8000;
 
    //Loops through the screen and clears it
    for (int i = 0; i < 25 * 80 * 2; i++)
        *(vidmem + i) = 0;
 
    kprintf("Hello world!");
 
    for (;;)
        {}
}