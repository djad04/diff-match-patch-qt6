#include "../diff_match_patch.h"
#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QRandomGenerator>
#include <QString>
#include <QList>
#include <QVector>
#include <QPair>
#include <iostream>
#include <iomanip>

#ifdef Q_OS_WIN
#include <windows.h>
#include <psapi.h>
#elif defined(Q_OS_LINUX) || defined(Q_OS_MAC)
#include <sys/resource.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#endif

class MemoryTest {
private:
    diff_match_patch dmp;
    
    size_t getCurrentMemoryUsage() {
#ifdef Q_OS_WIN
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            return pmc.WorkingSetSize;
        }
#elif defined(Q_OS_LINUX)
        FILE* file = fopen("/proc/self/status", "r");
        if (file) {
            char line[128];
            while (fgets(line, 128, file) != nullptr) {
                if (strncmp(line, "VmRSS:", 6) == 0) {
                    int kb;
                    sscanf(line, "VmRSS: %d kB", &kb);
                    fclose(file);
                    return kb * 1024; // Convert to bytes
                }
            }
            fclose(file);
        }
#elif defined(Q_OS_MAC)
        struct rusage usage;
        if (getrusage(RUSAGE_SELF, &usage) == 0) {
            return usage.ru_maxrss; // Already in bytes on macOS
        }
#endif
        return 0;
    }
    
    QString formatMemory(size_t bytes) {
        if (bytes < 1024) {
            return QString("%1 B").arg(bytes);
        } else if (bytes < 1024 * 1024) {
            return QString("%1 KB").arg(bytes / 1024.0, 0, 'f', 1);
        } else {
            return QString("%1 MB").arg(bytes / (1024.0 * 1024.0), 0, 'f', 1);
        }
    }
    
    QString generateRandomText(int size) {
        QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 \n";
        QString result;
        result.reserve(size);
        
        for (int i = 0; i < size; ++i) {
            result += chars[QRandomGenerator::global()->bounded(chars.length())];
        }
        return result;
    }
    
    QString modifyText(const QString& original, double changePercent) {
        QString modified = original;
        int changeCount = static_cast<int>(original.length() * changePercent);
        
        for (int i = 0; i < changeCount; ++i) {
            int pos = QRandomGenerator::global()->bounded(modified.length());
            if (QRandomGenerator::global()->bounded(3) == 0) {
                modified.insert(pos, 'X');
            } else if (QRandomGenerator::global()->bounded(2) == 0 && pos < modified.length()) {
                modified.remove(pos, 1);
            } else if (pos < modified.length()) {
                modified[pos] = 'Y';
            }
        }
        return modified;
    }

