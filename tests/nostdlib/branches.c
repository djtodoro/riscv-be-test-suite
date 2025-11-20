// Test conditional branches and comparisons
//
// REQUIRES: gcc
// RUN: %gcc -nostdlib -static -mbig-endian -O2 -march=rv64gc %s -o %t-gcc.out
// RUN: %qemu %t-gcc.out | FileCheck %s
//
// REQUIRES: clang
// RUN: %clang -nostdlib -static -fuse-ld=lld -target riscv64-unknown-linux-gnu -mbig-endian -O2 -march=rv64gc %s -o %t-clang.out
// RUN: %qemu %t-clang.out | FileCheck %s
//
// CHECK: eq: true
// CHECK: ne: false
// CHECK: lt: true
// CHECK: le: true
// CHECK: gt: false
// CHECK: ge: false

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

void _start() {
    long a = 0x12345678;
    long b = 0x87654321;
    long c = 0x12345678;

    // Test equality
    write_str("eq: ");
    if (a == c) {
        write_str("true\n");
    } else {
        write_str("false\n");
    }

    // Test inequality
    write_str("ne: ");
    if (a != c) {
        write_str("true\n");
    } else {
        write_str("false\n");
    }

    // Test less than
    write_str("lt: ");
    if (a < b) {
        write_str("true\n");
    } else {
        write_str("false\n");
    }

    // Test less than or equal
    write_str("le: ");
    if (a <= c) {
        write_str("true\n");
    } else {
        write_str("false\n");
    }

    // Test greater than
    write_str("gt: ");
    if (a > b) {
        write_str("true\n");
    } else {
        write_str("false\n");
    }

    // Test greater than or equal
    write_str("ge: ");
    if (a >= b) {
        write_str("true\n");
    } else {
        write_str("false\n");
    }

    syscall3(SYS_exit, 0, 0, 0);
}
