int _start() {
  unsigned int t = 0x87654321;
  unsigned short h = t >> 16;
  unsigned short l = t;
  // extern int printf (const char *__restrict __format, ...);
  // printf("%04x %04x", h, l);
  return 0;
}
