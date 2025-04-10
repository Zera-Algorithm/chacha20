	.file	"chacha20_mod.c"
	.option pic
	.attribute arch, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_v1p0_zicsr2p0_zifencei2p0_zbb1p0_zvbb_zve32f1p0_zve32x1p0_zve64d1p0_zve64f1p0_zve64x1p0_zvl128b1p0_zvl32b1p0_zvl64b1p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
# GNU C17 (Ubuntu 13.3.0-6ubuntu2~24.04) version 13.3.0 (riscv64-linux-gnu)
#	compiled by GNU C version 13.3.0, GMP version 6.3.0, MPFR version 4.2.1, MPC version 1.3.1, isl version isl-0.26-GMP

# GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
# options passed: -mabi=lp64d -misa-spec=20191213 -march=rv64imafdcv_zicsr_zifencei_zbb_zvbb_zve32f_zve32x_zve64d_zve64f_zve64x_zvl128b_zvl32b_zvl64b -O3 -O3 -fno-builtin
	.text
	.align	1
	.type	chacha20_multi_asm.part.0, @function
chacha20_multi_asm.part.0:
.LFB4:
	.cfi_startproc
# src/host/chacha20_mod.c:302: void chacha20_multi_asm(u32 *output, const u32 *input) {
	mv	a4,a0	# tmp136, output
	mv	a5,a1	# tmp137, input
# src/host/chacha20_mod.c:334:     asm(
#APP
# 334 "src/host/chacha20_mod.c" 1
	move a0, a5	# tmp137
vle32.v v0, (a0)
addi a0, a0, 32
vle32.v v2, (a0)
addi a0, a0, 32
vle32.v v4, (a0)
addi a0, a0, 32
vle32.v v6, (a0)
addi a0, a0, 32
vle32.v v8, (a0)
addi a0, a0, 32
vle32.v v10, (a0)
addi a0, a0, 32
vle32.v v12, (a0)
addi a0, a0, 32
vle32.v v14, (a0)
addi a0, a0, 32
vle32.v v16, (a0)
addi a0, a0, 32
vle32.v v18, (a0)
addi a0, a0, 32
vle32.v v20, (a0)
addi a0, a0, 32
vle32.v v22, (a0)
addi a0, a0, 32
vle32.v v24, (a0)
addi a0, a0, 32
vle32.v v26, (a0)
addi a0, a0, 32
vle32.v v28, (a0)
addi a0, a0, 32
vle32.v v30, (a0)
addi a0, a0, 32
li a0, 16
li a1, 12
li a2, 8
li a3, 7
li t0, 10
1: 
vadd.vv v0, v0, v8
vxor.vv v24, v24, v0
vrol.vx v24, v24, a0
vadd.vv v16, v16, v24
vxor.vv v8, v8, v16
vrol.vx v8, v8, a1
vadd.vv v0, v0, v8
vxor.vv v24, v24, v0
vrol.vx v24, v24, a2
vadd.vv v16, v16, v24
vxor.vv v8, v8, v16
vrol.vx v8, v8, a3
vadd.vv v2, v2, v10
vxor.vv v26, v26, v2
vrol.vx v26, v26, a0
vadd.vv v18, v18, v26
vxor.vv v10, v10, v18
vrol.vx v10, v10, a1
vadd.vv v2, v2, v10
vxor.vv v26, v26, v2
vrol.vx v26, v26, a2
vadd.vv v18, v18, v26
vxor.vv v10, v10, v18
vrol.vx v10, v10, a3
vadd.vv v4, v4, v12
vxor.vv v28, v28, v4
vrol.vx v28, v28, a0
vadd.vv v20, v20, v28
vxor.vv v12, v12, v20
vrol.vx v12, v12, a1
vadd.vv v4, v4, v12
vxor.vv v28, v28, v4
vrol.vx v28, v28, a2
vadd.vv v20, v20, v28
vxor.vv v12, v12, v20
vrol.vx v12, v12, a3
vadd.vv v6, v6, v14
vxor.vv v30, v30, v6
vrol.vx v30, v30, a0
vadd.vv v22, v22, v30
vxor.vv v14, v14, v22
vrol.vx v14, v14, a1
vadd.vv v6, v6, v14
vxor.vv v30, v30, v6
vrol.vx v30, v30, a2
vadd.vv v22, v22, v30
vxor.vv v14, v14, v22
vrol.vx v14, v14, a3
vadd.vv v0, v0, v10
vxor.vv v30, v30, v0
vrol.vx v30, v30, a0
vadd.vv v20, v20, v30
vxor.vv v10, v10, v20
vrol.vx v10, v10, a1
vadd.vv v0, v0, v10
vxor.vv v30, v30, v0
vrol.vx v30, v30, a2
vadd.vv v20, v20, v30
vxor.vv v10, v10, v20
vrol.vx v10, v10, a3
vadd.vv v2, v2, v12
vxor.vv v24, v24, v2
vrol.vx v24, v24, a0
vadd.vv v22, v22, v24
vxor.vv v12, v12, v22
vrol.vx v12, v12, a1
vadd.vv v2, v2, v12
vxor.vv v24, v24, v2
vrol.vx v24, v24, a2
vadd.vv v22, v22, v24
vxor.vv v12, v12, v22
vrol.vx v12, v12, a3
vadd.vv v4, v4, v14
vxor.vv v26, v26, v4
vrol.vx v26, v26, a0
vadd.vv v16, v16, v26
vxor.vv v14, v14, v16
vrol.vx v14, v14, a1
vadd.vv v4, v4, v14
vxor.vv v26, v26, v4
vrol.vx v26, v26, a2
vadd.vv v16, v16, v26
vxor.vv v14, v14, v16
vrol.vx v14, v14, a3
vadd.vv v6, v6, v8
vxor.vv v28, v28, v6
vrol.vx v28, v28, a0
vadd.vv v18, v18, v28
vxor.vv v8, v8, v18
vrol.vx v8, v8, a1
vadd.vv v6, v6, v8
vxor.vv v28, v28, v6
vrol.vx v28, v28, a2
vadd.vv v18, v18, v28
vxor.vv v8, v8, v18
vrol.vx v8, v8, a3
addi t0, t0, -1
bnez t0, 1b
move a0, a4	# tmp136
vse32.v v0, (a0)
addi a0, a0, 32
vse32.v v2, (a0)
addi a0, a0, 32
vse32.v v4, (a0)
addi a0, a0, 32
vse32.v v6, (a0)
addi a0, a0, 32
vse32.v v8, (a0)
addi a0, a0, 32
vse32.v v10, (a0)
addi a0, a0, 32
vse32.v v12, (a0)
addi a0, a0, 32
vse32.v v14, (a0)
addi a0, a0, 32
vse32.v v16, (a0)
addi a0, a0, 32
vse32.v v18, (a0)
addi a0, a0, 32
vse32.v v20, (a0)
addi a0, a0, 32
vse32.v v22, (a0)
addi a0, a0, 32
vse32.v v24, (a0)
addi a0, a0, 32
vse32.v v26, (a0)
addi a0, a0, 32
vse32.v v28, (a0)
addi a0, a0, 32
vse32.v v30, (a0)
addi a0, a0, 32

