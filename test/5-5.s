	.global	_start
	# size of each hart's stack is 1024 bytes
	.equ	STACK_SIZE, 1024

_start:
	# li sp,	0x80000000
	la sp, stacks + STACK_SIZE
	addi    sp,sp,-32
	addi    s0,sp,32
	# int array[2] = {0x11111111, 0xffffffff};
	li      a0,0x11111111
	; sw      a0,-32(s0)
	li      a5,-1
	sw      a5,-28(s0)
	# int i = array[0];
	lw		a5,-32(s0)
	sw		a5,-20(s0)
	# int j = array[1];
	lw      a5,-28(s0)
	sw      a5,-24(s0)
	j _start

stacks:
	.skip	STACK_SIZE

	.end				# End of file

