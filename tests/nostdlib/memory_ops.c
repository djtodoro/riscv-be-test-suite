// Test memory load/store operations with various sizes
//
// REQUIRES: gcc
// RUN: %gcc -nostdlib -static -mbig-endian -O2 -march=rv64gc %s -o %t-gcc.out
// RUN: %qemu %t-gcc.out | FileCheck %s
//
// REQUIRES: clang
// RUN: %clang -nostdlib -static -fuse-ld=lld -target riscv64-unknown-linux-gnu -mbig-endian -O2 -march=rv64gc %s -o %t-clang.out
// RUN: %qemu %t-clang.out | FileCheck %s
//
// CHECK: byte: f0
// CHECK: half: def0
// CHECK: word: 9abcdef0
// CHECK: dword: 123456789abcdef0

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

static void write_hex(unsigned long val, int digits) {
    const char hex[] = "0123456789abcdef";
    char buf[17];
    for (int i = digits - 1; i >= 0; i--) {
        buf[i] = hex[val & 0xf];
        val >>= 4;
    }
    buf[digits] = '\n';
    syscall3(SYS_write, 1, (long)buf, digits + 1);
}

void _start() {
    // Initialize data
    unsigned long data = 0x123456789abcdef0UL;

    // Test byte load (8-bit)
    unsigned char *byte_ptr = (unsigned char *)&data;
    write_str("byte: ");
    write_hex(byte_ptr[7], 2);  // Read last byte in big-endian

    // Test halfword load (16-bit)
    unsigned short *half_ptr = (unsigned short *)&data;
    write_str("half: ");
    write_hex(half_ptr[3], 4);  // Read last halfword in big-endian

    // Test word load (32-bit)
    unsigned int *word_ptr = (unsigned int *)&data;
    write_str("word: ");
    write_hex(word_ptr[1], 8);  // Read last word in big-endian

    // Test doubleword load (64-bit)
    write_str("dword: ");
    write_hex(data, 16);

    syscall3(SYS_exit, 0, 0, 0);
}