# 0 "" 2
# src/host/chacha20_mod.c:539: }
#NO_APP
	ret	
	.cfi_endproc
.LFE4:
	.size	chacha20_multi_asm.part.0, .-chacha20_multi_asm.part.0
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
	beq	a5,a3,.L5	#, actual_len, tmp141,
# src/host/chacha20_mod.c:284: }
	ret	
.L5:
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
1: 
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
bnez t0, 1b
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
# src/host/chacha20_mod.c:284: }
#NO_APP
	ret	
	.cfi_endproc
.LFE1:
	.size	chacha20, .-chacha20
	.align	1
	.globl	chacha20_multi_asm
	.type	chacha20_multi_asm, @function
chacha20_multi_asm:
.LFB2:
	.cfi_startproc
# src/host/chacha20_mod.c:324:     asm(
	li	a5,8		# tmp138,
# src/host/chacha20_mod.c:329:     if (actual_len != 8) {
	li	a4,8		# tmp140,
# src/host/chacha20_mod.c:324:     asm(
#APP
# 324 "src/host/chacha20_mod.c" 1
	vsetvli a5, a5, e32, m2	# actual_len, tmp138
# 0 "" 2
#NO_APP
	sext.w	a5,a5	# actual_len, actual_len
# src/host/chacha20_mod.c:329:     if (actual_len != 8) {
	bne	a5,a4,.L6	#, actual_len, tmp140,
	tail	chacha20_multi_asm.part.0		#
.L6:
# src/host/chacha20_mod.c:539: }
	ret	
	.cfi_endproc
.LFE2:
	.size	chacha20_multi_asm, .-chacha20_multi_asm
	.align	1
	.globl	chacha20_multi
	.type	chacha20_multi, @function
chacha20_multi:
.LFB3:
	.cfi_startproc
	addi	sp,sp,-1056	#,,
	.cfi_def_cfa_offset 1056
	mv	a6,sp	# tmp194,
	sd	s1,1032(sp)	#,
	sd	s2,1024(sp)	#,
	sd	ra,1048(sp)	#,
	.cfi_offset 9, -24
	.cfi_offset 18, -32
	.cfi_offset 1, -8
	mv	s2,a0	# output, tmp196
	sd	s0,1040(sp)	#,
	.cfi_offset 8, -16
	mv	s1,a6	# ivtmp.24, tmp194
# src/host/chacha20_mod.c:542: {
	mv	a3,a6	# ivtmp.33, tmp194
# src/host/chacha20_mod.c:546:     for (int i = 0; i < 16 * DUP; i++) {
	li	a4,0		# i,
# src/host/chacha20_mod.c:546:     for (int i = 0; i < 16 * DUP; i++) {
	li	a0,128		# tmp171,
.L9:
# src/host/chacha20_mod.c:549:         x[i] = input[(i % DUP) * 16 + i / DUP];
	andi	a5,a4,7	#, tmp161, i
