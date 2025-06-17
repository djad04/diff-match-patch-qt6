# diff-match-patch-qt6 Tests

cross-platform test suite built using QTest framework and CTest integration for CI/CD.

## 🏗️ Structure

```
tests/
├── CMakeLists.txt              # Main test configuration
├── unit/                       # Unit tests with QTest
│   ├── test_diff.cpp          # Diff operations tests
│   ├── test_patch.cpp         # Patch operations tests
│   ├── test_match.cpp         # Match operations tests
│   └── test_edge_cases.cpp    # Edge cases and error handling
├── benchmark/                  # Performance tests
│   └── test_performance.cpp   # Algorithm performance benchmarks
└── memory/                     # Memory usage tests
    └── test_memory.cpp        # Memory efficiency and leak detection
```

## 🚀 Running Tests

### Local Development

```bash
# Build and run all tests
cd tests
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure

# Run specific test categories
ctest --test-dir build -R "DiffOperations|PatchOperations"  # Unit tests only
ctest --test-dir build -R "Performance"                     # Performance tests only
ctest --test-dir build -R "Memory"                         # Memory tests only
```

### Individual Test Execution

```bash
# Run specific tests directly
cd tests/build
./unit/test_diff                    # Diff operations
./unit/test_patch                   # Patch operations  
./unit/test_match                   # Match operations
./unit/test_edge_cases             # Edge cases
./benchmark/test_performance       # Performance benchmarks
./memory/test_memory               # Memory tests
```

## 🧪 Test Categories

### Unit Tests (QTest Framework)
- **test_diff.cpp**: Core diff algorithm validation
  - Basic insertions, deletions, replacements
  - Unicode and special character support
  - Large text handling
  - Line-based diff operations

- **test_patch.cpp**: Patch creation and application
  - Patch generation from diffs
  - Patch application and round-trip testing
  - Patch serialization/deserialization
  - Error handling for incompatible patches

- **test_match.cpp**: Fuzzy matching operations
  - Exact and approximate pattern matching
  - Unicode pattern matching
  - Special character handling
  - Position-based matching

- **test_edge_cases.cpp**: Boundary conditions
  - Empty string handling
  - Very large texts
  - Single character operations
  - Whitespace-only content

### Performance Tests
- **test_performance.cpp**: Algorithm efficiency
  - Small, medium, and large file benchmarks
  - Noise sensitivity analysis
  - Real-world scenario testing
  - Performance regression detection

### Memory Tests
- **test_memory.cpp**: Memory usage validation
  - Memory leak detection
  - Large file memory efficiency
  - Object lifecycle management
  - Memory cleanup verification

## 📊 CI/CD Integration

### GitHub Actions
Tests run automatically on:
- Push to `main` or `develop` branches
- Pull requests to `main`
- Multiple platforms: Ubuntu, Windows, macOS
- Qt 6.5.0+ compatibility

### Test Results
- ✅ **PASS**: All tests successful
- ❌ **FAIL**: Test failures with detailed output
- 📊 **Coverage**: Code coverage reports (Linux only)

### Status Badges

```markdown
[![CI](https://github.com/djad04/diff-match-patch-qt6/workflows/CI/badge.svg)](https://github.com/djad04/diff-match-patch-qt6/actions)
```

## 🔧 Configuration

### Algorithm Configuration
Tests use optimized settings:
```cpp
dmp.Diff_Timeout = 0.0f;        // No timeout for maximum precision
dmp.Match_Distance = 100000;    // Extended search distance
dmp.Match_MaxBits = 8192;       // Large pattern support
```

### Failure Criteria
Tests fail if:
- Algorithm produces incorrect results
- Performance degrades significantly
- Memory leaks detected
- Crashes or exceptions occur

## 🛠️ Development

### Adding New Tests
1. Create test file in appropriate directory
2. Use QTest framework macros
3. Add to CMakeLists.txt
4. Register with CTest

## 📝 Example Test

```cpp
#include <QtTest/QtTest>
#include "../../diff_match_patch.h"

class TestExample : public QObject
{
    Q_OBJECT

private slots:
    void testBasicOperation() {
        diff_match_patch dmp;
        QString text1 = "Hello World";
        QString text2 = "Hello Universe";
        
        QList<Diff> diffs = dmp.diff_main(text1, text2);
        
        QVERIFY(!diffs.isEmpty());
        QVERIFY(diffs.size() >= 2);
    }
};

QTEST_MAIN(TestExample)
#include "test_example.moc"
```
