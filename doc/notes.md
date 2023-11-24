<https://www.bilibili.com/video/BV1Q5411w7z5>

## basics

硬件架构
- 冯诺依曼架构: 指令和数据不分离, 大型服务器, 总线开销小, 控制逻辑简单, 执行效率低
- 哈佛架构: 指令和数据分离, 小型微控制器, 总线开销大, 控制逻辑实现复杂

mini pc conception
- 程序执行: fetch decode execute
- model: (REG <-> ALU <-> MEM) <-> CU(IR + PC)
- machine code -> asm -> interpreter/compiler + Language
- 简单机器码 format: addr reg opcode
- 指令设计 load store add
- 存储层级结构: reg cache mem disk network
  - trade-off: 速度 容量 成本
- 操作系统
  - 保护硬件 (防止失控软件滥用)
  - 抽象接口 (简单一致)

### isa

what is isa
- app - os - isa - microarchitecture
  - isa 是标准, 微架构是实现
  - isa 之于微架构, 如同 posix 之于 unix
- isa spec
  - 数据类型, 寄存器, 指令, 寻址模式, 异常中断等
  - 隔离了操作系统和硬件物理结构
- x86, arm, risc-v, sparc(sun), power(ibm), mips

cisc -> risc -> 融合
- 早期计算机, 内存少 -> cisc: 指令种类多, 程序短
- 硬件, 编译器能力逐渐解放 -> risc: 指令种类少, 程序长

isa 宽度 -> 通用寄存器的宽度
- 寻址范围 数据运算能力
- e.g. 8051, 8086, MSP430
- isa 宽度 != 指令长度 (e.g. x86)

risc-v isa
- 最初用于教学, x86/ARM 复杂, 以及 IP 问题
- 简单 稳定 分层 模块化 社区化
- 命名格式: RV\[字宽]\[模块]
    - RV32I RV32E RV64I
    - 模块: I E M A F D C, G=IMAFD
- 设计方式/理念
  - 增量 ISA: 保证向后兼容, 历史包袱重
  - 模块化 ISA: "插件化", 基本指令集 + 可选扩展指令集

basics in risc-v spec
- gpr: 32 reg + pc
- hart 硬件线程, 可理解为执行流, 或处理器, 或独立能取指并执行的单元
- privileged level 特权级别 U S M
  - 类比: 上电 -> 实模式/Machine -> 保护模式/Supervisor
  - M(embed), MU(secure embed), MSU(Unix-like, MMU)
  - 切换: ecall ebreak
- csr (control/status reg)
  - 不同特权级对应不同的 csr
  - 低级别下不能访问高级别的 csr
- 内存管理和保护
  - 物理内存保护 (R/W/X Lock)
  - 虚拟内存 (Supervisor, MMU)
- 异常和中断
  - Exception: 程序自身引起, 当前指令 -> 异常处理 -> 当前指令(重新执行)
  - Interrupt: 外设硬件引起, 当前指令 -> 中断处理 -> 下一条指令(透明)

## risc-v

assembly language
- 难读难写 难移植 (机器指令的 alias)
- 访问硬件 性能优化

asm syntax (gnu ver)
- statement: \[label:] \[operation] \[comment]
- label: alias for address
- operation = inst | pseudo-inst | directive | macro
  - pseudo-inst: alias for inst
  - directive: `.xx`, 语法糖, 非标准
  - macro: preprocessed `.macro`, `.endm`

操作对象
- 寄存器 x0 ~ x31, hart 的操作数必须来自寄存器
- 内存, RV32 寻址空间: 2^32

