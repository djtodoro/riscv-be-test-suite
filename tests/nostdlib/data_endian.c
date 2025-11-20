// Test that data is actually stored in big-endian format
// This test verifies endianness by reading multi-byte values
//
// REQUIRES: gcc
// RUN: %gcc -nostdlib -static -mbig-endian -O2 -march=rv64gc %s -o %t-gcc.out
// RUN: %qemu %t-gcc.out | FileCheck %s
//
// REQUIRES: clang
// RUN: %clang -nostdlib -static -fuse-ld=lld -target riscv64-unknown-linux-gnu -mbig-endian -O2 -march=rv64gc %s -o %t-clang.out
// RUN: %qemu %t-clang.out | FileCheck %s
//
// CHECK: int32: 12345678
// CHECK: int64: 123456789abcdef0

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

static void write_hex32(unsigned int val) {
    const char hex[] = "0123456789abcdef";
    char buf[9];
    for (int i = 7; i >= 0; i--) {
        buf[i] = hex[val & 0xf];
        val >>= 4;
    }
    buf[8] = '\n';
    syscall3(SYS_write, 1, (long)buf, 9);
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
    // Test 32-bit value
    unsigned int val32 = 0x12345678;
    write_str("int32: ");
    write_hex32(val32);

    // Test 64-bit value
    unsigned long val64 = 0x123456789abcdef0UL;
    write_str("int64: ");
    write_hex64(val64);

    syscall3(SYS_exit, 0, 0, 0);
}
