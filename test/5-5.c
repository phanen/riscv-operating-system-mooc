// if we don't use crt, how to write pure code??
__attribute__((naked)) int _start() {
  // while(1);
  // _start:
  // 实际上这么搞栈指针是没法写进去的, 我暂时也不知道为啥...(可能是不可写的段?)
  //   static const MemMapEntry virt_memmap[] = {
  //     [VIRT_DEBUG] =        {        0x0,         0x100 },
  //     [VIRT_MROM] =         {     0x1000,        0xf000 },
  //     [VIRT_TEST] =         {   0x100000,        0x1000 },
  //     [VIRT_RTC] =          {   0x101000,        0x1000 },
  //     [VIRT_CLINT] =        {  0x2000000,       0x10000 },
  //     [VIRT_ACLINT_SSWI] =  {  0x2F00000,        0x4000 },
  //     [VIRT_PCIE_PIO] =     {  0x3000000,       0x10000 },
  //     [VIRT_PLATFORM_BUS] = {  0x4000000,     0x2000000 },
  //     [VIRT_PLIC] =         {  0xc000000, VIRT_PLIC_SIZE(VIRT_CPUS_MAX * 2)
  //     }, [VIRT_APLIC_M] =      {  0xc000000, APLIC_SIZE(VIRT_CPUS_MAX) },
  //     [VIRT_APLIC_S] =      {  0xd000000, APLIC_SIZE(VIRT_CPUS_MAX) },
  //     [VIRT_UART0] =        { 0x10000000,         0x100 },
  //     [VIRT_VIRTIO] =       { 0x10001000,        0x1000 },
  //     [VIRT_FW_CFG] =       { 0x10100000,          0x18 },
  //     [VIRT_FLASH] =        { 0x20000000,     0x4000000 },
  //     [VIRT_IMSIC_M] =      { 0x24000000, VIRT_IMSIC_MAX_SIZE },
  //     [VIRT_IMSIC_S] =      { 0x28000000, VIRT_IMSIC_MAX_SIZE },
  //     [VIRT_PCIE_ECAM] =    { 0x30000000,    0x10000000 },
  //     [VIRT_PCIE_MMIO] =    { 0x40000000,    0x40000000 },
  //     [VIRT_DRAM] =         { 0x80000000,           0x0 },
  // };
  // 这样 sp 会位于 VIRT_PCIE_MMIO, gpt:
  //
  // 在 QEMU 中, VIRT_PCIE_MMIO 是用于模拟 PCIe 设备的内存映射 I/O (MMIO)
  // 地址空间. 当你在 QEMU 中写入到 VIRT_PCIE_MMIO 段地址时, 但读取后却始终为
  // 0xffffffff, 可能是由以下原因引起的:
  //
  // 未正确初始化 PCIe 设备的 MMIO 地址空间: 在 QEMU 中, 你需要正确初始化 PCIe
  // 设备的 MMIO
  // 地址空间, 包括设置基址和长度等. 如果没有正确初始化地址空间,
  // 读取该地址可能会返回默认值 0xffffffff.
  //
  // 写入的地址超出了有效的 MMIO 地址范围: 请确保你写入的地址在有效的 MMIO
  // 地址范围内. 如果写入的地址超出了有效范围, 读取该地址可能会返回默认值
  // 0xffffffff.
  //
  // QEMU 配置错误: 检查 QEMU 的配置文件或命令行参数, 确保正确配置了 PCIe
  // 设备和相关的 MMIO 地址空间. 可能需要指定正确的设备模型, PCIe 槽位以及 MMIO
  // 基址和长度等参数.
  //
  // PCIe 设备驱动问题: 如果 PCIe 设备有相关的驱动程序, 确保驱动程序正确处理了
  // MMIO 地址空间的读写操作, 以及正确解析和处理写入和读取的数据.
  //
  // asm volatile("li sp, 0x80000000");
  // asm volatile("call foo");
  // 合理的栈分配方法见 5-9
  // while (1)
    // ;
  // asm volatile("j _start");
}

int foo() {
  int array[2] = {0x11111111, 0xffffffff};
  int i = array[0];
  int j = array[1];
  return 1;
}
