#include "../diff_match_patch.h"
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QRandomGenerator>
#include <QString>
#include <QList>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <string>

/**
 * Professional Benchmark Suite for diff-match-patch-qt6
 * 
 * Measures only core algorithm performance, excluding text generation time.
 * Provides comprehensive analysis of algorithm behavior across different scenarios.
 */
class DiffMatchPatchBenchmark {
private:
    diff_match_patch dmp;
    
    struct BenchmarkResult {
        std::string testName;
        size_t fileSize;
        double executionTimeMs;
        int operationCount;
        double efficiency;
        std::string notes;
    };
    
    std::vector<BenchmarkResult> results;
    
    // Text generation utilities (excluded from timing)
    QString generateRandomText(size_t size) {
        const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 \n\t";
        QString result;
        result.reserve(static_cast<int>(size));
        
        for (size_t i = 0; i < size; ++i) {
            result += chars[QRandomGenerator::global()->bounded(chars.length())];
        }
        return result;
    }
    
    QString modifyText(const QString& original, double changeRate) {
        QString modified = original;
        int changeCount = static_cast<int>(original.length() * changeRate);
        
        for (int i = 0; i < changeCount; ++i) {
            int pos = QRandomGenerator::global()->bounded(modified.length());
            modified[pos] = (modified[pos] == 'X') ? 'Y' : 'X';
        }
        return modified;
    }
    
    // Core benchmark execution - measures ONLY algorithm time
    BenchmarkResult executeDiffBenchmark(const QString& testName,
                                        const QString& text1,
                                        const QString& text2,
                                        const std::string& notes = "") {

        // Show progress for large files
        if (text1.length() > 100000) {
            std::cout << "  Running " << testName.toStdString() << " ("
                      << formatFileSize(text1.length()) << ")... " << std::flush;
        }

        // Measure ONLY the algorithm execution time
        auto start = std::chrono::high_resolution_clock::now();
        QList<Diff> diffs = dmp.diff_main(text1, text2);
        auto end = std::chrono::high_resolution_clock::now();

        if (text1.length() > 100000) {
            std::cout << "Done!\n";
        }
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        double timeMs = duration.count() / 1000.0;
        
        // Calculate efficiency (percentage of content preserved as EQUAL)
        int totalEqualChars = 0;
        for (const Diff& diff : diffs) {
            if (diff.operation == EQUAL) {
                totalEqualChars += diff.text.length();
            }
        }
        double efficiency = (double)totalEqualChars / std::max(text1.length(), text2.length()) * 100.0;
        
        BenchmarkResult result;
        result.testName = testName.toStdString();
        result.fileSize = std::max(text1.length(), text2.length());
        result.executionTimeMs = timeMs;
        result.operationCount = diffs.size();
        result.efficiency = efficiency;
        result.notes = notes;
        
        results.push_back(result);
        return result;
    }
    
    std::string formatFileSize(size_t bytes) {
        if (bytes < 1024) {
            return std::to_string(bytes) + " B";
        } else if (bytes < 1024 * 1024) {
            return std::to_string(static_cast<int>(bytes / 1024.0 * 10) / 10.0) + " KB";
        } else {
            return std::to_string(static_cast<int>(bytes / (1024.0 * 1024.0) * 10) / 10.0) + " MB";
        }
    }
    
    void printResult(const BenchmarkResult& result) {
        std::cout << std::left << std::setw(30) << result.testName
                  << " | " << std::setw(8) << formatFileSize(result.fileSize)
                  << " | " << std::setw(10) << std::fixed << std::setprecision(2) << result.executionTimeMs << " ms"
                  << " | " << std::setw(6) << result.operationCount << " ops"
                  << " | " << std::setw(6) << std::setprecision(1) << result.efficiency << "%";

        if (!result.notes.empty()) {
            std::cout << " | " << result.notes;
        }
        std::cout << "\n" << std::flush;  // Force immediate output
    }
    
    void printHeader() {
        std::cout << std::left << std::setw(30) << "Test Name"
                  << " | " << std::setw(8) << "Size"
                  << " | " << std::setw(12) << "Time"
                  << " | " << std::setw(8) << "Ops"
                  << " | " << std::setw(8) << "Efficiency"
                  << " | Notes\n";
        std::cout << std::string(90, '-') << "\n";
    }
    
    void printSeparator() {
        std::cout << std::string(90, '-') << "\n";
    }
    
    void testBasicPerformance() {
        std::cout << "\n=== Basic Performance Tests ===\n" << std::flush;

        std::vector<size_t> sizes = {100, 1024, 10240, 102400}; // 100B to 100KB
        
        for (size_t size : sizes) {
            std::cout << "  Testing " << formatFileSize(size) << "... " << std::flush;

            // Pre-generate texts (not timed)
            QString text1 = generateRandomText(size);
            QString text2 = modifyText(text1, 0.1); // 10% changes

            // Execute and time only the algorithm
            auto result = executeDiffBenchmark(
                QString("Basic Diff (%1)").arg(formatFileSize(size).c_str()),
                text1, text2, "10% random changes"
            );
            std::cout << "Done!\n";
            printResult(result);
        }
    }
    
    void testNoiseSensitivity() {
        std::cout << "\n=== Noise Sensitivity Analysis ===\n" << std::flush;

        const size_t testSize = 50000; // 50KB
        std::vector<double> noiseRates = {0.01, 0.05, 0.10, 0.20, 0.50};
        
        for (double rate : noiseRates) {
            // Pre-generate texts (not timed)
            QString text1 = generateRandomText(testSize);
            QString text2 = modifyText(text1, rate);
            
            std::string notes = std::to_string(static_cast<int>(rate * 100)) + "% noise";
            auto result = executeDiffBenchmark(
                QString("Noise Test (%1%)").arg(static_cast<int>(rate * 100)),
                text1, text2, notes
            );
            printResult(result);
        }
    }
    
