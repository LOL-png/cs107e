# `make all` (or just `make)` compiles both
all: larson.bin extension.bin

# larson.bin is built from the assembly program larson.s
larson.bin: larson.s
	riscv64-unknown-elf-as larson.s -o larson.o
	riscv64-unknown-elf-objcopy larson.o -O binary larson.bin

# `make run` sends larson binary to Pi to execute
run: larson.bin
	mango-run larson.bin

# extension.bin is built from extension.s
extension.bin: extension.s
	riscv64-unknown-elf-as extension.s -o extension.o
	riscv64-unknown-elf-objcopy extension.o -O binary extension.bin

# `make extension` sends larson binary to Pi to execute
extension: extension.bin
	mango-run extension.bin


# `make clean` removes all compiled build products
clean:
	rm -f larson.o larson.bin extension.o extension.bin