编码格式
![img:asm-encode-format](https://i.imgur.com/psmjsVf.png)
> register/immediate/store/branch/upper/jump
- 指令长度 ILEN1 = 32 (RV32I)
- 指令对齐 IALIGN = 32 (RV32I)
- 32 bits 分为不同的 fields
- opcode/funct3/funct7 > 决定指令类型
- 小端序: 多字节数如何存放
- asm: 汇编码 -> 确定 type -> 查表 (add) -> 填充值 -> 字节序 -> 机器码


### instructions


arithmetic instructions
```asm
add rd, rs1, rs2 # R
# 12 bit imm, 计算时符号拓展到 32 bit
addi rd, rs1, imm # I
# we have no `subi`

# psudo-inst
neg rd, rs # sub rd, x0, rs
mov rd, rs # addi rd, rs, 0
nop # addi x0, x0, 0

# 处理大数加法 -> 构造高拼接低
lui x5, 0x12345	    # x5 = 0x12345 << 12
addi x5, x5, 0x678  # x5 = x5 + 0x678
# 问题: 符号拓展
lui x5, 0x12345
addi x5, x5, 0xfff  # !!! 符号拓展
# hack trick
lui x5, 0x12346
addi x5, x5, 0xfff # -1
# better solution -> psudo-inst
li x5, 0x12345678

auipc x5, 0x12345 # x5 = PC + (0x12345 << 12)
auipc x6, 0	  # x6 = PC, to obtain the current PC
# i.e. pc+lui

# auipc -> la (psudo-inst)
la x5, _start # x5 = _start
jr x5
```
![img:arithmetic-inst](https://i.imgur.com/5nwnyMe.png)

logical instructions
![img:logical-inst](https://i.imgur.com/vSCenDB.png)
```asm
not x5, x6      # x5 = ~x6
xori x5, x6, -1	# the same as not
```

shifting instructions
> shift left/right logical/arithmetic
- 逻辑移位 sll srl slli srli
- 算术移位 sra srai
![img:shifting-inst](https://i.imgur.com/maDXtfA.png)

load and store instructions
- load -> I-type
```asm
lb rd, imm(rs1)
# rd = mem[(rs1) + imm]
```
![img:load-inst](https://i.imgur.com/maDXtfA.png)
- store -> S-type
![img:store-inst](https://i.imgur.com/0PpBfwp.png)
- load 要考虑拓展, store 则不需要?
  - load to reg, 粒度是整个 reg?
  - store from reg, 粒度是 byte?
  - 本质是不同存储单元位宽不匹配, 小的单元装到大的里需要确定拓展规则 (risc-v 只有 x0-x32, 而 x86 可以分 ah al ax eax rax...)
- 为什么 imm 编码分裂? 为什么区分 S-type 和 I-type?
  - 硬件设计, 解码效率和准确


contidtional branch instructions
- beq rs1, rs2, imm(label)
- jump to imm*2+pc
![img:cb-inst](https://i.imgur.com/0tVColS.png)
> beqz, why zero? -> 成功唯一, 失败多因
```asm
# int i = 0; # while (i < 5) i++;
li x5, 0
li x6, 5

jmp while
loop:
  addi x5, x5, 1
  while: blt x5, x6, loop
```

unconditional jump instructions (function call)
- jal (jump and link) -> J-type
  - `jal x1, imm/label` -> pc+/-1MB
  - sign-ex(imm*2)+pc
- jalr (jump and link register) -> I-type
  - `jalr x0, imm(rs1)` -> rs1+/-2KB
  - (sign-ex(imm)+rs1) & (~1)
```asm
# alias for no-return case
j offset # jal x0, offset
jr rs # jalr x0, 0(rs)

# far jump
auipc x6, imm-20
jalr x1, x6, imm-12

# void sum() { a = a + b; }
# void _start() { sum(); }
jal x5, sum
jalr x0, 0(x5)
```

addressing mode
![img:find-oprand](https://i.imgur.com/E7Smoew.png)

### calling conventions

calling conventions
- call func -> push
- exit func -> pop
- function -> stack frame (c 自动变量)
- protocol between caller and callee
  - callargs retaddr retval
  - specs -> compiler 工程化的必然

寄存器: 由谁来维护 (caller/callee)
![img:reg](https://i.imgur.com/eAnciAs.png)
- temporaries reg -> caller 维护
- saved reg -> callee 维护

跳转和返回指令
```asm
# 保存返回地址的跳转
jal offset # jal x1, offset
jalr rs # jalr x1, 0(rs)
# 函数返回
ret # jalr x0, 0(x1)

# 不保存返回地址的跳转
j offset # jal x0, offset
jr rs # jalr x0, 0(rs)

# 长跳转 auipc + jalr
call offset
# 长跳转+尾调用
tail offset
```

callee
- prologue: push saved (ra for non-leaf)
- epilogue: pop saved (ra for non-leaf)

call_leaf
```asm
# int square(int a) { return a * a; }
li a0, 3
call square
square:
  # prologue
  addi sp, sp, -8
  sw s0, 0(sp)
  sw s1, 4(sp)

  # body
  mv s0, a0
  mul s1, s0, s0
  mv a0, s1

  # epilogue
  lw s0, 0(sp)
  lw s1, 4(sp)
  addi sp, sp, 8
  ret
```

call_nested
```asm
# int aa_bb (int a, int b) { return square(a) + square(b); }
la sp, stack_end
li a0, 3
li a1, 4
call aa_bb

aa_bb:
  # prologue
  addi sp, sp, -16
  sw s0, 0(sp)
  sw s1, 4(sp)
  sw s2, 8(sp)
  sw ra, 12(sp)

  # body
  mv s0, a0
  mv s1, a1

  li s2, 0
  mv a0, s0
  jal square
  add s2, s2, a0
  mv a0, s1
  jal square
  add s2, s2, a0

  mov a0, s2

  # epilogue
  lw s0, 0(sp)
  lw s1, 4(sp)
  lw s2, 8(sp)
  lw ra, 12(sp)
  addi sp, sp, 16
  ret
```

### inline asm

asm -> c
```asm
# void foo(int a, int b) { return a + b; }
  .global foo
_start:
  li a0, 1
  call foo
```
inline asm
```c
int foo(int a, int b) {
  int c;
  asm volatile ( // volatile: 是否允许优化
    "add %[sum], %[add1], %[add2]" // asm
    :[sum]"=r"(c) // 输出操作数, 有可能写的寄存器
    :[add1]"r"(a), [add2]"r"(b) // 输入操作数
	//... 可能影响的寄存器/存储器
  );
}

// 简化, 位置参数
  asm volatile (
    "add %0, %1, %2" :"=r"(c) :"r"(a), "r"(b)
  );
```

### ebp

x86 引入 `ebp` 能更方便地维护栈(尤其是作 backtrace), risc-v 也有类似的操作, 比如用 `s0`
```
int foo(int a, int b)
{
80000042:       7179                    addi    sp,sp,-48
80000044:       d622                    sw      s0,44(sp)
80000046:       1800                    addi    s0,sp,48
80000048:       fca42e23                sw      a0,-36(s0)
8000004c:       fcb42c23                sw      a1,-40(s0)
        int c = a + b;
80000050:       fdc42703                lw      a4,-36(s0)
80000054:       fd842783                lw      a5,-40(s0)
80000058:       97ba                    add     a5,a5,a4
8000005a:       fef42623                sw      a5,-20(s0)
        return c;
8000005e:       fec42783                lw      a5,-20(s0)
80000062:       853e                    mv      a0,a5
80000064:       5432                    lw      s0,44(sp)
80000066:       6145                    addi    sp,sp,48
80000068:       8082                    ret
```
- s0 更新为新的栈帧顶部
- s0 上方 -> more args
- s0 下方 -> old s0, saved, ra, locals

## rvos
- 广义: 内核 + 软件 -> 发行包
- bare metal(embed) -> rtos(secure-embed) -> ros(rich os)

### bootstrap

![img:bootstrap](https://i.imgur.com/EfoJMx4.png)
qemu 开机执行 bootloader, 跳转到硬编码的 0x80000000
```asm
0x1000  auipc   t0,0x0
0x1004  add     a2,t0,40
0x1008  csrr    a0,mhartid
0x100c  lw      a1,32(t0)
0x1010  lw      t0,24(t0)
0x1014  jr      t0
```

os entry 也应该加载到 0x80000000
- 加载 `os.elf` 实际也是 qemu 完成的, 实际有一个 parse elf 的过程
- 也就是需要设置 entry point 的地址 0x80000000

gcc 没有把 entry 放到任意地址的能力 (workaround <https://zhuanlan.zhihu.com/p/444809991>)
```sh
# 令 .text 地址为 0x80000000
-Ttext 0x80000000 # -e _start
# 然后汇编里手动把 _start 放到 .text 头部
```
os entry (start.S)
- 只执行一个 Hart, 其他空转(wfi)
  - `csrw` <- `csrrw` (atomic r/w csr)
  - `csrr` <- `csrrs `(atomic r/s in csr)
- 每个 Hart 初始化栈空间 1K
- 跳转到 kernel
```sh
riscv64-elf-gcc -nostdlib -fno-builtin -march=rv32gc -mabi=ilp32 -g -Wall -c -o start.o start.S
riscv64-elf-gcc -nostdlib -fno-builtin -march=rv32gc -mabi=ilp32 -g -Wall -c -o kernel.o kernel.c
riscv64-elf-gcc -nostdlib -fno-builtin -march=rv32gc -mabi=ilp32 -g -Wall -Ttext=0x80000000 -o os.elf start.o kernel.o
riscv64-elf-objcopy -O binary os.elf os.bin
```

### uart

UART (Universal Asynchronous Receiver and Transmitter)
- 串行: 按位发送和接收, 顺序低到高位
- 波特率 (baud rate) -> bit per second (bps)
- 异步: 硬件上简单, 不需要时钟线, 不需要协商什么时候开始, 直接发送数据; 但软件上需要约定通讯协议格式
- 全双工

uart protocol
![img:uart](https://i.imgur.com/MZRLt6O.png)
- 数据位的长度和校验方式要预先配置
- 默认是无校验位: 大多数 com 口连接是近距离传送, 出错概率低


NS16550a 编程接口
- <http://byterunner.com/16550.html>
- <https://en.wikipedia.org/wiki/16550_UART>
- qemu 设置的 mmio 见 virt_memmap

polling
```c
int uart_putc(char ch) {
  while (0 == (uart_read_reg(LSR) & LSR_TX_IDLE));
  return uart_write_reg(THR, ch);
}
```

### memory manage
- 自动管理 - 栈 -> 编译器(自动)管理
- 静态内存 - 全局/静态变量 -> elf 数据段 (链接器负责)
- 动态内存 - 堆 -> `malloc`/`free` 系统调用, os 管理

> `os.ld` 描述 layout, 通过 `PROVIDE` 定义段界, 之后可在 asm 中使用, 并通过 `.global` 导出给 c
![img:sections](https://i.imgur.com/v88cxMA.png)

align page round up
```c
uint32_t order = (1 << PAGE_ORDER) - 1;
return (address + order) & (~order);
```

bitmap page management
![img:page-manage](https://i.imgur.com/1V1h6w8.png)
- alloc: find continous `npages` free pages
- free: calculate the bm addr, then "dealloc" till last page

### the first task

multitask via context switch
- os 记录维护每个 task 的 context, 单核时分复用 hart
- 调度 api: 当前任务保存 context, 获取新任务 context, ret 返回到新任务
- 唯一的问题: context 保存到哪里, 又从哪里获取 -> 任务队列

调度 api: `switch_to(next)`
- 预设: `mscratch` 指向当前 task 的 context
- save ctx: 保存当前 context 到 `mscratch` 指向
- 更新 `mscratch` 指针到 `next`
- restore ctx: 从新 `mscratch` 恢复新 context, `ret` 直接进入新任务
```asm
  # assert: mscratch point to current ctx

  csrrw	t6, mscratch, t6
  beqz	t6, label

  reg_save t6

  mv	t5, t6		# t5 points to the context of current task
  csrr	t6, mscratch	# read t6 back from mscratch
  sw	t6, 120(t5)	# save t6 with t5 as base

label:
  csrw	mscratch, a0

  mv	t6, a0
  reg_restore t6

  ret
```
> 实际上可以不单独拿一个闲置的 `mscratch`, xv6 用的是 `swtch(cur, next)`, os 维护当前任务指针, 和下一个任务的指针, context 不保存整个 regflie 而是 `ra, sp, callee-saved(s0~s11)`

kernel -> the first task (第一次执行 `switch_to`)
- 第一次不需要保存 ctx, 只要跳转到 next, 当然应该要先设计好 next 栈帧的 layout
- 具体实现: `mscratch` 初始化一个 dummy, 来区分这种 case
```c
void sched_init()
{
	w_mscratch(0);
	ctx_task.sp = (reg_t) &task_stack[STACK_SIZE];
	ctx_task.ra = (reg_t) user_task0;
}
```

> `mscratch` 要是能直接用将绝杀:
> ```asm
>   beqz	mscratch, label
>   reg_save mscratch
> label:
>   csrw	mscratch, a0
>   reg_restore mscratch
>   ret
> ```

关于 `mscratch`: 单独拿出一个寄存器来做 task schedule 会不会有点浪费
> Typically, it is used to hold a pointer to a machine-mode **hart-local** context space and swapped with a user register upon entry to an M-mode trap handler


### schedule

multitasking
- cooperative -> decided by task -> `task_yield`
- preemptive -> decided by os -> hook `schedule`

```c
// _top 当前任务数
// _current 当前任务
void schedule() // -> task_yield
{
	if (_top <= 0) {
		panic("Num of task should be greater than zero!");
		return;
	}

	_current = (_current + 1) % _top;
	struct context *next = &(ctx_tasks[_current]);
	switch_to(next);
}

int task_create(void (*start_routin)(void))
{
	if (_top < MAX_TASKS) {
		ctx_tasks[_top].sp = (reg_t) &task_stack[_top][STACK_SIZE];
		ctx_tasks[_top].ra = (reg_t) start_routin;
		_top++;
		return 0;
	} else {
		return -1;
	}
}
```

### trap & exception cfl

> how to hook `schedule` on interrupt (formally, on trap)?

> riscv 里 exception control flow 都可以叫 trap: 异常是 sync trap,  中断是 async trap

control flow -> exception control flow
![img:risc-v-trap](https://i.imgur.com/l7fiInG.png)
- trap 发生后要去哪 -> 替换 pc 为 `mtvec`
  - `mtvec` 低两位决定 mode, Direct/Vectored: 单个中断函数/中断向量表
- trap 后怎么回来 -> 保存 pc 到 `mepc` (`mret`)
- trap 发生的原因 -> `mcause`
  - msb: 是中断还是异常 + 中断/异常号
- trap 发生的具体原因 ->  `mtval`
- mstatus
![img:mstatus](https://i.imgur.com/IlxQx1w.png)
  - xIE (interrupt enable, in "x" level)
  - xPIE (previous IE, in "x" level)
  - xPP (previous privilege, in "x" level)
    - MPP: U (-> M), S (-> M), M (-> M)
    - SPP: U (-> S), S (-> S)
    - UPP?? U (-> U), 没必要区分, 如果当前是 U, 那么 trap 前一定也是 U


#### trap workflow

> 似乎中断并不"打断"指令, 反而异常"打断"

预设 (设置中断向量表...)
```c
void trap_init() {
	w_mtvec((reg_t)trap_vector);
}
```

top half: hardware part, hart 自动执行
- 中断设置 + 权限设置 + 地址设置
```sh
# the following op can be considered in parallel way
mstatus.MPIE = mstatus.MIE
mstatus.MIE = 0
mepc = is_intr ? pc : pc - off # so we need manually handle exception
pc = mtvec.BASE + mtvec.MODE == Vectored ? 4*excpetion-code : 0
mcause = what trap?
mtval = what additional?
mstatus.MPP = current mode
hart-priviledge =  machine-mode
```
> 如果 `mtvec.MODE` 设为 `vectored`, 那么实际 pc 跳转不是 `mtvec.BASE` 而是 `mtvec.BASE + 4 * code` (就是 IVT)


bottom half: software part
```asm
trap_vector:
	# save context
	csrrw	t6, mscratch, t6
	reg_save t6
	mv	t5, t6
	csrr	t6, mscratch
	sw	t6, 120(t5)
	csrw	mscratch, t5

	# call the C trap handler in trap.c
	csrr	a0, mepc
	csrr	a1, mcause
	call	trap_handler

	#  parse mepc out (error handling)
	csrw	mepc, a0

	# restore context
	csrr	t6, mscratch
	reg_restore t6

	mret
```

返回: `mret` -> `mepc`
![img:mret](https://i.imgur.com/6h9sIqW.png)
- 中断设置 + 权限设置 + 地址设置
```sh
hart-priviledge =  mstatus.MPP
mstauts.MPP = U
mstatus.MPIE = 1
mstatus.MIE = mstatus.MPIE
pc = mepc
```

### interrupt & plic
- software interrupt -> CLINT
- timer interrupt -> CLINT
- external interrupt -> PLIC

二级中断控制(`mstatus.mie` 之下)
- mie: enable (write)
- mip: pending (read)
![img:mie-mip](https://i.imgur.com/bUu4WiN.png)


platform-level interrupt controller
- <https://sifive.cdn.prismic.io/sifive/d3ed5cd0-6e74-46b2-a12d-72b06706513e_fu540-c000-manual-v1p4.pdf>
- 类比 x86 下: pic(intel 8259a), apic
- 集成在 soc 里的 hub
- 中断源 -> 中断号 -> 中断向量 -> 中断服务程序(ISR) -> 返回
![img:plic](https://i.imgur.com/441YewZ.png)
![img:plic-source](https://i.imgur.com/Mpypij1.png)
![img:plic-reg](https://i.imgur.com/0b5Pez7.png)

mmio (base 不在标准的范畴里, platform 自定义)
```c
#define UART0 0x10000000L
#define UART0_IRQ 10

#define PLIC_BASE 0x0c000000L

// 0 禁用中断源, 7 最高优先, 同优先级看 id 越小越优先
#define PLIC_PRIORITY(id) (PLIC_BASE + (id) * 4)
#define PLIC_PENDING(id) (PLIC_BASE + 0x1000 + ((id) / 32) * 4)
// 中断使能
#define PLIC_MENABLE(hart) (PLIC_BASE + 0x2000 + (hart) * 0x80)
// 丢弃阈值
#define PLIC_MTHRESHOLD(hart) (PLIC_BASE + 0x200000 + (hart) * 0x1000)

// (读) 获取最高的 Pending 中断源, 清除 Pending 位
#define PLIC_MCLAIM(hart) (PLIC_BASE + 0x200004 + (hart) * 0x1000)
// (写) 通知 PLIC 中断处理结束
#define PLIC_MCOMPLETE(hart) (PLIC_BASE + 0x200004 + (hart) * 0x1000)
```

用例
```c
void plic_init(void) {
	int hart = r_tp();
	*(uint32_t*)PLIC_PRIORITY(UART0_IRQ) = 1;
	*(uint32_t*)PLIC_MENABLE(hart)= (1 << UART0_IRQ);
	*(uint32_t*)PLIC_MTHRESHOLD(hart) = 0;
	// enable machine-mode external interrupts
	w_mie(r_mie() | MIE_MEIE);
	// enable machine-mode global interrupts
	w_mstatus(r_mstatus() | MSTATUS_MIE);
}

int plic_claim(void) {
	int hart = r_tp();
	int irq = *(uint32_t*)PLIC_MCLAIM(hart);
	return irq;
}

void plic_complete(int irq) {
	int hart = r_tp();
	*(uint32_t*)PLIC_MCOMPLETE(hart) = irq;
}

void external_interrupt_handler() {
	int irq = plic_claim();
	if (irq == UART0_IRQ){
      		uart_isr();
	} else if (irq) {
		printf("unexpected interrupt irq = %d\n", irq);
	}

	if (irq) {
		plic_complete(irq);
	}
}
```

rtl 视角看 PLIC
![img:rtl-plic](https://i.imgur.com/wKHLv5q.png)


### clint & hwtimer

hardware timer 是一种 local interrupt, 不是外设触发, 通过 Soc 内的 CLINT (Core Local INTerrupt) 来触发
![img:clint](https://i.imgur.com/JLLETkz.png)


mmio
```c
#define CLINT_BASE 0x2000000L
#define CLINT_MSIP(hartid) (CLINT_BASE + 4 * (hartid))

// mtime, 上电复位, real-time counter
#define CLINT_MTIME (CLINT_BASE + 0xBFF8) // cycles since boot.
// mtimecmp, 需手动初始化
#define CLINT_MTIMECMP(hartid) (CLINT_BASE + 0x4000 + 8 * (hartid))
// ticks per-second
#define CLINT_TIMEBASE_FREQ 10000000
```

硬件机制: 如果 `cmtime >= ctimecmp`, clint 发出中断
- 要产生周期性中断, 必须不断更新 `mtimecmp`
![img:clint](https://i.imgur.com/0InmhCu.png)
```c
void timer_load(int interval)
{
	/* each CPU has a separate source of timer interrupts. */
	int id = r_mhartid();
	*(uint64_t*)CLINT_MTIMECMP(id) = *(uint64_t*)CLINT_MTIME + interval;
}

void timer_init()
{
	// On reset, mtime is cleared to zero, but the mtimecmp registers
	// are not reset. So we have to init the mtimecmp manually.
	timer_load(TIMER_INTERVAL);
	// enable machine-mode timer interrupts
	w_mie(r_mie() | MIE_MTIE);
	// enable machine-mode global interrupts
	w_mstatus(r_mstatus() | MSTATUS_MIE);
}

void timer_handler()
{
	_tick++;
	timer_load(TIMER_INTERVAL);
}
```
### preemptive maultitasking

debug: 似乎只要在 `timer_handler` 里加入 `schedule` 就可以
- 目前 restore ctx 用 `ret -> ra`, 而中断返回用 `mret -> mepc`
- 中断里进行了两次上下文保存
  - 第一次 `trap_vector` 里保存 task1 ctx
  - 第二次 `switch_to` 保存中断的 ctx, 覆盖了 task1 ctx (因为中断没有更新 mscratch)
- 中断通过 `ret + ra` 转到了 task2, 本来应该用 `mret + mepc`
- 所以此时中断使能还是关闭的 -> 也就是没法再触发中断调度
  - 就算能调度, task1 ctx 已经被破坏了

重构 `switch_to`
- 消除歧义: 让 `switch_to` 只用来恢复 ctx, 用 `mret` 返回, 假定 caller 只能是中断例程
- 同时 `trap_vector` 里保存上下文时要多包括 `mepc`
```asm
trap_vector:
	# save
	csrrw	t6, mscratch, t6	
	reg_save t6
	mv	t5, t6		
	csrr	t6, mscratch	
	sw	t6, 120(t5)	
	csrr	a0, mepc
	sw	a0, 124(t5)

	csrw	mscratch, t5

	csrr	a0, mepc
	csrr	a1, mcause
	call	trap_handler
	csrw	mepc, a0

	# restore
	csrr	t6, mscratch
	reg_restore t6
	mret

.globl switch_to
.align 4
switch_to:
	csrw	mscratch, a0
	lw	a1, 124(a0)
	csrw	mepc, a1
	mv	t6, a0
	reg_restore t6

	# Do actual context switching.
	# Notice this will enable global interrupt
	mret
```

此时创建任务时, 也应该初始化 `mepc`(`context->pc`), 而不是 `ra`
```c
ctx_tasks[_top].sp = (reg_t) &task_stack[_top][STACK_SIZE];
// ctx_tasks[_top].ra = (reg_t) start_routin;
ctx_tasks[_top].pc = (reg_t) start_routin;
```

兼容 cooperative, 如何两个都要?
- 既然我们限制 `switch_to` 只从中断进入, 那么肯定需要一种软件 "主动触发中断" 的机制, 也就是软中断
![img:software-interrupt](https://i.imgur.com/mRyPk5U.png)


既然用了软中断, 软中断开关 flag 也要初始化:
```c
void sched_init()
{
	w_mscratch(0);
	// enable machine-mode software interrupts
	w_mie(r_mie() | MIE_MSIE);
}
```

此外, 我们第一次进入 task 时, 也会来一个 `mret`, 鉴于其副作用, 需要初始化一些 `mstatus` 的 field
```asm
# Notice: default mstatus is 0
# Set mstatus.MPP to 3, so we still run in Machine mode after MRET.
# Set mstatus.MPIE to 1, so MRET will enable the interrupt.
li	t0, 3 << 11 | 1 << 7
csrr	a1, mstatus
or	t0, t0, a1
csrw	mstatus, t0

j	start_kernel
```


### spinlock


原子交换
![img:spinlock](https://i.imgur.com/RNYXd2t.png)


关中断
```c
int spin_lock()
{
	w_mstatus(r_mstatus() & ~MSTATUS_MIE);
	return 0;
}

int spin_unlock()
{
	w_mstatus(r_mstatus() | MSTATUS_MIE);
	return 0;
}
```


### swtimer

```c
struct timer {
	void (*func)(void *arg);
	void *arg;
	uint32_t timeout_tick;
};

struct timer *timer_create(void (*handler)(void *arg), void *arg, uint32_t timeout)
{
	/* TBD: params should be checked more, but now we just simplify this */
	if (NULL == handler || 0 == timeout) {
		return NULL;
	}

	/* use lock to protect the shared timer_list between multiple tasks */
	spin_lock();

	struct timer *t = &(timer_list[0]);
	for (int i = 0; i < MAX_TIMER; i++) {
		if (NULL == t->func) {
			break;
		}
		t++;
	}
	if (NULL != t->func) {
		spin_unlock();
		return NULL;
	}

	t->func = handler;
	t->arg = arg;
	t->timeout_tick = _tick + timeout;

	spin_unlock();

	return t;
}

// timer_check in timer_handler (也就是中断上下文跑, 运行的时候不 tick...)
static inline void timer_check()
{
	struct timer *t = &(timer_list[0]);
	for (int i = 0; i < MAX_TIMER; i++) {
		if (NULL != t->func) {
			if (_tick >= t->timeout_tick) {
				t->func(t->arg);

				/* once time, just delete it after timeout */
				t->func = NULL;
				t->arg = NULL;

				break;
			}
		}
		t++;
	}
}
```

> 讲道理, 中断上下文也应该 tick 才对... 不然时钟怎么对
> 优化空间: 链表 跳表 (优先队列应该也可以


### syscall

kernel 和 user 的界限
- 第一次进入 user task: `schedule` -> `switch_to` -> `mret`
- `mret` 触发: `mstatus.{MPP,MPIE}` 恢复
- 初始化 `MPP` `MPIE` 为用户态, 通过 `mret` 来进入用户态
```asm
#ifdef CONFIG_SYSCALL
	# Set mstatus.MPP as 0, so we will run in User mode after MRET.
	# No need to set mstatus.MPIE to 1 explicitly, because according to ISA
	# specification: interrupts for M-mode, which is higher than U-mode, are
	# always globally enabled regardless of the setting of the global MIE bit.
	# So finally we simply reset t0 to zero.
	li	t0, 0
#else
	# Set mstatus.MPP to 3, so we still run in Machine mode after MRET.
	# Set mstatus.MPIE to 1, so MRET will enable the interrupt.
	li	t0, 3 << 11 | 1 << 7
#endif
csrr	a1, mstatus
or	t0, t0, a1
csrw	mstatus, t0

j	start_kernel
```

限制用户态没法使用特权指令, 用户使用封装过的 syscall
```c
// if syscall is supported, this will trigger exception, (for user mode)
// code = 2 (Illegal instruction)
hid = r_mhartid();
printf("hart id is %d\n", hid);
```


系统调用: ecall (environment call) 主动触发异常
> environment: 通过当前 hart 所在的特权级来设置 exception-code
![img:ecall](https://i.imgur.com/I6kcm3g.png)

设计: `func` -> `sys_func`
- user api: header + asm ecall
- os impl: syscall table/switch statement
  - `trap_vector` -> `trap_handler` -> excpetion handler -> `do_syscall` -> `sys_func`
![img:args](https://i.imgur.com/A6wFJ9e.png)

> 一般而言, 系统调用的接口 -> libc 库, impl -> kernel

## misc

### toolchains

交叉编译 cross
- native: host = target
- cross: host != target
  - host: 运行编译器的系统
  - target: 运行程序的系统
  - (build: 生成编译器的系统)

gdb
- b r c p, s: 进入函数, n: 不进入函数
- 远程调试 gdb -- gdbserver -- program
- <https://wangchujiang.com/linux-command/c/gdb.html>

qemu
- user mode
- system mode
```bash
riscv64-unknown-elf-gcc -march=rv32ima -mabi=ilp32 hello.c
qemu-riscv32 ./a.out
```

make
- `make -f makefile`
- 缺省规则 `.DEFAULT_GOAL := all`
- 伪规则 `.PHONY` `clean`

gcc (gnu complier collection)
```bash
gcc -E foo.c -o foo.i # cc1
gcc -S foo.i -o foo.s # cc1
gcc -c foo.s -o foo.o # as
gcc foo.o -o a.out    # ld
```
flags
```make
CFLAGS = -nostdlib -fno-builtin -march=rv32ima -mabi=ilp32 -g -Wall
```
- -nostdlib 不链接系统标准库
- -fno-builtin 不识别 不带 __builtin__ 前缀的 内置函数
- -march=rv32ima riscv 的 module
- -mabi=ilp32 指定 data model, ilp32 是所谓的 SysV-like (int, long, pointers 都是 32 位)

### executable linkable format(elf)
- file types
  - relocatable file: *.o
  - executable file: *.out
  - share object file: *.so
  - core dump file: core
- format
  - ELF Header (元数据)
  - Program Header Table (运行视图, 描述 Segment)
  - .text .init .data .bss ... (节)
  - Section Header Table (链接视图)
- binutils: ar as ld objcopy objdump readelf


### linker script
`ld --verbose`

组合 objects 的规则, 描述如何 merge 节, 组成段
```make
os.elf: ${OBJS}
	${CC} ${CFLAGS} -T os.ld -o os.elf $^
	${OBJCOPY} -O binary os.elf os.bin
```

syntax
- `ENTRY` -> 设置入口点
- `MEMORY` -> 描述新的 layout
- `SECTIONS`
  - 如何映射 input sections 到 output sections
  - 如何将 output sections 放到内存中
- `PROVIDE` -> 定义新的段
![img:sections](https://i.imgur.com/v88cxMA.png)

在代码中引用
```c
.global HEAP_START
HEAP_START: .word _heap_start
#
extern uint32_t HEAP_START;
```

### minimal code

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

只要消除 prologue/epilogue
```c
__attribute__((naked))
int _start() {
  // while(1);
  asm volatile("j _start");
}
```
> note: debug 报错 naked 函数体内只能用内联汇编, 但我用了 c 照样能过编译


## number flag label
forward/backward
```asm
	bgeu	a0, a1, 2f
1:
	sw	zero, (a0)
	addi	a0, a0, 4
	bltu	a0, a1, 1b
2:

```
