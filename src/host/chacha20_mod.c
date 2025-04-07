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
# define QUARTERROUND(a,b,c,d) ( \
x[a] += x[b], x[d] = ROTATE((x[d] ^ x[a]),16), \
x[c] += x[d], x[b] = ROTATE((x[b] ^ x[c]),12), \
x[a] += x[b], x[d] = ROTATE((x[d] ^ x[a]), 8), \
x[c] += x[d], x[b] = ROTATE((x[b] ^ x[c]), 7)  )

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
        "loop: \n"
        
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
        "bnez t0, loop\n" // if (t0 != 0) goto loop
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
}
#endif

/* chacha_core performs 20 rounds of ChaCha on the input words in
 * |input| and writes the 64 output bytes to |output|. */
void chacha20(chacha_buf *output, const u32 input[16])
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
