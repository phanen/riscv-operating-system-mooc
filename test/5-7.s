	.global	_start
	.text
_start:
	la a1, msg

	loop:
	lbu a2, 0(a1)
	beqz a2, stop
	addi a1, a1, 1
	j loop
	stop:
	j _start

msg:
	.string "hello,world!\0"
