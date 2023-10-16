# ok
# riscv64-elf-gcc -nostdlib -fno-builtin -march=rv32ima -mabi=ilp32 -g -Wall -c hello.c

# not ok (abi??)
# riscv64-elf-gcc -march=rv32ima -mabi=ilp32 hello.c

# ok
riscv64-elf-gcc -march=rv32im -mabi=ilp32 $1

qemu-riscv32 ./a.out
