/*
 * Copyright 2015-2020 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the Apache License 2.0 (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

/* Adapted from the public domain code by D. Bernstein from SUPERCOP. */

#include <chacha20.h>

#define PLAN3

#ifndef PLAN1
#define ROTATE(v, n) (((v) << (n)) | ((v) >> (32 - (n))))
#endif

#ifdef PLAN1
#define ROTATE(v, n) (rotate((v), (n)))
#endif

#ifdef PLAN1
inline u32 rotate(u32 value, u32 len) {
    u32 ret;
    asm("rolw %0,%1,%2" :"=r"(ret) :"r"(value), "r"(len) );
    return ret;
}
#endif


/* QUARTERROUND updates a, b, c, d with a ChaCha "quarter" round. */
# define MY_QUARTERROUND(a,b,c,d) ( \
x[a * DUP + i] += x[b * DUP + i], x[d * DUP + i] = ROTATE((x[d * DUP + i] ^ x[a * DUP + i]),16), \
x[c * DUP + i] += x[d * DUP + i], x[b * DUP + i] = ROTATE((x[b * DUP + i] ^ x[c * DUP + i]),12), \
x[a * DUP + i] += x[b * DUP + i], x[d * DUP + i] = ROTATE((x[d * DUP + i] ^ x[a * DUP + i]), 8), \
x[c * DUP + i] += x[d * DUP + i], x[b * DUP + i] = ROTATE((x[b * DUP + i] ^ x[c * DUP + i]), 7)  )

/*
ROTATE表示左滚动3格，可以使用rol指令实现
*/

#ifdef PLAN2
inline void fast_quarterround(u32 *x) {
    // QUARTERROUND(0, 4, 8, 12);
    // QUARTERROUND(1, 5, 9, 13);
    // QUARTERROUND(2, 6, 10, 14);
    // QUARTERROUND(3, 7, 11, 15);

    asm(
        "move t0, %[x]\n"
        "vle32.v v0, (t0)\n" // 32-bit unit-stride load, load x[a]
        "addi t0, t0, 16\n" // 增加128bit地址计数
        "vle32.v v1, (t0)\n" // load x[b]
        "addi t0, t0, 16\n"
        "vle32.v v2, (t0)\n" // load x[c]
        "addi t0, t0, 16\n"
        "vle32.v v3, (t0)\n" // load x[d]

        "vadd.vv v0, v0, v1\n" // x[a] += x[b]
        "vxor.vv v3, v3, v0\n"
        "li t0, 16\n"
        "vrol.vx v3, v3, t0\n" // x[d] = ROTATE((x[d] ^ x[a]),16)

        "vadd.vv v2, v2, v3\n" // x[c] += x[d]
        "vxor.vv v1, v1, v2\n"
        "li t0, 12\n"
        "vrol.vx v1, v1, t0\n" // x[b] = ROTATE((x[b] ^ x[c]),12)

        "vadd.vv v0, v0, v1\n" // x[a] += x[b]
        "vxor.vv v3, v3, v0\n"
        "li t0, 8\n"
        "vrol.vx v3, v3, t0\n" // x[d] = ROTATE((x[d] ^ x[a]), 8)

        "vadd.vv v2, v2, v3\n" // x[c] += x[d]
        "vxor.vv v1, v1, v2\n"
        "li t0, 7\n"
        "vrol.vx v1, v1, t0\n" // x[b] = ROTATE((x[b] ^ x[c]), 7)

        "move t0, %[x]\n"
        "vse32.v v0, (t0)\n" // Store x[a]
        "addi t0, t0, 16\n"
        "vse32.v v1, (t0)\n" // Store x[b]
        "addi t0, t0, 16\n"
        "vse32.v v2, (t0)\n" // Store x[c]
        "addi t0, t0, 16\n"
        "vse32.v v3, (t0)\n" // Store x[d]
        :
        : [x] "r"(x)
        : "t0", "memory", "v0", "v1", "v2", "v3"
    );
}
#endif

