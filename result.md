# 不同优化方法对性能（周期数）的影响

## Native
根据文档，周期为143409。
```
$ make run
./xs-emu -i ./chacha20_baremetal-256.bin --no-diff 2>/dev/null
xs-emu compiled at Mar 28 2025, 11:26:20
Using simulated 32768B flash
Using simulated 8386560MB RAM
The image is ./chacha20_baremetal-256.bin
Test Start!
Initial state generated for LEN=256 SEED=0xdeadbeef
Cycles: 143409
Final output: 
ans[0] = 0xbe64d5a5
ans[1] = 0x83c6c3fa
ans[2] = 0xac710378
ans[3] = 0x858f0082
ans[4] = 0x207f2dc2
ans[5] = 0xeb5ee49e
ans[6] = 0x4f801257
ans[7] = 0x2e4f03c9
ans[8] = 0x74b07deb
ans[9] = 0x3e6dde54
ans[10] = 0x6dd3ddad
ans[11] = 0xee18cf6c
ans[12] = 0x053765e6
ans[13] = 0x79f814eb
ans[14] = 0x40b18db8
ans[15] = 0xa299c057
Core 0: HIT GOOD TRAP at pc = 0x80010002
Core-0 instrCnt = 452,265, cycleCnt = 171,941, IPC = 2.630350
Seed=0 Guest cycle spent: 171,945 (this will be different from cycleCnt if emu loads a snapshot)
Host time spent: 16,376ms
```

自己实测周期为148671，相去不远。

## 使用rolw翻转指令

```
inline u32 rotate(u32 value, u32 len) {
    u32 ret;
    asm("rolw %0,%1,%2" :"=r"(ret) :"r"(value), "r"(len) );
    return ret;
}
```
周期数变为97142。

```
riscv64-linux-gnu-gcc -O3 -Ilib -Isrc -fno-builtin -static -march=rv64gc_zbb -mcmodel=medany -nostdlib -T lib/linker.ld -o chacha20_baremetal-256 lib/am_start.S src/bench-256.o src/chacha20.o lib/textio.o lib/uart_uartlite.o
riscv64-linux-gnu-objcopy -O binary chacha20_baremetal-256 chacha20_baremetal-256.bin
./xs-emu -i ./chacha20_baremetal-256.bin --no-diff 2>/dev/null
xs-emu compiled at Mar 28 2025, 11:26:20
[WARNING] /mnt/data/xs/xs-env/XiangShan/build/constantin.txt does not exist, so all constants default to initialized values.
Using simulated 32768B flash
Using simulated 8386560MB RAM
The image is ./chacha20_baremetal-256.bin
Test Start!
Initial state generated for LEN=256 SEED=0xdeadbeef
Cycles: 97142
Final output: 
ans[0] = 0xbe64d5a5
ans[1] = 0x83c6c3fa
ans[2] = 0xac710378
ans[3] = 0x858f0082
ans[4] = 0x207f2dc2
ans[5] = 0xeb5ee49e
ans[6] = 0x4f801257
ans[7] = 0x2e4f03c9
ans[8] = 0x74b07deb
ans[9] = 0x3e6dde54
ans[10] = 0x6dd3ddad
ans[11] = 0xee18cf6c
ans[12] = 0x053765e6
ans[13] = 0x79f814eb
ans[14] = 0x40b18db8
ans[15] = 0xa299c057
Core 0: HIT GOOD TRAP at pc = 0x80010002
Core-0 instrCnt = 307776, cycleCnt = 125450, IPC = 2.453376
Seed=0 Guest cycle spent: 125454 (this will be different from cycleCnt if emu loads a snapshot)
Host time spent: 24560ms
```


## 使用vector指令

香山昆明湖的VLEN向量宽度为128。
ref: https://cloud.tencent.com/developer/article/2448576

```
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
```

发现这样做，周期数会暴涨到371016。分析发现，x数组中只有16个元素。如果使用上述代码，每次都需要从内存中读取值，访存效率很低。而原本的代码只需要做寄存器操作，无需做访存操作，所以效率很高。

忽略正确性，如果把每段汇编前面的`vle32.v`和后面的`vse32.v`去掉，周期数回落到了189754，但还是比原来高很多。推测是因为做了"memory"破坏性标记，之后的`x`数组中的数据还需要去访存取回，增加了访存开销。

## 全vector运算

Vector Register Gather Instructions: 重新组织vector寄存器中的数据

```asm
vrgather.vv vd, vs2, vs1, vm     # vd[i] = (vs1[i] >= VLMAX) ? 0 : vs2[vs1[i]];
```

## 使用lmul

lmul测试代码：
```c
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

// 用vle32.v, vadd, vse32.v指令测试lmul是否生效
u32 t[8] = {1, 2, 3, 4, 5, 6, 7, 8};
asm(
    "move a0, %[input]\n" // a0 = &x[0], 数组地址
    "vle32.v v0, (a0)\n" // 32-bit unit-stride load, load x[a]
    "vadd.vv v2, v0, v0\n" // x[a] += x[b]
    "move a0, %[output]\n"
    "vse32.v v2, (a0)\n" // Store x[a]
    :
    : [input] "r"(t), [output] "r"(t)
    : "a0", "memory", "v0", "v1", "v2"
);

#include <textio.h>
for (int i = 0; i < 8; i++) {
    print_long(t[i]);
    print_s("\n");
}
return;
```

实际代码见 `chacha20_mod.c` 文件的`chacha20_multi_asm`函数。我们将8个元素并为一组，使用LMUL=2的vector去合并计算，取得了较高的性能提升。最终周期数为103743：

```text
The image is ./chacha20_baremetal-256.bin
Test Start!
Initial state generated for LEN=256 SEED=0xdeadbeef
Cycles: 103743
Final output: 
ans[0] = 0xbe64d5a5
ans[1] = 0x83c6c3fa
ans[2] = 0xac710378
ans[3] = 0x858f0082
ans[4] = 0x207f2dc2
ans[5] = 0xeb5ee49e
ans[6] = 0x4f801257
ans[7] = 0x2e4f03c9
ans[8] = 0x74b07deb
ans[9] = 0x3e6dde54
ans[10] = 0x6dd3ddad
ans[11] = 0xee18cf6c
ans[12] = 0x053765e6
ans[13] = 0x79f814eb
ans[14] = 0x40b18db8
ans[15] = 0xa299c057
Core 0: HIT GOOD TRAP at pc = 0x80010002
Core-0 instrCnt = 143898, cycleCnt = 133143, IPC = 1.080778
Seed=0 Guest cycle spent: 133147 (this will be different from cycleCnt if emu loads a snapshot)
Host time spent: 97186ms
```


