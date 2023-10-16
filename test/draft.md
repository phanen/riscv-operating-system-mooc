
## 3
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


## 7

