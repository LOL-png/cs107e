NAME = buggy

ARCH    = -march=rv64im -mabi=lp64
ASFLAGS = $(ARCH)
CFLAGS  = $(ARCH) -g -Og -I$$CS107E/include -Wall -ffreestanding
LDFLAGS = -nostdlib -L$$CS107E/lib -T memmap.ld
LDLIBS  = -lmango

all : $(NAME).bin

%.bin: %.elf
	riscv64-unknown-elf-objcopy $< -O binary $@

%.elf: %.o
	riscv64-unknown-elf-gcc $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	riscv64-unknown-elf-gcc $(CFLAGS) -c $< -o $@

%.o: %.s
	riscv64-unknown-elf-as $(ASFLAGS) $< -o $@

run: $(NAME).bin
	mango-run $<

debug: $(NAME).elf
	riscv64-unknown-elf-gdb $(GDB_FLAGS) $<

clean:
	rm -f *.o *.bin *.elf *.list *~

.PHONY: all clean run debug
.PRECIOUS: %.elf %.o

GDB_FLAGS = -q --command=$$CS107E/other/gdbsim.commands
