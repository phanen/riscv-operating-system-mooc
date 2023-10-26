
	.global	_start
	# size of each hart's stack is 1024 bytes
	.equ	STACK_SIZE, 1024


# s is addr of structure
.macro set_sturct s
	sw  a0, 0(\s)
	sw  a1, 4(\s)
.endm

.macro get_sturct s
	lw  a0, 0(\s)
	lw  a1, 4(\s)
.endm


_start:
	call foo
	ret
	j _start


foo:
	la s0, stru
	li a0, 0x12345678
	li a1, 0x87654321
	# get_sturct s0
	set_sturct s0
	li a0, 0
	li a1, 0
	get_sturct s0
	j _start

#  不要写 struct ... gdb 会认不出来..
stru:
	.word 1
	.word 1
