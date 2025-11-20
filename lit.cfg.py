import os
import lit.formats
import lit.util

# Configuration file for the 'lit' test runner.

# name: The name of this test suite.
config.name = 'RISC-V Big-Endian Test Suite'

# testFormat: The test format to use to interpret tests.
config.test_format = lit.formats.ShTest(True)

# suffixes: A list of file extensions to treat as test files.
config.suffixes = ['.c']

# test_source_root: The root path where tests are located.
config.test_source_root = os.path.dirname(__file__)

# test_exec_root: The root path where tests should be run.
config.test_exec_root = os.path.join(config.test_source_root, 'output')

# Ensure output directory exists
os.makedirs(config.test_exec_root, exist_ok=True)

# Get paths from environment variables (required)
gcc_path = os.environ.get('RISCV_BE_GCC')
clang_path = os.environ.get('RISCV_BE_CLANG')
qemu_path = os.environ.get('RISCV_BE_QEMU')

# Check that required environment variables are set
if not gcc_path:
    lit_config.fatal('RISCV_BE_GCC environment variable must be set')
if not clang_path:
    lit_config.fatal('RISCV_BE_CLANG environment variable must be set')
if not qemu_path:
    lit_config.fatal('RISCV_BE_QEMU environment variable must be set')

# Verify paths exist
def check_tool(name, path):
    if not os.path.exists(path):
        lit_config.warning(f'{name} not found at: {path}')
        lit_config.note(f'Set {name.upper().replace(" ", "_")}_PATH environment variable')
        return False
    return True

gcc_available = check_tool('GCC', gcc_path)
clang_available = check_tool('Clang', clang_path)
qemu_available = check_tool('QEMU', qemu_path)

if not qemu_available:
    lit_config.fatal('QEMU is required to run tests')

# Find FileCheck
filecheck_path = os.path.join(os.path.dirname(clang_path), 'FileCheck')
if not os.path.exists(filecheck_path):
    filecheck_path = lit.util.which('FileCheck')
    if not filecheck_path:
        lit_config.fatal('FileCheck not found')

# Add substitutions for use in RUN lines
config.substitutions.append(('%gcc', gcc_path))
config.substitutions.append(('%clang', clang_path))
config.substitutions.append(('%qemu', qemu_path))
# Note: LIT automatically provides %t as a unique temporary file path per test
config.substitutions.append(('FileCheck', filecheck_path))

# Common compiler flags
gcc_flags = '-nostdlib -static -EB -O2 -march=rv64gc'
clang_flags = '-nostdlib -static -target riscv64-unknown-linux-gnu -mbig-endian -O2 -march=rv64gc'

config.substitutions.append(('%gcc_flags', gcc_flags))
config.substitutions.append(('%clang_flags', clang_flags))

# Features based on tool availability
if gcc_available:
    config.available_features.add('gcc')
if clang_available:
    config.available_features.add('clang')

lit_config.note(f'GCC: {gcc_path} (available: {gcc_available})')
lit_config.note(f'Clang: {clang_path} (available: {clang_available})')
lit_config.note(f'QEMU: {qemu_path} (available: {qemu_available})')
