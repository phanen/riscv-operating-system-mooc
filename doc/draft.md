
## 3-2
<https://linuxtools-rst.readthedocs.io/zh-cn/latest/tool/nm.html>

- `readelf -s`
- `nm`
- `objdump -t`
- `objdump -s`

```c
int global_init = 0x11111111; // .sdata
const int global_const = 0x22222222; // .srodata

void main() {
  static int static_var = 0x33333333; // .sdata
  static int static_var_uninit; // .bss

  int auto_var = 0x44444444; // stack 上, .data?

  printf("hello world!\n"); // .strtab
  return;
}
```

## 4
<https://www.sifive.com/blog/all-aboard-part-1-compiler-args>

```sh
# ok
# riscv64-elf-gcc -nostdlib -fno-builtin -march=rv32ima -mabi=ilp32 -g -Wall -c hello.c

# not ok (abi??)
# riscv64-elf-gcc -march=rv32ima -mabi=ilp32 hello.c

# ok
riscv64-elf-gcc -march=rv32im -mabi=ilp32 hello.c

qemu-riscv32 ./a.out
```


## 5-1

```
sub x5, x6, x7          # x5 = x6 - x7
407302b3                sub     t0,t1,t2

R-type: funct7 rs2 rs1 funct3 rd opcode
0100000_00111_00110_000_00101_0110011
0100_0000_0111_0011_0000_0010_1011_0011
```

opcode -> sub -> R-type -> interpret for each field
```
b3 05 95 00
-> 00 95 05 b3
-> 0000_0000_1001_0101_0000_0101_1011_0011
-> _0110011 -> R-type -> funct3 -> 000 -> add
-> 0000000_01001_01010_000_01011_0110011
-> add x11, x10, x9 -> (or add a1, s1, a0)
```


## 7

