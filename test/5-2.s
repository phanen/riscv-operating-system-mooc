# register int a, b, c, d, e;
# b = 1;
# c = 2;
# e = 3;
# a = b + c;
# d = a - e;

	.text			# Define beginning of text section
	.global	_start		# Define entry _start

# 不论 register 关键字还有用没有...
# 一般变量用 sp 分配的
_start:
    li t2, 1
    li t3, 2
    li t5, 3
    add t1, t2, t3
    add t4, t1, t5

stop:
	j stop			# Infinite loop to stop execution

	.end			# End of file
