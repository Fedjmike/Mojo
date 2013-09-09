[GLOBAL gdt_flush]
gdt_flush:
    mov eax, [esp+4]
    lgdt [eax]

    mov ax, 0x10 ; Data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush ; Far jump to code segment
.flush:
    ret