# src/host/chacha20_mod.c:549:         x[i] = input[(i % DUP) * 16 + i / DUP];
	sraiw	a2,a4,3	#, tmp163, i
# src/host/chacha20_mod.c:549:         x[i] = input[(i % DUP) * 16 + i / DUP];
	slliw	a5,a5,4	#, tmp162, tmp161
# src/host/chacha20_mod.c:549:         x[i] = input[(i % DUP) * 16 + i / DUP];
	addw	a5,a5,a2	# tmp163, tmp165, tmp162
# src/host/chacha20_mod.c:549:         x[i] = input[(i % DUP) * 16 + i / DUP];
	slli	a5,a5,2	#, tmp166, tmp165
	add	a5,a1,a5	# tmp166, tmp167, input
# src/host/chacha20_mod.c:549:         x[i] = input[(i % DUP) * 16 + i / DUP];
	lw	a5,0(a5)		# *_7, *_7
# src/host/chacha20_mod.c:546:     for (int i = 0; i < 16 * DUP; i++) {
	addiw	a4,a4,1	#, i, i
# src/host/chacha20_mod.c:546:     for (int i = 0; i < 16 * DUP; i++) {
	addi	a3,a3,4	#, ivtmp.33, ivtmp.33
# src/host/chacha20_mod.c:549:         x[i] = input[(i % DUP) * 16 + i / DUP];
	sw	a5,-4(a3)	# *_7, MEM[(unsigned int *)_46]
# src/host/chacha20_mod.c:546:     for (int i = 0; i < 16 * DUP; i++) {
	bne	a4,a0,.L9	#, i, tmp171,
# src/host/chacha20_mod.c:324:     asm(
	li	a5,8		# tmp173,
# src/host/chacha20_mod.c:329:     if (actual_len != 8) {
	li	a4,8		# tmp175,
# src/host/chacha20_mod.c:324:     asm(
#APP
# 324 "src/host/chacha20_mod.c" 1
	vsetvli a5, a5, e32, m2	# actual_len, tmp173
# 0 "" 2
#NO_APP
	sext.w	a5,a5	# actual_len, actual_len
	addi	s0,sp,512	#, ivtmp.23,
# src/host/chacha20_mod.c:329:     if (actual_len != 8) {
	bne	a5,a4,.L10	#, actual_len, tmp175,
	mv	a1,a6	#, tmp194
	mv	a0,s0	#, ivtmp.23
	call	chacha20_multi_asm.part.0		#
.L10:
	mv	a1,s0	# ivtmp.23, ivtmp.23
# src/host/chacha20_mod.c:546:     for (int i = 0; i < 16 * DUP; i++) {
	li	a4,0		# i,
# src/host/chacha20_mod.c:553:     for (int i = 0; i < 16 * DUP; i++) {
	li	a0,128		# tmp193,
.L11:
# src/host/chacha20_mod.c:556:         output[i % DUP].u[i / DUP] = my_output[i] + x[i];
	lw	a6,0(a1)		# MEM[(unsigned int *)_51], MEM[(unsigned int *)_51]
	lw	a3,0(s1)		# MEM[(unsigned int *)_50], MEM[(unsigned int *)_50]
# src/host/chacha20_mod.c:556:         output[i % DUP].u[i / DUP] = my_output[i] + x[i];
	andi	a5,a4,7	#, tmp179, i
# src/host/chacha20_mod.c:556:         output[i % DUP].u[i / DUP] = my_output[i] + x[i];
	srai	a2,a4,3	#, tmp183, i
	slli	a5,a5,6	#, tmp180, tmp179
	add	a5,s2,a5	# tmp180, tmp181, output
	slli	a2,a2,2	#, tmp184, tmp183
	add	a5,a5,a2	# tmp184, tmp185, tmp181
# src/host/chacha20_mod.c:556:         output[i % DUP].u[i / DUP] = my_output[i] + x[i];
	addw	a3,a3,a6	# MEM[(unsigned int *)_51], tmp186, MEM[(unsigned int *)_50]
# src/host/chacha20_mod.c:556:         output[i % DUP].u[i / DUP] = my_output[i] + x[i];
	sw	a3,0(a5)	# tmp186, _14->u[_15]
# src/host/chacha20_mod.c:553:     for (int i = 0; i < 16 * DUP; i++) {
	addiw	a4,a4,1	#, i, i
# src/host/chacha20_mod.c:553:     for (int i = 0; i < 16 * DUP; i++) {
	addi	a1,a1,4	#, ivtmp.23, ivtmp.23
	addi	s1,s1,4	#, ivtmp.24, ivtmp.24
	bne	a4,a0,.L11	#, i, tmp193,
# src/host/chacha20_mod.c:558: }
	ld	ra,1048(sp)		#,
	.cfi_restore 1
	ld	s0,1040(sp)		#,
	.cfi_restore 8
	ld	s1,1032(sp)		#,
	.cfi_restore 9
	ld	s2,1024(sp)		#,
	.cfi_restore 18
	addi	sp,sp,1056	#,,
	.cfi_def_cfa_offset 0
	jr	ra		#
	.cfi_endproc
.LFE3:
	.size	chacha20_multi, .-chacha20_multi
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
