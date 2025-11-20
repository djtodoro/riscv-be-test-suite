// Test struct layout and member access in big-endian mode
//
// REQUIRES: gcc
// RUN: %gcc -nostdlib -static -mbig-endian -O2 -march=rv64gc %s -o %t-gcc.out
// RUN: %qemu %t-gcc.out | FileCheck %s
//
// REQUIRES: clang
// RUN: %clang -nostdlib -static -fuse-ld=lld -target riscv64-unknown-linux-gnu -mbig-endian -O2 -march=rv64gc %s -o %t-clang.out
// RUN: %qemu %t-clang.out | FileCheck %s
//
// CHECK: a: 12
// CHECK: b: 3456
// CHECK: c: 789abcde
// CHECK: d: f0123456789abcde

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

struct test_struct {
    unsigned char a;    // 1 byte
    unsigned short b;   // 2 bytes
    unsigned int c;     // 4 bytes
    unsigned long d;    // 8 bytes
};

void _start() {
    struct test_struct s = {
        .a = 0x12,
        .b = 0x3456,
        .c = 0x789abcde,
        .d = 0xf0123456789abcdeUL
    };

    write_str("a: ");
    write_hex(s.a, 2);

    write_str("b: ");
    write_hex(s.b, 4);

    write_str("c: ");
    write_hex(s.c, 8);

    write_str("d: ");
    write_hex(s.d, 16);

    syscall3(SYS_exit, 0, 0, 0);
}
