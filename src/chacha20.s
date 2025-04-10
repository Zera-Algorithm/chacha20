	.file	"chacha20_mod.c"
	.option pic
	.attribute arch, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_v1p0_zicsr2p0_zifencei2p0_zbb1p0_zvbb_zve32f1p0_zve32x1p0_zve64d1p0_zve64f1p0_zve64x1p0_zvl128b1p0_zvl32b1p0_zvl64b1p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
# GNU C17 (Ubuntu 13.3.0-6ubuntu2~24.04) version 13.3.0 (riscv64-linux-gnu)
#	compiled by GNU C version 13.3.0, GMP version 6.3.0, MPFR version 4.2.1, MPC version 1.3.1, isl version isl-0.26-GMP

# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed: -mabi=lp64d -misa-spec=20191213 -march=rv64imafdcv_zicsr_zifencei_zbb_zvbb_zve32f_zve32x_zve64d_zve64f_zve64x_zvl128b_zvl32b_zvl64b -O3 -fno-builtin
	.text
	.align	1
	.globl	chacha20
	.type	chacha20, @function
chacha20:
.LFB1:
	.cfi_startproc
# src/host/chacha20_mod.c:243:         asm(
	li	a5,4		# tmp139,
# src/host/chacha20_mod.c:248:         if (actual_len != 4) {
	li	a3,4		# tmp141,
# src/host/chacha20_mod.c:243:         asm(
#APP
# 243 "src/host/chacha20_mod.c" 1
	vsetvli a5, a5, e32	# actual_len, tmp139
# 0 "" 2
#NO_APP
	sext.w	a5,a5	# actual_len, actual_len
# src/host/chacha20_mod.c:240: {
	mv	a6,a0	# output, tmp160
	mv	a4,a1	# input, tmp161
# src/host/chacha20_mod.c:248:         if (actual_len != 4) {
	beq	a5,a3,.L4	#, actual_len, tmp141,
# src/host/chacha20_mod.c:286: }
	ret	
.L4:
# src/host/chacha20_mod.c:123:     asm(
	lla	a5,.LANCHOR0	# tmp142,
	lla	a7,.LANCHOR0+16	# tmp144,
	lla	t1,.LANCHOR0+32	# tmp146,
	lla	t3,.LANCHOR0+48	# tmp148,
	lla	t4,.LANCHOR0+64	# tmp150,
	lla	t5,.LANCHOR0+80	# tmp153,
	lla	t6,.LANCHOR0+96	# tmp156,
	lla	t2,.LANCHOR0+112	# tmp159,
#APP
# 123 "src/host/chacha20_mod.c" 1
	move a0, a4	# input
vle32.v v0, (a0)
addi a0, a0, 16
vle32.v v1, (a0)
addi a0, a0, 16
vle32.v v2, (a0)
addi a0, a0, 16
vle32.v v3, (a0)
vmv.v.v v16, v0
vmv.v.v v17, v1
vmv.v.v v18, v2
vmv.v.v v19, v3
li a0, 16
li a1, 12
li a2, 8
li a3, 7
vle32.v v4, (a5)	# tmp142
vle32.v v5, (a7)	# tmp144
vle32.v v6, (t1)	# tmp146
vle32.v v7, (t3)	# tmp148
vle32.v v8, (t4)	# tmp150
vle32.v v9, (t5)	# tmp153
vle32.v v10, (t6)	# tmp156
vle32.v v11, (t2)	# tmp159
li t0, 10
loop: 
vadd.vv v0, v0, v1
vxor.vv v3, v3, v0
vrol.vx v3, v3, a0
vadd.vv v2, v2, v3
vxor.vv v1, v1, v2
vrol.vx v1, v1, a1
vadd.vv v0, v0, v1
vxor.vv v3, v3, v0
vrol.vx v3, v3, a2
vadd.vv v2, v2, v3
vxor.vv v1, v1, v2
vrol.vx v1, v1, a3
vrgather.vv v12, v0, v4
vrgather.vv v13, v1, v5
vrgather.vv v14, v2, v6
vrgather.vv v15, v3, v7
vadd.vv v12, v12, v13
vxor.vv v15, v15, v12
vrol.vx v15, v15, a0
vadd.vv v14, v14, v15
vxor.vv v13, v13, v14
vrol.vx v13, v13, a1
vadd.vv v12, v12, v13
vxor.vv v15, v15, v12
vrol.vx v15, v15, a2
vadd.vv v14, v14, v15
vxor.vv v13, v13, v14
vrol.vx v13, v13, a3
vrgather.vv v0, v12, v8
vrgather.vv v1, v13, v9
vrgather.vv v2, v14, v10
vrgather.vv v3, v15, v11
addi t0, t0, -1
bnez t0, loop
vadd.vv v0, v0, v16
vadd.vv v1, v1, v17
vadd.vv v2, v2, v18
vadd.vv v3, v3, v19
move a0, a6	# output
vse32.v v0, (a0)
addi a0, a0, 16
vse32.v v1, (a0)
addi a0, a0, 16
vse32.v v2, (a0)
addi a0, a0, 16
vse32.v v3, (a0)

# 0 "" 2
# src/host/chacha20_mod.c:286: }
#NO_APP
	ret	
	.cfi_endproc
.LFE1:
	.size	chacha20, .-chacha20
	.globl	rev_assign
	.globl	assign
	.section	.rodata
	.align	3
	.set	.LANCHOR0,. + 0
	.type	assign, @object
	.size	assign, 64
assign:
	.word	0
	.word	1
	.word	2
	.word	3
	.word	1
	.word	2
	.word	3
	.word	0
	.word	2
	.word	3
	.word	0
	.word	1
	.word	3
	.word	0
	.word	1
	.word	2
	.type	rev_assign, @object
	.size	rev_assign, 64
rev_assign:
	.word	0
	.word	1
	.word	2
	.word	3
	.word	3
	.word	0
	.word	1
	.word	2
	.word	2
	.word	3
	.word	0
	.word	1
	.word	1
	.word	2
	.word	3
	.word	0
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
