
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


## minimal code

```c
	.global	_start		# Define entry _start
_start:
	j _start

```

如果用纯 c 怎么写呢, 你可能会想
```c
int _start() {
  while(1);
}
```

实际上, 没有了 crt, sp 没有初始化, 而编译器会帮 `_start` 默认生成 `prologue`/`epilogue`...
- 那么如何消除 prologue/epilogue, 还得是 gpt...
```c
__attribute__((naked))
int _start() {
  // while(1);
  asm volatile("j _start");
}
```
> note: debug 报错 naked 函数体内只能用内联汇编, 但我用了 c 照样能过编译


## ebp

```
int _start() {
80000000:       1101                    addi    sp,sp,-32
80000002:       ce22                    sw      s0,28(sp)
80000004:       1000                    addi    s0,sp,32

...............


80000024:       4472                    lw      s0,28(sp)
80000026:       6105                    addi    sp,sp,32
80000028:       8082                    ret

```


## specify arbitrary entry
- <https://zhuanlan.zhihu.com/p/444809991>

rvos 的 bootloader 和 kernel 是一体的, 而 `qemu -bios none` 裸机显然不可能有 parse `os.elf` 的能力, 自然不可能聪明的直接跳跃到 entry
而是会跳到硬编码的 0x80000000, 那么作为限制, 我们也需要令 `os.elf` 的 entry 地址固定为 0x80000000

但是: gcc 没有把 entry 放到任意地址的能力, 作为一个 workaroud:
```sh
# 令 .text 地址为 0x80000000
-Ttext 0x80000000 # -e _start
# 然后通过写汇编把 _start 放到 .text 头部
```

如果我想修改 entry, 可能的方式有:
- 直接改源码, 调整位置
- 先编译出来, 看 entry 的 offset, 把 -Ttext 修改一下 (姑且不说 load 阶段能不能搬运成功...)
```
# objdump -h os.elf
```

所以就灵活性而言, 分离出一个 loader 和 kernel 还是挺重要的

## before jmp to 0x80000000

qemu 开机执行:
```
0x1000  auipc   t0,0x0
0x1004  add     a2,t0,40
0x1008  csrr    a0,mhartid
0x100c  lw      a1,32(t0)
0x1010  lw      t0,24(t0)
0x1014  jr      t0
0x1018  unimp
0x101a  .2byte  0x8000
0x101c  unimp
0x101e  unimp
0x1020  unimp
0x1022  .2byte  0x87e0
0x1024  unimp
0x1026  unimp
0x1028  fnmadd.s        ft6,ft4,fs4,fs1,unknown
0x102c  c.slli64        zero
0x102e  unimp
0x1030  fsw     fs0,32(s0)
0x1032  .2byte  0x7fff
0x1034  nop
```
