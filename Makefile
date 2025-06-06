CROSS_COMPILE := riscv64-linux-gnu-
LOADER := qemu-riscv64
CROSS_CC := $(CROSS_COMPILE)gcc
CROSS_LD := $(CROSS_COMPILE)ld
CROSS_OBJCOPY := $(CROSS_COMPILE)objcopy
CFLAGS := -O3 -Ilib -Isrc -fno-builtin -static
ARCH := rv64gcv_zbb_zvbb
CROSS_CFLAGS := -march=${ARCH} -mcmodel=medany
BAREMETAL_CFLAGS := -nostdlib
BENCH_LEN := 256

# CROSS_COMPILE linux
chacha20_linux-$(BENCH_LEN): src/bench-$(BENCH_LEN).o src/chacha20.o lib/textio.o lib/uart_stdout.o
	$(CROSS_CC) $(CFLAGS) $(CROSS_CFLAGS) -static -o $@ $^

# CROSS_COMPILE baremetal
chacha20_baremetal-$(BENCH_LEN): lib/am_start.S src/bench-$(BENCH_LEN).o src/chacha20.o lib/textio.o lib/uart_uartlite.o
	$(CROSS_CC) $(CFLAGS) $(CROSS_CFLAGS) $(BAREMETAL_CFLAGS) -T lib/linker.ld -o $@ $^

chacha20_baremetal-$(BENCH_LEN).bin: chacha20_baremetal-$(BENCH_LEN)
	$(CROSS_OBJCOPY) -O binary $^ $@

# CROSS_COMPILE objs
src/bench-$(BENCH_LEN).o: src/bench.c
	$(CROSS_CC) $(CFLAGS) $(CROSS_CFLAGS) $(BAREMETAL_CFLAGS) -DBENCH_LEN=$(BENCH_LEN) -c -o $@ $^

src/chacha20.o: src/chacha20.s
	$(CROSS_CC) $(CFLAGS) $(CROSS_CFLAGS) $(BAREMETAL_CFLAGS) -c -o $@ $^

lib/textio.o: lib/textio.c
	$(CROSS_CC) $(CFLAGS) $(CROSS_CFLAGS) $(BAREMETAL_CFLAGS) -c -o $@ $^

lib/uart_uartlite.o: lib/uart_uartlite.c
	$(CROSS_CC) $(CFLAGS) $(CROSS_CFLAGS) $(BAREMETAL_CFLAGS) -c -o $@ $^

lib/uart_stdout.o: lib/uart_stdout.c
	$(CROSS_CC) $(CFLAGS) $(CROSS_CFLAGS) -c -o $@ $^

# HOST objs
src/host/gen_answer: src/host/gen_answer.c src/host/chacha20_c.c
	$(CC) $(CFLAGS) -o $@ $^

answer:
	@mkdir -p answer

answer/$(BENCH_LEN).txt: src/host/gen_answer answer
	./src/host/gen_answer $(BENCH_LEN) 0xdeadbeef | grep "ans\[" > $@

xs-emu:
	if [ `nproc` -lt 8 ]; then \
		wget https://github.com/cyyself/chacha20-xiangshan/releases/download/beta/xs-emu-1t-1f23fd0f52.xz -O - | xz -d > $@; \
	else \
		wget https://github.com/cyyself/chacha20-xiangshan/releases/download/beta/xs-emu-8t-1f23fd0f52.xz -O - | xz -d > $@; \
	fi
	chmod +x $@

.PHONY: validate validate-rtl clean run

clean:
	rm -f chacha20_linux-* chacha20_baremetal-* lib/*.o src/*.o src/host/gen_answer src/chacha20.s

validate: chacha20_linux-$(BENCH_LEN) answer/$(BENCH_LEN).txt
	@$(LOADER) ./chacha20_linux-$(BENCH_LEN) | tail -n 16 | diff answer/$(BENCH_LEN).txt - && echo "Test passed"

validate-out: chacha20_linux-$(BENCH_LEN) answer/$(BENCH_LEN).txt
	@$(LOADER) ./chacha20_linux-$(BENCH_LEN)

validate-rtl: xs-emu chacha20_baremetal-$(BENCH_LEN).bin answer/$(BENCH_LEN).txt
	./xs-emu -i ./chacha20_baremetal-$(BENCH_LEN).bin --no-diff 2>/dev/null | grep "ans\[" | diff answer/$(BENCH_LEN).txt - && echo "Test passed"

run: xs-emu chacha20_baremetal-$(BENCH_LEN).bin
	./xs-emu -i ./chacha20_baremetal-$(BENCH_LEN).bin --no-diff 2>xs.log

src/chacha20.s: src/host/chacha20_mod.c
	$(CROSS_CC) -O3 -nostdlib $(CFLAGS) -march=$(ARCH) -Isrc -S -fverbose-asm src/host/chacha20_mod.c -o src/chacha20.s

# -nostdlib -fno-builtin不调用标准库函数，不使用内建函数如memcpy