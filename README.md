# 🚀 diff-match-patch-qt6

[![Qt Version](https://img.shields.io/badge/Qt-6.0%2B-green.svg)](https://www.qt.io/)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Build Status](https://img.shields.io/badge/Build-Passing-brightgreen.svg)]()
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)]()
[![CI](https://github.com/djad04/diff-match-patch-qt6/actions/workflows/ci.yml/badge.svg)](https://github.com/djad04/diff-match-patch-qt6/actions/workflows/ci.yml)



> **A complete reimplementation of Google's diff-match-patch for Qt6, designed to leverage the full power of the latest Qt features.**
Unlike the original Qt4 version, this port eliminates deprecated components and optimizes the core algorithm for modern C++ and Qt6, offering robust support for diffing, patching, and pattern matching with enhanced precision for development workflows.


## 🚨🔴 MODIFIED ALGORITHM WARNING

> ⚠️ **This implementation has been _modified from Google’s original_ to prioritize _maximum precision and detailed diffs_, with _timeout limitations removed_ for complete accuracy.**

✅ **Best for**: Code files up to **1MB (~12,000 lines)** where human-readable, precise results are essential.  
⚠️ **For larger files**: Consider performance-optimized algorithms like **Myers** or **Patience diff**.

🧩 However, _those alternatives are more complex to integrate_, often requiring **multiple source files and additional dependencies**, whereas **this implementation is extremely easy to set up and use**, making it ideal for lightweight or embedded tools.

## Features

- **Enhanced Precision**: Modified algorithm for maximum accuracy (no timeout limitations)
- **Qt6 Integration**: Native QString support and Qt-style API
- **Unicode Support**: Full international text support
- **Human-Readable Output**: Optimized for code analysis and development workflows
- **Cross-platform**: Works on Windows, Linux, macOS
- **Header-only library**: Easy integration

## Installation

```bash
git clone https://github.com/yourusername/diff-match-patch-qt6.git
```

Include in your project:
```cpp
#include "diff_match_patch.h"
```

## Usage

```cpp
#include "diff_match_patch.h"

diff_match_patch dmp;

// Generate differences
QList<Diff> diffs = dmp.diff_main("Hello World", "Hello Universe");

// Create patches
QList<Patch> patches = dmp.patch_make("old content", "new content");
QString patchText = dmp.patch_toText(patches);

// Apply patches
QPair<QString, QVector<bool>> result = dmp.patch_apply(patches, "old content");
QString newText = result.first;
```


**Benchmark Results** (MSVC 1943, Qt 6.8.2):

### **Core Performance Tests**
| File Size | Processing Time | Operations | Efficiency | Notes |
|-----------|----------------|------------|------------|-------|
| **100 B** | 0.13 ms | 31 ops | 90.0% | Instant response |
| **1 KB** | 0.94 ms | 247 ops | 90.6% | Very fast |
| **10 KB** | 49.22 ms | 2,593 ops | 90.7% | Fast processing |
| **100 KB** | 4.35 s | 26,366 ops | 90.5% | Good precision |
| **500 KB** | 10.60 s | 71,041 ops | 95.1% | Excellent precision |

⚠️ Note: An efficiency of 95% does not mean that 5% of changes were missed — all changes are found, but the algorithm may not always find the shortest or cleanest sequence of edits possible. It’s about optimality, not correctness.

### **Noise Rate Analysis (50KB test file)**
| Change Rate | Processing Time | Operations | Efficiency |
|-------------|----------------|------------|------------|
| 1% changes | 11.53 ms | 1,471 ops | 99.0% |
| 5% changes | 181.18 ms | 6,941 ops | 95.1% |
| 10% changes | 1.10 s | 12,897 ops | 90.5% |
| 20% changes | 2.99 s | 21,957 ops | 82.1% |
| 50% changes | 9.92 s | 35,834 ops | 61.7% |

### **Real-World Scenarios**
| Scenario | File Size | Time | Operations | Efficiency |
|----------|-----------|------|------------|------------|
| JavaScript Code | 184 B | 0.10 ms | 13 ops | 73.9% |
| Config File | 4.9 KB | 18.19 ms | 901 ops | 78.4% |
| Unicode Text | 4.6 KB | 28.61 ms | 1,201 ops | 58.3% |



## Configuration(you can play with the configuration for different results)

```cpp
// Modified settings for maximum precision (default configuration)
dmp.Diff_Timeout = 0.0f;          // NO TIMEOUT - Full precision mode
dmp.Match_Threshold = 0.5f;       // Standard accuracy threshold
dmp.Match_Distance = 100000;      // Extended search distance for large files
dmp.Match_MaxBits = 8192;         // Large pattern support for complex diffs
dmp.Patch_DeleteThreshold = 0.5f; // Standard patch reliability
```

## API

### Core Methods
- `diff_main(text1, text2)` - Generate differences between texts
- `patch_make(text1, text2)` - Create patches for synchronization
- `patch_apply(patches, text)` - Apply patches to text
- `match_main(text, pattern, location)` - Find pattern with fuzzy matching

### Algorithm Features
- **Maximum precision mode**: No timeout limitations for complete analysis
- **Enhanced pattern matching**: Support for large patterns (8192 bits vs 32 in original)
- **Extended search distance**: 100x larger search radius for better matches
- **Half-match optimization**: Automatically detects large common sections in files
- **Line-mode processing**: Efficient handling of text files with line-based changes
- **Unicode support**: Full support for international text and emoji
- **Configurable thresholds**: Adjustable matching sensitivity and performance tuning

## 🔴 **Usage Recommendations**

### **Ideal Use Cases (≤1MB files, ~12,000 lines of code)**
- **Source code comparison**: Perfect for development workflows
- **Configuration file analysis**: Detailed change tracking
- **Document revision**: Human-readable diff output
- **Small to medium datasets**: Excellent precision and reasonable speed

### **Large Files (>1MB) Considerations**
For files larger than 1MB (~12,000 lines of code), consider:

- **Processing time**: Can take several minutes for multi-MB files
- **Memory usage**: Loads entire files into memory
- **Alternative algorithms**: For very large files, consider more performance-oriented algorithms like:
  - **Myers algorithm**: Faster but less human-readable output
  - **Patience diff**: Better for code with moved blocks
  - **Histogram diff**: Optimized for large files with repetitive content

**This implementation prioritizes accuracy and human-readable results over raw performance.**

## Testing & Performance

Includes comprehensive test suite with performance benchmarks in the `testing/` folder. The test benchmark validates algorithm correctness and measures performance across different file sizes and text types.


## License

MIT License - see [LICENSE](LICENSE) file.

## Acknowledgments

Inspired by Google's diff-match-patch algorithm. Completely rewritten for Qt6.


