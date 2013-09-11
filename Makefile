OBJS = $(patsubst src/%.d, obj/%.d.o, $(wildcard src/*.d))
OBJS += $(patsubst src/%.c, obj/%.c.o, $(wildcard src/*.c))
OBJS += $(patsubst src/%.s, obj/%.s.o, $(wildcard src/*.s))
OUT = bin/kernel
IMG = bin/floppy.img

AS = nasm
ASFLAGS = -f elf

DC = gdc
DFLAGS = -nostdlib -nodefaultlibs -g -c -m32

CC = gcc
CFLAGS = -nostdlib -nodefaultlibs -g -c -m32
CFLAGS += -std=c11 -Werror -Wall -Wextra

LD = $(HOST)-ld
LDFLAGS = -T link.ld

obj/%.d.o: src/%.d
	$(DC) $(DFLAGS) $< -o $@

obj/%.c.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

obj/%.s.o: src/%.s
	$(AS) $(ASFLAGS) $< -o $@
    
$(OUT): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(OUT)
    
$(IMG): $(OUT)
	losetup /dev/loop0 $(IMG)
	mount /dev/loop0 /mnt
	cp $(OUT) /mnt/kernel
	umount /dev/loop0
	losetup -d /dev/loop0
    
install: $(IMG)
    
clean:
	rm -vrf obj/*
	rm -vf $(OUT)

.PHONY = all install clean