const u32 assign[4][4] = {
    {0, 1, 2, 3},
    {1, 2, 3, 0},
    {2, 3, 0, 1},
    {3, 0, 1, 2},
};

const u32 rev_assign[4][4] = {
    {0, 1, 2, 3},
    {3, 0, 1, 2},
    {2, 3, 0, 1},
    {1, 2, 3, 0},
};

#ifdef PLAN3
inline void full_quarterround(u32 *input, u32 *output) {
    // 循环10次
    // QUARTERROUND(0, 4, 8, 12);
    // QUARTERROUND(1, 5, 9, 13);
    // QUARTERROUND(2, 6, 10, 14);
    // QUARTERROUND(3, 7, 11, 15);

    // QUARTERROUND(0, 5, 10, 15);
    // QUARTERROUND(1, 6, 11, 12);
    // QUARTERROUND(2, 7, 8, 13);
    // QUARTERROUND(3, 4, 9, 14);

    asm(
        "move a0, %[input]\n" // a0 = &x[0], 数组地址
        "vle32.v v0, (a0)\n" // 32-bit unit-stride load, load x[a]
        "addi a0, a0, 16\n" // 增加128bit地址计数
        "vle32.v v1, (a0)\n" // load x[b]
        "addi a0, a0, 16\n"
        "vle32.v v2, (a0)\n" // load x[c]
        "addi a0, a0, 16\n"
        "vle32.v v3, (a0)\n" // load x[d]

        "vmv.v.v v16, v0\n" // 备份 x[a]
        "vmv.v.v v17, v1\n" // 备份 x[b]
        "vmv.v.v v18, v2\n" // 备份 x[c]
        "vmv.v.v v19, v3\n" // 备份 x[d]

        // 提前加载 ROTATE 的标量参数
        "li a0, 16\n"
        "li a1, 12\n"
        "li a2, 8\n"
        "li a3, 7\n"

        // 提前加载正向 gather 向量寄存器
        "vle32.v v4, (%[assign0])\n"
        "vle32.v v5, (%[assign1])\n"
        "vle32.v v6, (%[assign2])\n"
        "vle32.v v7, (%[assign3])\n"

        // 提前加载反向 gather 向量寄存器
        "vle32.v v8, (%[rassign0])\n"
        "vle32.v v9, (%[rassign1])\n"
        "vle32.v v10, (%[rassign2])\n"
        "vle32.v v11, (%[rassign3])\n"

        "li t0, 10\n" // Note: 循环
        "1: \n"
        
        // Part1: 前4个QUARTERROUND
        "vadd.vv v0, v0, v1\n" // x[a] += x[b]
        "vxor.vv v3, v3, v0\n"
        "vrol.vx v3, v3, a0\n" // x[d] = ROTATE((x[d] ^ x[a]),16)

        "vadd.vv v2, v2, v3\n" // x[c] += x[d]
        "vxor.vv v1, v1, v2\n"
        "vrol.vx v1, v1, a1\n" // x[b] = ROTATE((x[b] ^ x[c]),12)

        "vadd.vv v0, v0, v1\n" // x[a] += x[b]
        "vxor.vv v3, v3, v0\n"
        "vrol.vx v3, v3, a2\n" // x[d] = ROTATE((x[d] ^ x[a]), 8)

        "vadd.vv v2, v2, v3\n" // x[c] += x[d]
        "vxor.vv v1, v1, v2\n"
        "vrol.vx v1, v1, a3\n" // x[b] = ROTATE((x[b] ^ x[c]), 7)

        // 正向gather（源寄存器不可以与目的寄存器重叠）
        // TODO: 使用vslideup和vslidedown指令来实现vrgather
        "vrgather.vv v12, v0, v4\n"
        "vrgather.vv v13, v1, v5\n"
        "vrgather.vv v14, v2, v6\n"
        "vrgather.vv v15, v3, v7\n"

        // PART2: 后 4 个 QUARTERROUND
        // repeat
        "vadd.vv v12, v12, v13\n" // x[a] += x[b]
        "vxor.vv v15, v15, v12\n"
        "vrol.vx v15, v15, a0\n" // x[d] = ROTATE((x[d] ^ x[a]),16)

        "vadd.vv v14, v14, v15\n" // x[c] += x[d]
        "vxor.vv v13, v13, v14\n"
        "vrol.vx v13, v13, a1\n" // x[b] = ROTATE((x[b] ^ x[c]),12)

        "vadd.vv v12, v12, v13\n" // x[a] += x[b]
        "vxor.vv v15, v15, v12\n"
        "vrol.vx v15, v15, a2\n" // x[d] = ROTATE((x[d] ^ x[a]), 8)

        "vadd.vv v14, v14, v15\n" // x[c] += x[d]
        "vxor.vv v13, v13, v14\n"
        "vrol.vx v13, v13, a3\n" // x[b] = ROTATE((x[b] ^ x[c]), 7)

        // 反向 gather
        "vrgather.vv v0, v12, v8\n"
        "vrgather.vv v1, v13, v9\n"
        "vrgather.vv v2, v14, v10\n"
        "vrgather.vv v3, v15, v11\n"

        "addi t0, t0, -1\n" // t0--
        "bnez t0, 1b\n" // if (t0 != 0) goto loop
        // 结束循环

        "vadd.vv v0, v0, v16\n" // x[i] + input[i];
        "vadd.vv v1, v1, v17\n"
        "vadd.vv v2, v2, v18\n"
        "vadd.vv v3, v3, v19\n"

        "move a0, %[output]\n"
        "vse32.v v0, (a0)\n" // Store x[a]
        "addi a0, a0, 16\n"
        "vse32.v v1, (a0)\n" // Store x[b]
        "addi a0, a0, 16\n"
        "vse32.v v2, (a0)\n" // Store x[c]
        "addi a0, a0, 16\n"
        "vse32.v v3, (a0)\n" // Store x[d]
        
        :
        : [input] "r"(input), [output] "r"(output),
            [assign0] "r"(assign[0]), [assign1] "r"(assign[1]), [assign2] "r"(assign[2]), [assign3] "r"(assign[3]),
            [rassign0] "r"(rev_assign[0]), [rassign1] "r"(rev_assign[1]), [rassign2] "r"(rev_assign[2]), [rassign3] "r"(rev_assign[3])
        : "t0", "a0", "a1", "a2", "a3", 
            "memory"
    );
    // TODO: 6次chacha20并行，提高可并行发射的指令数，提高IPC
    // https://zhuanlan.zhihu.com/p/14912428140
}
#endif