public:
    void runMemoryTests() {
        std::cout << "=== diff-match-patch-qt6 Memory Usage Test ===\n\n";

        size_t initialMemory = getCurrentMemoryUsage();
        std::cout << "Initial memory usage: " << formatMemory(initialMemory).toStdString() << "\n\n";

        testMemoryUsageBySize();
        testMemoryLeaks();
        testLargeFileMemory();

        size_t finalMemory = getCurrentMemoryUsage();
        std::cout << "\nFinal memory usage: " << formatMemory(finalMemory).toStdString() << "\n";

        if (finalMemory > initialMemory) {
            size_t increase = finalMemory - initialMemory;
            std::cout << "Memory increase: " << formatMemory(increase).toStdString() << "\n";
        } else {
            std::cout << "No significant memory increase detected\n";
        }

        std::cout << "\n=== Memory Test Complete ===\n";
    }
    
    void testMemoryUsageBySize() {
        std::cout << "Testing memory usage by file size:\n";
        std::cout << std::left << std::setw(15) << "File Size"
                  << " | " << std::setw(15) << "Memory Before"
                  << " | " << std::setw(15) << "Memory After" << "\n";
        std::cout << "----------------------------------------------------\n";

        QList<int> sizes = {1024, 10240, 51200}; // 1KB, 10KB, 50KB (reduced for faster testing)

        for (int size : sizes) {
            size_t memBefore = getCurrentMemoryUsage();

            QString text1 = generateRandomText(size);
            QString text2 = modifyText(text1, 0.1);

            QList<Diff> diffs = dmp.diff_main(text1, text2);

            size_t memAfter = getCurrentMemoryUsage();

            std::string sizeStr = formatMemory(size).toStdString();
            std::string beforeStr = formatMemory(memBefore).toStdString();
            std::string afterStr = formatMemory(memAfter).toStdString();

            std::cout << std::left << std::setw(15) << sizeStr
                      << " | " << std::setw(15) << beforeStr
                      << " | " << std::setw(15) << afterStr << "\n";

            // Clean up
            text1.clear();
            text2.clear();
            diffs.clear();
        }
    }
    
    void testMemoryLeaks() {
        std::cout << "\nTesting for memory leaks (100 iterations):\n";

        size_t memBefore = getCurrentMemoryUsage();

        for (int i = 0; i < 100; ++i) {  // Reduced from 1000 to 100
            if (i % 20 == 0) {
                std::cout << "  Progress: " << i << "/100 iterations...\n" << std::flush;
            }

            QString text1 = generateRandomText(1000);
            QString text2 = modifyText(text1, 0.1);

            QList<Diff> diffs = dmp.diff_main(text1, text2);
            QList<Patch> patches = dmp.patch_make(text1, text2);
            QPair<QString, QVector<bool>> result = dmp.patch_apply(patches, text1);

            // Force cleanup
            text1.clear();
            text2.clear();
            diffs.clear();
            patches.clear();
        }
        std::cout << "  Progress: 100/100 iterations complete!\n";
        
        size_t memAfter = getCurrentMemoryUsage();
        
        std::cout << "Memory before iterations: " << formatMemory(memBefore).toStdString() << "\n";
        std::cout << "Memory after iterations: " << formatMemory(memAfter).toStdString() << "\n";

        if (memAfter > memBefore) {
            size_t increase = memAfter - memBefore;
            std::cout << "Memory increase: " << formatMemory(increase).toStdString() << "\n";

            // Check if increase is reasonable (less than 5MB for 100 iterations)
            if (increase < 5 * 1024 * 1024) {
                std::cout << "PASS: Memory usage appears stable\n";
            } else {
                std::cout << "WARNING: Potential memory leak detected\n";
            }
        } else {
            std::cout << "PASS: No memory increase detected\n";
        }
    }
    
    void testLargeFileMemory() {
        std::cout << "\nTesting large file memory efficiency:\n";

        // Test with 1MB file (reduced from 5MB)
        size_t memBefore = getCurrentMemoryUsage();
        std::cout << "Memory before large file test: " << formatMemory(memBefore).toStdString() << "\n";

        std::cout << "  Loading 1MB text... " << std::flush;
        QString largeText1 = generateRandomText(1024 * 1024); // 1MB
        size_t memAfterLoad = getCurrentMemoryUsage();
        std::cout << "Done!\n";
        std::cout << "Memory after loading 1MB text: " << formatMemory(memAfterLoad).toStdString() << "\n";

        std::cout << "  Creating modified text... " << std::flush;
        QString largeText2 = modifyText(largeText1, 0.01); // 1% changes (realistic scenario)
        size_t memAfterModify = getCurrentMemoryUsage();
        std::cout << "Done!\n";
        std::cout << "Memory after creating modified text: " << formatMemory(memAfterModify).toStdString() << "\n";
        std::cout << "Note: Testing with 1% changes (realistic code editing scenario)\n";

        std::cout << "  Running diff algorithm... " << std::flush;
        QElapsedTimer timer;
        timer.start();

        QList<Diff> diffs = dmp.diff_main(largeText1, largeText2);

        qint64 elapsed = timer.elapsed();
        size_t memAfterDiff = getCurrentMemoryUsage();
        std::cout << "Done!\n";

        std::cout << "Memory after diff operation: " << formatMemory(memAfterDiff).toStdString() << "\n";
        std::cout << "Diff completed in: " << elapsed << " ms\n";
        std::cout << "Number of diff operations: " << diffs.size() << "\n";

        // Calculate memory efficiency
        size_t totalTextSize = largeText1.toUtf8().size() + largeText2.toUtf8().size();
        size_t memoryUsed = memAfterDiff - memBefore;
        double efficiency = (double)totalTextSize / memoryUsed;

        std::cout << "Total text size: " << formatMemory(totalTextSize).toStdString() << "\n";
        std::cout << "Memory used: " << formatMemory(memoryUsed).toStdString() << "\n";

        // Clean up
        largeText1.clear();
        largeText2.clear();
        diffs.clear();

        size_t memAfterCleanup = getCurrentMemoryUsage();
        std::cout << "Memory after cleanup: " << formatMemory(memAfterCleanup).toStdString() << "\n";
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    // Optimize stdio for faster output
    std::ios::sync_with_stdio(false);

    std::cout << "diff-match-patch-qt6 Memory Test\n";
    std::cout << "=================================\n";
    std::cout << "Platform: ";
#ifdef Q_OS_WIN
    std::cout << "Windows";
#elif defined(Q_OS_LINUX)
    std::cout << "Linux";
#elif defined(Q_OS_MAC)
    std::cout << "macOS";
#else
    std::cout << "Unknown";
#endif
    std::cout << "\n\n";

    MemoryTest memTest;
    memTest.runMemoryTests();

    return 0;
}
