module kernel.main;

extern (C) void* _Dmodule_ref;

extern (C) int kprintf (const char* format, ...);

extern (C) void gdt_init ();
extern (C) void idt_init ();

extern (C) void main (uint magic, uint addr) {
    ubyte* vidmem = cast(ubyte*) 0xFFFF8000000B8000;
 
    //Loops through the screen and clears it
    for (int i = 0; i < 25 * 80 * 2; i++)
        *(vidmem + i) = 0;
 
    kprintf("Hello world!");
    
    gdt_init(); kprintf("GDT initialized\n");
    idt_init(); kprintf("IDT initialized\n");

    asm {
        int 0x3;
        int 0x4;
    }
 
    for (;;)
        {}
}