    void testLargeFilePerformance() {
        std::cout << "\n=== Large File Performance ===\n" << std::flush;

        std::vector<size_t> largeSizes = {102400, 512000}; // 100KB, 500KB (reduced for faster testing)
        
        for (size_t size : largeSizes) {
            // Pre-generate texts (not timed)
            QString text1 = generateRandomText(size);
            QString text2 = modifyText(text1, 0.05); // 5% changes
            
            auto result = executeDiffBenchmark(
                QString("Large File (%1)").arg(formatFileSize(size).c_str()),
                text1, text2, "5% distributed changes"
            );
            printResult(result);
        }
    }
    
    void testRealWorldScenarios() {
        std::cout << "\n=== Real-World Scenarios ===\n" << std::flush;
        
        // Code comparison
        QString code1 = R"(function calculateTotal(items) {
    let total = 0;
    for (let item of items) {
        total += item.price;
    }
    return total;
})";
        
        QString code2 = R"(function calculateTotal(items, tax = 0.1) {
    let subtotal = 0;
    for (let item of items) {
        subtotal += item.price * item.quantity;
    }
    return subtotal * (1 + tax);
})";
        
        auto result1 = executeDiffBenchmark("Code Comparison", code1, code2, "JavaScript function");
        printResult(result1);
        
        // Configuration file
        QString config1 = QString("server.port=8080\nserver.host=localhost\ndebug=false\n").repeated(100);
        QString config2 = config1;
        config2.replace("8080", "9090");
        config2.replace("localhost", "0.0.0.0");
        
        auto result2 = executeDiffBenchmark("Config File", config1, config2, "Server configuration");
        printResult(result2);
        
        // Unicode text
        QString unicode1 = QString("Hello 世界 🌍 مرحبا ").repeated(200);
        QString unicode2 = unicode1;
        unicode2.replace("世界", "Universe");
        unicode2.replace("🌍", "🚀");
        
        auto result3 = executeDiffBenchmark("Unicode Text", unicode1, unicode2, "International text");
        printResult(result3);
    }
    
    void testAlgorithmComparison() {
        std::cout << "\n=== Algorithm Configuration Comparison ===\n" << std::flush;

        const size_t testSize = 102400; // 100KB (reduced for faster testing)
        QString text1 = generateRandomText(testSize);
        QString text2 = modifyText(text1, 0.05);
        
        // Test with original Google settings
        diff_match_patch originalDmp;
        originalDmp.Diff_Timeout = 1.0f;
        originalDmp.Match_Distance = 1000;
        originalDmp.Match_MaxBits = 32;
        
        auto start = std::chrono::high_resolution_clock::now();
        QList<Diff> originalDiffs = originalDmp.diff_main(text1, text2);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        std::cout << "Original Algorithm    | " << formatFileSize(testSize) 
                  << " | " << std::setw(10) << std::fixed << std::setprecision(2) 
                  << (duration.count() / 1000.0) << " ms | " 
                  << std::setw(6) << originalDiffs.size() << " ops | Fast but limited\n";
        
        // Test with modified settings
        auto result = executeDiffBenchmark("Modified Algorithm", text1, text2, "Enhanced precision");
        printResult(result);
    }
    
    void printSummary() {
        std::cout << "\n=== Benchmark Summary ===\n";
        std::cout << "Total tests executed: " << results.size() << "\n";
        
        double totalTime = 0;
        for (const auto& result : results) {
            totalTime += result.executionTimeMs;
        }
        std::cout << "Total algorithm execution time: " << std::fixed << std::setprecision(2) 
                  << totalTime << " ms\n";
        
        std::cout << "\nAlgorithm Configuration:\n";
        std::cout << "  Timeout: " << (dmp.Diff_Timeout == 0 ? "Disabled (maximum precision)" : "Enabled") << "\n";
        std::cout << "  Match Distance: " << dmp.Match_Distance << "\n";
        std::cout << "  Max Pattern Bits: " << dmp.Match_MaxBits << "\n";
        std::cout << "  Match Threshold: " << dmp.Match_Threshold << "\n";
    }

public:
    void runComprehensiveBenchmark() {
        std::cout << "=== diff-match-patch-qt6 Professional Benchmark Suite ===\n" << std::flush;
        std::cout << "Measuring pure algorithm performance (excluding text generation)\n" << std::flush;
        std::cout << "Algorithm timeout: " << (dmp.Diff_Timeout == 0 ? "DISABLED (max precision)" : "ENABLED") << "\n\n" << std::flush;
        std::cout << "Starting benchmark execution...\n" << std::flush;

        printHeader();
        
        testBasicPerformance();
        testNoiseSensitivity();
        testLargeFilePerformance();
        testRealWorldScenarios();
        testAlgorithmComparison();
        
        printSummary();
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    // Optimize stdio for faster output
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    std::cout << "diff-match-patch-qt6 Professional Benchmark\n";
    std::cout << "Qt Version: " << QT_VERSION_STR << "\n";
    std::cout << "Compiler: ";
#ifdef __GNUC__
    std::cout << "GCC " << __GNUC__ << "." << __GNUC_MINOR__;
#elif defined(_MSC_VER)
    std::cout << "MSVC " << _MSC_VER;
#else
    std::cout << "Unknown";
#endif
    std::cout << "\n\n";
    
    DiffMatchPatchBenchmark benchmark;
    benchmark.runComprehensiveBenchmark();
    
    return 0;
}
