// Test basic arithmetic operations in big-endian mode
//
// REQUIRES: gcc
// RUN: %gcc -nostdlib -static -mbig-endian -O2 -march=rv64gc %s -o %t-gcc.out
// RUN: %qemu %t-gcc.out | FileCheck %s
//
// REQUIRES: clang
// RUN: %clang -nostdlib -static -fuse-ld=lld -target riscv64-unknown-linux-gnu -mbig-endian -O2 -march=rv64gc %s -o %t-clang.out
// RUN: %qemu %t-clang.out | FileCheck %s
//
// CHECK: add: 0000000000579bbd
// CHECK: sub: 0000000000555533
// CHECK: mul: 00000062623ee058
// CHECK: div: 000000000000004b

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

void _start() {
    unsigned long a = 0x0000000000012345UL;
    unsigned long b = 0x0000000000567878UL;

    // Addition
    write_str("add: ");
    write_hex64(a + b);

    // Subtraction
    write_str("sub: ");
    write_hex64(b - a);

    // Multiplication
    write_str("mul: ");
    write_hex64(a * b);

    // Division
    write_str("div: ");
    write_hex64(b / a);

    syscall3(SYS_exit, 0, 0, 0);
}
