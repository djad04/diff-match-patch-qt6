# Testing Suite for diff-match-patch-qt6

This directory contains comprehensive tests and benchmarks for the diff-match-patch-qt6 library.

## Test Components

### 1. **test_suite.cpp** - Functional Tests
- Basic diff operations (insert, delete, modify)
- Complex multi-line changes
- Unicode and emoji handling
- Patch creation and application
- Pattern matching operations
- Edge cases and error handling
- Real-world scenarios (code, configs, documents)

### 2. **benchmark.cpp** - Performance Tests
- Processing speed across different file sizes
- Unicode text performance
- Patch operation benchmarks
- Real-world scenario timing
- Stress tests with large files

### 3. **memory_test.cpp** - Memory Usage Analysis
- Memory usage by file size
- Memory leak detection
- Large file memory efficiency
- Memory cleanup verification

## Quick Start

### Windows
```bash
# Run all tests and benchmarks
run_all.bat

# Or run individually:
run_tests.bat      # Functional tests + memory tests
run_benchmark.bat  # Performance benchmarks
```

### Linux/macOS
```bash
# Build and run manually
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make

# Run tests
./test_suite
./benchmark
./memory_test
```

## Requirements

- Qt6 (Core module)
- CMake 3.16+
- C++17 compatible compiler

## Expected Results

The test suite should achieve:
- **100% test pass rate** - All functional tests pass
- **Sub-millisecond performance** - Small files (< 1KB) process in < 1ms
- **Efficient memory usage** - No significant memory leaks
- **Unicode support** - Full international character handling

## Interpreting Results

### Test Suite Output
```
✓ PASS: Test description
✗ FAIL: Test description (if any)

📊 Test Summary
===============
Tests Passed: X / Y
Success Rate: Z%
```

### Benchmark Output
```
Test Name                 |     Size |   Time
------------------------------------------------
Basic Diff               |    1.0 KB |      0 ms
Basic Diff               |   10.0 KB |      2 ms
...
```

### Memory Test Output
```
File Size       | Memory Before   | Memory After
----------------------------------------------------
1.0 KB         | 15.2 MB        | 15.3 MB
...
```

## Updating README Performance Data

After running the benchmarks, update the main README.md with actual results:

1. Run `run_benchmark.bat`
2. Copy the performance table results
3. Update the "Performance" section in `../README.md`
4. Include any notable findings or optimizations

## Troubleshooting

### Build Issues
- Ensure Qt6 is properly installed and in PATH
- Check CMake version (3.16+ required)
- Verify C++17 compiler support

### Test Failures
- Check Qt6 installation
- Verify diff_match_patch.h and .cpp are in parent directory
- Review compiler warnings for potential issues

### Performance Issues
- Run in Release mode for accurate benchmarks
- Close other applications during testing
- Consider system specifications when comparing results
