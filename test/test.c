#define STACK_SIZE 1024
__attribute__((aligned(16))) unsigned char stack[STACK_SIZE];

__attribute__((naked)) int _start() {
  // TODO: 不知道为啥不行?
  // asm volatile("la sp, stack + 1024");

  asm volatile("la sp, stack\n"
               "addi sp, sp, 1024\n");

  int foo(int a, int b);
  foo(1, 2);
  return 0;
}

int foo(int a, int b) {
  int c;
  // c = a * a + b * b;
  asm volatile("lw a0, %1\n"
               "lw a1, %2\n"
               "mul a0, a0, a0\n"
               "mul a1, a1, a1\n"
               "add %0, a0, a1"
               : "=r"(c)
               : "m"(a), "m"(b)
               : "a0", "a1");

  return c;
}