/* chacha_core performs 20 rounds of ChaCha on the input words in
 * |input| and writes the 64 output bytes to |output|. */
void chacha20(chacha_buf *output, const u32 *input)
{
    #if defined(PLAN2) || defined(PLAN3)
        u32 len = 4, actual_len;
        asm(
            "vsetvli %0, %1, e32"
            : "=r"(actual_len)
            : "r"(len)
        );
        if (actual_len != 4) {
            // 出错
            return;
        }
    #endif

    #ifndef PLAN3
        u32 x[16];
        int i;

        for (int i = 0; i < 16; i++) {
            x[i] = input[i];
        }

        // 运行10次
        for (i = 20; i > 0; i -= 2) {
            #ifdef PLAN2
                fast_quarterround(x);
            #else
                QUARTERROUND(0, 4, 8, 12);
                QUARTERROUND(1, 5, 9, 13);
                QUARTERROUND(2, 6, 10, 14);
                QUARTERROUND(3, 7, 11, 15);
            #endif

            QUARTERROUND(0, 5, 10, 15);
            QUARTERROUND(1, 6, 11, 12);
            QUARTERROUND(2, 7, 8, 13);
            QUARTERROUND(3, 4, 9, 14);
        }

        for (i = 0; i < 16; ++i)
            output->u[i] = x[i] + input[i];
    #else
        full_quarterround(input, output->u);
    #endif
}

