module kernel.main;

extern (C) void* _Dmodule_ref;

extern (C) int kprintf (const char* format, ...);

extern (C) void gdt_init ();
extern (C) void idt_init ();

extern (C) void timer_init (int freq);

extern (C) void main (uint magic, uint addr) {
    ubyte* vidmem = cast(ubyte*) 0xFFFF8000000B8000;
 
    //Loops through the screen and clears it
    for (int i = 0; i < 25 * 80 * 2; i++)
        *(vidmem + i) = 0;
 
    kprintf("Hello world!\n");
    
    gdt_init(); kprintf("GDT initialized\n");
    idt_init(); kprintf("IDT initialized\n");
    
    timer_init(20); kprintf("Timer initialized\n");
    
    asm {
        sti;
    }
 
    for (;;)
        {}
}