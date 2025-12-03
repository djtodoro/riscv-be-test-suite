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
if not gcc_path and not clang_path:
    lit_config.fatal('Either RISCV_BE_GCC or RISCV_BE_CLANG ' \
                     'environment variable must be set');
if not qemu_path:
    lit_config.fatal('RISCV_BE_QEMU environment variable must be set')

# Verify paths exist
def check_tool(name, path):
    if not path:
        lit_config.note(f'{name} not set')
        return False

    if not os.path.exists(path):
        lit_config.fatal(f'{name} not found at: {path}')
        return False
    return True

tools = ['GCC', 'Clang', 'QEMU']
tool_paths = [gcc_path, clang_path, qemu_path]

available_tools = [t for t in zip(tools, tool_paths)
                   if check_tool(t[0], t[1])];

# Find FileCheck
filecheck_path = os.path.join(os.path.dirname(clang_path), 'FileCheck')
if not os.path.exists(filecheck_path):
    filecheck_path = lit.util.which('FileCheck')
    if not filecheck_path:
        lit_config.fatal('FileCheck not found')

# Note: LIT automatically provides %t as a unique temporary file path per test
config.substitutions.append(('FileCheck', filecheck_path))

# Common compiler flags
gcc_flags = '-nostdlib -static -EB -O2 -march=rv64gc'
clang_flags = '-nostdlib -static -target riscv64-unknown-linux-gnu ' \
              '-mbig-endian -O2 -march=rv64gc'

config.substitutions.append(('%gcc_flags', gcc_flags))
config.substitutions.append(('%clang_flags', clang_flags))

# Features based on tool availability
for tool in available_tools:
    config.available_features.add(str(tool[0]).lower())
    # Add substitutions for use in RUN lines
    config.substitutions.append(('%' + str(tool[0]).lower(),
                                 str(tool[1])))