#include <textio.h>
// void chacha20_multi(chacha_buf *output, const u32 *input)
// {
//     for (int i = 0; i < DUP; i++) {
//         chacha20(&output[i], &input[i * 16]);
//     }

//     // for test
//     print_s("chacha20_multi output:\n");
//     for (int i = 0; i < DUP; i++) {
//         for (int j = 0; j < 16; j++) {
//             print_hex32(output[i].u[j]);
//         }
//     }
// }

void chacha20_multi_asm(u32 *output, const u32 *input) {
    // 与下面的汇编等价的写法
    // for (int i = 0; i < 16 * DUP; i++) {
    //     output[i] = input[i];
    // }
    // u32 *x = output;
    // for (int i = 0; i < DUP; i++) {
    //     for (int j = 20; j > 0; j -= 2) {
    //         MY_QUARTERROUND(0, 4, 8, 12);
    //         MY_QUARTERROUND(1, 5, 9, 13);
    //         MY_QUARTERROUND(2, 6, 10, 14);
    //         MY_QUARTERROUND(3, 7, 11, 15);

    //         MY_QUARTERROUND(0, 5, 10, 15);
    //         MY_QUARTERROUND(1, 6, 11, 12);
    //         MY_QUARTERROUND(2, 7, 8, 13);
    //         MY_QUARTERROUND(3, 4, 9, 14);
    //     }
    // }
    // return;

    u32 len = 8, actual_len;
    asm(
        "vsetvli %0, %1, e32, m2" // lmul=2, 2个vector寄存器合并为一个
        : "=r"(actual_len)
        : "r"(len)
    );
    if (actual_len != 8) {
        // 出错
        return;
    }
    
    asm(
        // 加载输入
        "move a0, %[input]\n" // a0 = &x[0], 数组地址
        "vle32.v v0, (a0)\n" // 64-bit unit-stride load, load x[a]
        "addi a0, a0, 32\n" // 增加256bit地址计数
        "vle32.v v2, (a0)\n"
        "addi a0, a0, 32\n"
        "vle32.v v4, (a0)\n"
        "addi a0, a0, 32\n"
        "vle32.v v6, (a0)\n"
        "addi a0, a0, 32\n"
        "vle32.v v8, (a0)\n"
        "addi a0, a0, 32\n"
        "vle32.v v10, (a0)\n"
        "addi a0, a0, 32\n"
        "vle32.v v12, (a0)\n"
        "addi a0, a0, 32\n"
        "vle32.v v14, (a0)\n"
        "addi a0, a0, 32\n"
        "vle32.v v16, (a0)\n"
        "addi a0, a0, 32\n"
        "vle32.v v18, (a0)\n"
        "addi a0, a0, 32\n"
        "vle32.v v20, (a0)\n"
        "addi a0, a0, 32\n"
        "vle32.v v22, (a0)\n"
        "addi a0, a0, 32\n"
        "vle32.v v24, (a0)\n"
        "addi a0, a0, 32\n"
        "vle32.v v26, (a0)\n"
        "addi a0, a0, 32\n"
        "vle32.v v28, (a0)\n"
        "addi a0, a0, 32\n"
        "vle32.v v30, (a0)\n"
        "addi a0, a0, 32\n"

        // 提前加载 ROTATE 的标量参数
        "li a0, 16\n"
        "li a1, 12\n"
        "li a2, 8\n"
        "li a3, 7\n"

        "li t0, 10\n" // Note: 循环
        "1: \n"
        
        // 第一个QUARTERROUND
        // # define QUARTERROUND(a,b,c,d) ( \
        // x[a] += x[b], x[d] = ROTATE((x[d] ^ x[a]),16), \
        // x[c] += x[d], x[b] = ROTATE((x[b] ^ x[c]),12), \
        // x[a] += x[b], x[d] = ROTATE((x[d] ^ x[a]), 8), \
        // x[c] += x[d], x[b] = ROTATE((x[b] ^ x[c]), 7)  )

        "vadd.vv v0, v0, v8\n" // x[a] += x[b]
        "vxor.vv v24, v24, v0\n" // x[d] ^= x[a]
        "vrol.vx v24, v24, a0\n" // x[d] = ROTATE((x[d] ^ x[a]),16)
        "vadd.vv v16, v16, v24\n" // x[c] += x[d]
        "vxor.vv v8, v8, v16\n" // x[b] ^= x[c]
        "vrol.vx v8, v8, a1\n" // x[b] = ROTATE((x[b] ^ x[c]),12)
        "vadd.vv v0, v0, v8\n" // x[a] += x[b]
        "vxor.vv v24, v24, v0\n" // x[d] ^= x[a]
        "vrol.vx v24, v24, a2\n" // x[d] = ROTATE((x[d] ^ x[a]), 8)
        "vadd.vv v16, v16, v24\n" // x[c] += x[d]
        "vxor.vv v8, v8, v16\n" // x[b] ^= x[c]
        "vrol.vx v8, v8, a3\n" // x[b] = ROTATE((x[b] ^ x[c]), 7)

        // 第二个QUARTERROUND
        // QUARTERROUND(1,5,9,13) -> a=1(v2), b=5(v10), c=9(v18), d=13(v26)
        "vadd.vv v2, v2, v10\n"      // x[a] += x[b]
        "vxor.vv v26, v26, v2\n"     // x[d] ^= x[a]
        "vrol.vx v26, v26, a0\n"     // ROTATE(16)
        "vadd.vv v18, v18, v26\n"    // x[c] += x[d]
        "vxor.vv v10, v10, v18\n"    // x[b] ^= x[c]
        "vrol.vx v10, v10, a1\n"     // ROTATE(12)
        "vadd.vv v2, v2, v10\n"      // x[a] += x[b]
        "vxor.vv v26, v26, v2\n"     // x[d] ^= x[a]
        "vrol.vx v26, v26, a2\n"     // ROTATE(8)
        "vadd.vv v18, v18, v26\n"    // x[c] += x[d]
        "vxor.vv v10, v10, v18\n"    // x[b] ^= x[c]
        "vrol.vx v10, v10, a3\n"     // ROTATE(7)

        // QUARTERROUND(2,6,10,14) -> a=2(v4), b=6(v12), c=10(v20), d=14(v28)
        "vadd.vv v4, v4, v12\n"      // x[a] += x[b]
        "vxor.vv v28, v28, v4\n"     // x[d] ^= x[a]
        "vrol.vx v28, v28, a0\n"     // ROTATE(16)
        "vadd.vv v20, v20, v28\n"    // x[c] += x[d]
        "vxor.vv v12, v12, v20\n"    // x[b] ^= x[c]
        "vrol.vx v12, v12, a1\n"     // ROTATE(12)
        "vadd.vv v4, v4, v12\n"      // x[a] += x[b]
        "vxor.vv v28, v28, v4\n"     // x[d] ^= x[a]
        "vrol.vx v28, v28, a2\n"     // ROTATE(8)
        "vadd.vv v20, v20, v28\n"    // x[c] += x[d]
        "vxor.vv v12, v12, v20\n"    // x[b] ^= x[c]
        "vrol.vx v12, v12, a3\n"     // ROTATE(7)

        // QUARTERROUND(3,7,11,15) -> a=3(v6), b=7(v14), c=11(v22), d=15(v30)
        "vadd.vv v6, v6, v14\n"      // x[a] += x[b]
        "vxor.vv v30, v30, v6\n"     // x[d] ^= x[a]
        "vrol.vx v30, v30, a0\n"     // ROTATE(16)
        "vadd.vv v22, v22, v30\n"    // x[c] += x[d]
        "vxor.vv v14, v14, v22\n"    // x[b] ^= x[c]
        "vrol.vx v14, v14, a1\n"     // ROTATE(12)
        "vadd.vv v6, v6, v14\n"      // x[a] += x[b]
        "vxor.vv v30, v30, v6\n"     // x[d] ^= x[a]
        "vrol.vx v30, v30, a2\n"     // ROTATE(8)
        "vadd.vv v22, v22, v30\n"    // x[c] += x[d]
        "vxor.vv v14, v14, v22\n"    // x[b] ^= x[c]
        "vrol.vx v14, v14, a3\n"     // ROTATE(7)

        //. QUARTERROUND(0,5,10,15) -> a=0(v0), b=5(v10), c=10(v20), d=15(v30)
        "vadd.vv v0, v0, v10\n"      // x[a] += x[b]
        "vxor.vv v30, v30, v0\n"     // x[d] ^= x[a]
        "vrol.vx v30, v30, a0\n"     // ROTATE(16)
        "vadd.vv v20, v20, v30\n"    // x[c] += x[d]
        "vxor.vv v10, v10, v20\n"    // x[b] ^= x[c]
        "vrol.vx v10, v10, a1\n"     // ROTATE(12)
        "vadd.vv v0, v0, v10\n"      // x[a] += x[b]
        "vxor.vv v30, v30, v0\n"     // x[d] ^= x[a]
        "vrol.vx v30, v30, a2\n"     // ROTATE(8)
        "vadd.vv v20, v20, v30\n"    // x[c] += x[d]
        "vxor.vv v10, v10, v20\n"    // x[b] ^= x[c]
        "vrol.vx v10, v10, a3\n"     // ROTATE(7)

        //. QUARTERROUND(1,6,11,12) -> a=1(v2), b=6(v12), c=11(v22), d=12(v24)
        "vadd.vv v2, v2, v12\n"      // x[a] += x[b]
        "vxor.vv v24, v24, v2\n"     // x[d] ^= x[a]
        "vrol.vx v24, v24, a0\n"     // ROTATE(16)
        "vadd.vv v22, v22, v24\n"    // x[c] += x[d]
        "vxor.vv v12, v12, v22\n"    // x[b] ^= x[c]
        "vrol.vx v12, v12, a1\n"     // ROTATE(12)
        "vadd.vv v2, v2, v12\n"      // x[a] += x[b]
        "vxor.vv v24, v24, v2\n"     // x[d] ^= x[a]
        "vrol.vx v24, v24, a2\n"     // ROTATE(8)
        "vadd.vv v22, v22, v24\n"    // x[c] += x[d]
        "vxor.vv v12, v12, v22\n"    // x[b] ^= x[c]
        "vrol.vx v12, v12, a3\n"     // ROTATE(7)

        //. QUARTERROUND(2,7,8,13) -> a=2(v4), b=7(v14), c=8(v16), d=13(v26)
        "vadd.vv v4, v4, v14\n"      // x[a] += x[b]
        "vxor.vv v26, v26, v4\n"     // x[d] ^= x[a]
        "vrol.vx v26, v26, a0\n"     // ROTATE(16)
        "vadd.vv v16, v16, v26\n"    // x[c] += x[d]
        "vxor.vv v14, v14, v16\n"    // x[b] ^= x[c]
        "vrol.vx v14, v14, a1\n"     // ROTATE(12)
        "vadd.vv v4, v4, v14\n"      // x[a] += x[b]
        "vxor.vv v26, v26, v4\n"     // x[d] ^= x[a]
        "vrol.vx v26, v26, a2\n"     // ROTATE(8)
        "vadd.vv v16, v16, v26\n"    // x[c] += x[d]
        "vxor.vv v14, v14, v16\n"    // x[b] ^= x[c]
        "vrol.vx v14, v14, a3\n"     // ROTATE(7)

        //. QUARTERROUND(3,4,9,14) -> a=3(v6), b=4(v8), c=9(v18), d=14(v28)
        "vadd.vv v6, v6, v8\n"       // x[a] += x[b]
        "vxor.vv v28, v28, v6\n"     // x[d] ^= x[a]
        "vrol.vx v28, v28, a0\n"     // ROTATE(16)
        "vadd.vv v18, v18, v28\n"    // x[c] += x[d]
        "vxor.vv v8, v8, v18\n"      // x[b] ^= x[c]
        "vrol.vx v8, v8, a1\n"       // ROTATE(12)
        "vadd.vv v6, v6, v8\n"       // x[a] += x[b]
        "vxor.vv v28, v28, v6\n"     // x[d] ^= x[a]
        "vrol.vx v28, v28, a2\n"     // ROTATE(8)
        "vadd.vv v18, v18, v28\n"    // x[c] += x[d]
        "vxor.vv v8, v8, v18\n"      // x[b] ^= x[c]
        "vrol.vx v8, v8, a3\n"       // ROTATE(7)

        "addi t0, t0, -1\n" // t0--
        "bnez t0, 1b\n" // if (t0 != 0) goto loop
        // 结束循环

        "move a0, %[output]\n"
        "vse32.v v0, (a0)\n" // 64-bit unit-stride load, load x[a]
        "addi a0, a0, 32\n" // 增加256bit地址计数
        "vse32.v v2, (a0)\n"
        "addi a0, a0, 32\n"
        "vse32.v v4, (a0)\n"
        "addi a0, a0, 32\n"
        "vse32.v v6, (a0)\n"
        "addi a0, a0, 32\n"
        "vse32.v v8, (a0)\n"
        "addi a0, a0, 32\n"
        "vse32.v v10, (a0)\n"
        "addi a0, a0, 32\n"
        "vse32.v v12, (a0)\n"
        "addi a0, a0, 32\n"
        "vse32.v v14, (a0)\n"
        "addi a0, a0, 32\n"
        "vse32.v v16, (a0)\n"
        "addi a0, a0, 32\n"
        "vse32.v v18, (a0)\n"
        "addi a0, a0, 32\n"
        "vse32.v v20, (a0)\n"
        "addi a0, a0, 32\n"
        "vse32.v v22, (a0)\n"
        "addi a0, a0, 32\n"
        "vse32.v v24, (a0)\n"
        "addi a0, a0, 32\n"
        "vse32.v v26, (a0)\n"
        "addi a0, a0, 32\n"
        "vse32.v v28, (a0)\n"
        "addi a0, a0, 32\n"
        "vse32.v v30, (a0)\n"
        "addi a0, a0, 32\n"
        :
        : [input] "r"(input), [output] "r"(output)
        : "memory", "a0", "a1", "a2", "a3", "t0", "v0", "v2", "v4", "v6", "v8", "v10", "v12", "v14", "v16", "v18", "v20", "v22", "v24", "v26", "v28", "v30"
    );
}

void chacha20_multi(chacha_buf *output, const u32 *input)
{
    // 重排
    u32 x[16 * DUP];
    u32 my_output[16 * DUP];
    for (int i = 0; i < 16 * DUP; i++) {
        // 组号: i % DUP
        // 位置：i / DUP
        x[i] = input[(i % DUP) * 16 + i / DUP];
    }
    chacha20_multi_asm(my_output, x);
    // 还原
    for (int i = 0; i < 16 * DUP; i++) {
        // 组号: i % DUP
        // 位置：i / DUP
        output[i % DUP].u[i / DUP] = my_output[i] + x[i];
    }
}


