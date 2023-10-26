#include <stdio.h>

int global_init = 0x11111111; // .sdata
const int global_const = 0x22222222; // .srodata

void main() {
  static int static_var = 0x33333333; // .sdata
  static int static_var_uninit; // .bss

  int auto_var = 0x44444444; // stack 上

  printf("hello world!\n"); // .strtab
  return;
}
