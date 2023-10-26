	.global	_start
# int t = 0x87654321;
# unsigned short h = t >> 16;
# unsigned short l = t;
# https://i.imgur.com/maDXtfA.png
# 直搬运内存也可以完成 16 位移位 (sw, lh, sh)
_start:
	li a0, 0x87654321
	#  srl a1, a0, 0x10 # also ok
	srli a1, a0, 0x10
	# andi a2, a0, 0xffff # error
	li t0, 0xffff
	and a2, a0, t0 # error
	j _start
