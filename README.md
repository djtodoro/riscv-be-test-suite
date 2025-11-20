# RISC-V Big-Endian Test Suite

A roundtrip test suite for validating RISC-V big-endian support in GCC and LLVM/Clang compilers using LLVM's LIT testing framework with actual execution (with QEMU for now only). So, it compiles executables with both compilers, execute them via emulator, and then checks the expected output with `lit+filecheck`.

## How to run

Simply run the run script...

```bash
$ ./run_tests.sh
RISC-V Big-Endian Test Suite
=============================
GCC:   bin/riscv64-unknown-linux-gnu-gcc
Clang: LLVM/build/bin/clang
QEMU:  BE_RISCV/qemu/build/qemu-riscv64be
LIT:   LLVM/build/bin/llvm-lit

-- Testing: 10 tests, 10 workers --
PASS: RISC-V Big-Endian Test Suite :: tests/nostdlib/arithmetic.c (1 of 10)
PASS: RISC-V Big-Endian Test Suite :: tests/nostdlib/bitwise.c (2 of 10)
PASS: RISC-V Big-Endian Test Suite :: tests/nostdlib/branches.c (3 of 10)
PASS: RISC-V Big-Endian Test Suite :: tests/nostdlib/data_endian.c (4 of 10)
PASS: RISC-V Big-Endian Test Suite :: tests/nostdlib/exit_code.c (5 of 10)
PASS: RISC-V Big-Endian Test Suite :: tests/nostdlib/function_calls.c (6 of 10)
PASS: RISC-V Big-Endian Test Suite :: tests/nostdlib/hello.c (7 of 10)
PASS: RISC-V Big-Endian Test Suite :: tests/nostdlib/memory_ops.c (8 of 10)
PASS: RISC-V Big-Endian Test Suite :: tests/nostdlib/pointer_ops.c (9 of 10)
PASS: RISC-V Big-Endian Test Suite :: tests/nostdlib/struct_layout.c (10 of 10)

Testing Time: 0.13s

Total Discovered Tests: 10
  Passed: 10 (100.00%)

```

## Prerequisites

- LLVM LIT (from LLVM build)
- FileCheck (from LLVM build)
- RISC-V big-endian GCC toolchain
- RISC-V big-endian LLVM/Clang (with djtodoro's patches - in the process of upstream)
- QEMU user-mode emulator with RISC-V big-endian support
    - https://github.com/djtodoro/qemu/tree/riscvbe/current
    - to build this, use:
    ```bash
    $ mkdir build && cd build
    $ ../configure --target-list=riscv64be-linux-user
    $ make -j4
    ```

## Setup

**Required:** Set the following environment variables to point to your tools:

```bash
export RISCV_BE_GCC=/path/to/riscv64-mti-linux-gnu-gcc
export RISCV_BE_CLANG=/path/to/clang
export RISCV_BE_QEMU=/path/to/qemu-riscv64be
export LLVM_LIT=/path/to/llvm-lit
```

**Note:** These environment variables are **required**. The test suite will fail with a clear error message if they are not set.

## Running Tests

### Using the convenience script (recommended):
```bash
./run_tests.sh
```

### Run all tests directly with llvm-lit:
```bash
llvm-lit -v .
```

Both methods require the environment variables to be set first.

### Run specific test directory:
```bash
llvm-lit -v tests/nostdlib/
```

### Run single test:
```bash
llvm-lit -v tests/nostdlib/hello.c
```

### Run only GCC or Clang tests:
```bash
llvm-lit -v --filter=gcc tests/
llvm-lit -v --filter=clang tests/
```

## Writing Tests

Each test file should include:

1. **Comment header** with test description
2. **REQUIRES** directives for compiler availability
3. **RUN** lines with compilation and execution commands
4. **CHECK** directives for FileCheck validation
5. **Test code**

Example:
```c
// Test description
//
// REQUIRES: gcc
// RUN: %gcc %gcc_flags %s -o %t.gcc.out
// RUN: %qemu %t.gcc.out | FileCheck %s
//
// CHECK: Expected output

void _start() {
    // Test implementation
}
```
## Current Test Categories

### nostdlib Tests
Tests that don't require standard library, using direct syscalls (10 tests):

**Basic Tests:**
- **hello.c** - Basic execution and stdout output verification
- **exit_code.c** - Exit code propagation test
- **data_endian.c** - Verify big-endian data storage format

**Arithmetic & Logic:**
- **arithmetic.c** - Basic arithmetic operations (add, sub, mul, div)
- **bitwise.c** - Bitwise operations (and, or, xor, shift left/right)

**Memory Operations:**
- **memory_ops.c** - Memory load/store with various sizes (byte, halfword, word, doubleword)
- **struct_layout.c** - Structure member access and layout verification
- **pointer_ops.c** - Pointer arithmetic and array indexing

**Control Flow:**
- **branches.c** - Conditional branches and comparisons (eq, ne, lt, le, gt, ge)
- **function_calls.c** - Function calls with 2, 4, and 8 arguments (register and stack passing)

### stdlib Tests (TODO)
Tests using standard library functions.

## Adding New Tests

1. Create test file in appropriate directory (`tests/nostdlib/` or `tests/stdlib/`)
2. Add RUN lines for both GCC and Clang (with REQUIRES directives)
3. Add CHECK directives for expected output
4. Run `llvm-lit -v` to verify
