// Test pointer arithmetic and dereferencing in big-endian mode
//
// REQUIRES: gcc
// RUN: %gcc -nostdlib -static -mbig-endian -O2 -march=rv64gc %s -o %t-gcc.out
// RUN: %qemu %t-gcc.out | FileCheck %s
//
// REQUIRES: clang
// RUN: %clang -nostdlib -static -fuse-ld=lld -target riscv64-unknown-linux-gnu -mbig-endian -O2 -march=rv64gc %s -o %t-clang.out
// RUN: %qemu %t-clang.out | FileCheck %s
//
// CHECK: arr[0]: 11111111
// CHECK: arr[1]: 22222222
// CHECK: arr[2]: 33333333
// CHECK: arr[3]: 44444444
// CHECK: ptr+1: 22222222
// CHECK: ptr+3: 44444444

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

void _start() {
    unsigned int arr[4] = {
        0x11111111,
        0x22222222,
        0x33333333,
        0x44444444
    };

    // Test array indexing
    for (int i = 0; i < 4; i++) {
        write_str("arr[");
        char idx = '0' + i;
        syscall3(SYS_write, 1, (long)&idx, 1);
        write_str("]: ");
        write_hex32(arr[i]);
    }

    // Test pointer arithmetic
    unsigned int *ptr = arr;

    write_str("ptr+1: ");
    write_hex32(*(ptr + 1));

    write_str("ptr+3: ");
    write_hex32(*(ptr + 3));

    syscall3(SYS_exit, 0, 0, 0);
}
