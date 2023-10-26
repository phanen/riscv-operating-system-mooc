	.global	_start
	.text
_start:
	la sp, stack_end	# prepare stack for calling functions
	# li sp, 0x80000000 # error... gp can not dereference?
	li a0, 0x3
	call sum_square

	j _start



# leaf
square:
	mul a0, a0, a0
	ret

#

sum_square:
	addi sp, sp, -4
	sw ra, 0(sp)

	# manually optmized...

	li t1, 0 # sum
	li t2, 1 # j
	mv t3, a0 # i
	loop:
	bgtu t2, t3, stop
	mv a0, t2
	call square
	add t1, t1, a0
	addi t2, t2, 1
	j loop
	stop:
	mv a0, t1

	lw ra, 0(sp)
	addi sp, sp, 4

	ret

stack_start:
	.rept 1
	.word 0
	.endr
stack_end:

	.end			# End of file
