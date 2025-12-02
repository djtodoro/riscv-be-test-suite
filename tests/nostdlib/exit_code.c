// Test exit code propagation
//
// REQUIRES: gcc || clang
//
// RUN: %if gcc %{ \
// RUN:   %gcc -nostdlib -static -mbig-endian -O2 -march=rv64gc %s -o %t-gcc.out && \
// RUN:   %qemu %t-gcc.out ; test $? -eq 42 \
// RUN: %}
// RUN: %if clang %{ \
// RUN:   %clang -nostdlib -static -fuse-ld=lld -target riscv64-unknown-linux-gnu \
// RUN:     -mbig-endian -O2 -march=rv64gc %s -o %t-clang.out && \
// RUN:   %qemu %t-clang.out ; test $? -eq 42 \
// RUN: %}

#define SYS_exit 93

static inline long syscall1(long n, long a0) {
    register long t0 asm("a0") = a0;
    register long syscall_id asm("a7") = n;
    asm volatile ("ecall" : "+r"(t0) : "r"(syscall_id) : "memory");
    return t0;
}

void _start() {
    syscall1(SYS_exit, 42);
}
