#!/bin/bash
# Convenience script to run the RISC-V big-endian test suite

# Check that required environment variables are set
if [ -z "$RISCV_BE_GCC" ]; then
    echo "Error: RISCV_BE_GCC environment variable is not set"
    echo "Example: export RISCV_BE_GCC=/path/to/riscv64-mti-linux-gnu-gcc"
    exit 1
fi

if [ -z "$RISCV_BE_CLANG" ]; then
    echo "Error: RISCV_BE_CLANG environment variable is not set"
    echo "Example: export RISCV_BE_CLANG=/path/to/clang"
    exit 1
fi

if [ -z "$RISCV_BE_QEMU" ]; then
    echo "Error: RISCV_BE_QEMU environment variable is not set"
    echo "Example: export RISCV_BE_QEMU=/path/to/qemu-riscv64be"
    exit 1
fi

if [ -z "$LLVM_LIT" ]; then
    echo "Error: LLVM_LIT environment variable is not set"
    echo "Example: export LLVM_LIT=/path/to/llvm-lit"
    exit 1
fi

export RISCV_BE_GCC RISCV_BE_CLANG RISCV_BE_QEMU

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "RISC-V Big-Endian Test Suite"
echo "============================="
echo "GCC:   $RISCV_BE_GCC"
echo "Clang: $RISCV_BE_CLANG"
echo "QEMU:  $RISCV_BE_QEMU"
echo "LIT:   $LLVM_LIT"
echo ""

# Check if llvm-lit exists
if [ ! -x "$LLVM_LIT" ]; then
    echo "Error: llvm-lit not found at: $LLVM_LIT"
    echo "Set LLVM_LIT environment variable or update this script"
    exit 1
fi

# Run tests
cd "$SCRIPT_DIR"
exec "$LLVM_LIT" -v "$@" .
