NAME = blink_actled

ARCH = -march=rv64im -mabi=lp64
CFLAGS = $(ARCH) -g -Og -Wall -ffreestanding
LDFLAGS = -nostdlib -e main

run: $(NAME).bin
	mango-run $<

%.bin: %.elf
	riscv64-unknown-elf-objcopy $< -O binary $@

%.elf: %.o
	riscv64-unknown-elf-gcc $(LDFLAGS) $< -o $@

%.o: %.c
	riscv64-unknown-elf-gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.elf *.bin

.PRECIOUS: %.o %.elf
