run: 
	riscv64-unknown-elf-as button.s -o button.o
	riscv64-unknown-elf-objcopy button.o -O binary button.bin
	mango-run button.bin
