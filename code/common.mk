CROSS_COMPILE = riscv64-elf-
CFLAGS = -nostdlib -fno-builtin -march=rv32gc -mabi=ilp32 -g -Wall
# CFLAGS = -nostdlib -fno-builtin -march=rv32gc -mabi=ilp32 -Wall -O2 # to disable prologue?
# CFLAGS = -march=rv32im -mabi=ilp32 -g -Wall

QEMU = qemu-system-riscv32
QFLAGS = -nographic -smp 1 -machine virt -bios none

GDB = riscv64-elf-gdb
CC = ${CROSS_COMPILE}gcc
OBJCOPY = ${CROSS_COMPILE}objcopy
OBJDUMP = ${CROSS_COMPILE}objdump
