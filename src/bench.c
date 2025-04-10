#include <textio.h>
#include "chacha20.h"

#ifndef SEED
#define SEED 0xdeadbeef
#endif

#ifndef BENCH_LEN
#define BENCH_LEN 128
#endif

unsigned long rdcycle() {
#ifdef __riscv
    unsigned long res;
    asm volatile ("rdcycle %0" : "=r" (res));
    return res;
#else
    return 0;
#endif
}

#define BUF_LEN 1024

chacha_buf buf[BUF_LEN];

void gen_chacha_state(u32 *state, u32 seed) {
    u32 buf[16];
    for (int i = 0; i < 16; i++)
        buf[i] = seed ^ i;
    chacha20((chacha_buf *)state, buf);
}

void run_benchmark(u32 len, u32 seed) {
    u32 state[16 * DUP];
    gen_chacha_state(state, seed);
    print_s("Initial state generated for LEN=");
    print_long(len);
    print_s(" SEED=");
    print_hex32(seed);
    unsigned long start = rdcycle();

    // 原来的代码
    // for (int i = 0; i < len; i++) {
    //     chacha20(&buf[i%BUF_LEN], state);
    //     state[15] ++;
    // }

    for (int i = 1; i < DUP; i++) {
        for (int j = 0; j < 15; j++) {
            state[i * 16 + j] = state[j];
        }
        state[i * 16 + 15] = state[15] + i;
    }

    for (int i = 0; i < len; i += DUP) {
        chacha20_multi(&buf[i%BUF_LEN], state);
        for (int j = 0; j < DUP; j++) {
            state[j * 16 + 15] += DUP;
        }
    }

    unsigned long end = rdcycle();
    unsigned long cycles = end - start;
    print_s("Cycles: ");
    print_long(cycles);
    print_s("\n");
    print_s("Final output: \n");
    for (int i = 0; i < 16; i++) {
        print_s("ans[");
        print_long(i);
        print_s("] = ");
        print_hex32(buf[(len-1)%BUF_LEN].u[i]);
    }
}

int main() {
    print_s("Test Start!\n");
    run_benchmark(BENCH_LEN, SEED);
    return 0;
}
