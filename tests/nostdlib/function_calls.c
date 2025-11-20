// Test function calls and argument passing
//
// REQUIRES: gcc
// RUN: %gcc -nostdlib -static -mbig-endian -O2 -march=rv64gc %s -o %t-gcc.out
// RUN: %qemu %t-gcc.out | FileCheck %s
//
// REQUIRES: clang
// RUN: %clang -nostdlib -static -fuse-ld=lld -target riscv64-unknown-linux-gnu -mbig-endian -O2 -march=rv64gc %s -o %t-clang.out
// RUN: %qemu %t-clang.out | FileCheck %s
//
// CHECK: add_two: 0000000000000037
// CHECK: add_four: 00000000000000a6
// CHECK: add_eight: 00000000000001e8

#define SYS_write 64
#define SYS_exit 93

static inline long syscall3(long n, long a0, long a1, long a2) {
    register long t0 asm("a0") = a0;
    register long t1 asm("a1") = a1;
    register long t2 asm("a2") = a2;
    register long syscall_id asm("a7") = n;
    asm volatile ("ecall" : "+r"(t0) : "r"(t1), "r"(t2), "r"(syscall_id) : "memory");
    return t0;
}

static void write_str(const char *s) {
    int len = 0;
    while (s[len]) len++;
    syscall3(SYS_write, 1, (long)s, len);
}

static void write_hex64(unsigned long val) {
    const char hex[] = "0123456789abcdef";
    char buf[17];
    for (int i = 15; i >= 0; i--) {
        buf[i] = hex[val & 0xf];
        val >>= 4;
    }
    buf[16] = '\n';
    syscall3(SYS_write, 1, (long)buf, 17);
}

// Test function with 2 arguments
unsigned long add_two(unsigned long a, unsigned long b) {
    return a + b;
}

// Test function with 4 arguments
unsigned long add_four(unsigned long a, unsigned long b,
                       unsigned long c, unsigned long d) {
    return a + b + c + d;
}

// Test function with 8 arguments
unsigned long add_eight(unsigned long a, unsigned long b,
                        unsigned long c, unsigned long d,
                        unsigned long e, unsigned long f,
                        unsigned long g, unsigned long h) {
    return a + b + c + d + e + f + g + h;
}

void _start() {
    // Test 2 arguments (passed in registers a0, a1)
    write_str("add_two: ");
    write_hex64(add_two(0x12, 0x25));

    // Test 4 arguments (passed in registers a0-a3)
    write_str("add_four: ");
    write_hex64(add_four(0x11, 0x22, 0x33, 0x40));

    // Test 8 arguments (first 8 in a0-a7)
    write_str("add_eight: ");
    write_hex64(add_eight(0x11, 0x22, 0x33, 0x44,
                         0x55, 0x66, 0x77, 0x0c));

    syscall3(SYS_exit, 0, 0, 0);
}
