#include "../diff_match_patch.h"
#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QRandomGenerator>
#include <QString>
#include <QList>
#include <QVector>
#include <QPair>

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
        qDebug() << "\n=== diff-match-patch-qt6 Memory Usage Test ===\n";
        
        size_t initialMemory = getCurrentMemoryUsage();
        qDebug() << "Initial memory usage:" << formatMemory(initialMemory);
        qDebug() << "";
        
        testMemoryUsageBySize();
        testMemoryLeaks();
        testLargeFileMemory();
        
        size_t finalMemory = getCurrentMemoryUsage();
        qDebug() << "\nFinal memory usage:" << formatMemory(finalMemory);
        
        if (finalMemory > initialMemory) {
            size_t increase = finalMemory - initialMemory;
            qDebug() << "Memory increase:" << formatMemory(increase);
        } else {
            qDebug() << "No significant memory increase detected";
        }
        
        qDebug() << "\n=== Memory Test Complete ===\n";
    }
    
    void testMemoryUsageBySize() {
        qDebug() << "Testing memory usage by file size:";
        qDebug() << QString("%-15s | %-15s | %-15s").arg("File Size", "Memory Before", "Memory After");
        qDebug() << "----------------------------------------------------";
        
        QList<int> sizes = {1024, 10240, 102400, 1048576}; // 1KB, 10KB, 100KB, 1MB
        
        for (int size : sizes) {
            size_t memBefore = getCurrentMemoryUsage();
            
            QString text1 = generateRandomText(size);
            QString text2 = modifyText(text1, 0.1);
            
            QList<Diff> diffs = dmp.diff_main(text1, text2);
            
            size_t memAfter = getCurrentMemoryUsage();
            
            QString sizeStr = formatMemory(size);
            QString beforeStr = formatMemory(memBefore);
            QString afterStr = formatMemory(memAfter);
            
            qDebug() << QString("%-15s | %-15s | %-15s").arg(sizeStr, beforeStr, afterStr);
            
            // Clean up
            text1.clear();
            text2.clear();
            diffs.clear();
        }
    }
    
    void testMemoryLeaks() {
        qDebug() << "\nTesting for memory leaks (1000 iterations):";
        
        size_t memBefore = getCurrentMemoryUsage();
        
        for (int i = 0; i < 1000; ++i) {
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
        
        size_t memAfter = getCurrentMemoryUsage();
        
        qDebug() << "Memory before iterations:" << formatMemory(memBefore);
        qDebug() << "Memory after iterations:" << formatMemory(memAfter);
        
        if (memAfter > memBefore) {
            size_t increase = memAfter - memBefore;
            qDebug() << "Memory increase:" << formatMemory(increase);
            
            // Check if increase is reasonable (less than 10MB for 1000 iterations)
            if (increase < 10 * 1024 * 1024) {
                qDebug() << "✓ Memory usage appears stable";
            } else {
                qDebug() << "⚠ Potential memory leak detected";
            }
        } else {
            qDebug() << "✓ No memory increase detected";
        }
    }
    
    void testLargeFileMemory() {
        qDebug() << "\nTesting large file memory efficiency:";
        
        // Test with 5MB file
        size_t memBefore = getCurrentMemoryUsage();
        qDebug() << "Memory before large file test:" << formatMemory(memBefore);
        
        QString largeText1 = generateRandomText(5 * 1024 * 1024); // 5MB
        size_t memAfterLoad = getCurrentMemoryUsage();
        qDebug() << "Memory after loading 5MB text:" << formatMemory(memAfterLoad);
        
        QString largeText2 = modifyText(largeText1, 0.05); // 5% changes
        size_t memAfterModify = getCurrentMemoryUsage();
        qDebug() << "Memory after creating modified text:" << formatMemory(memAfterModify);
        
        QElapsedTimer timer;
        timer.start();
        
        QList<Diff> diffs = dmp.diff_main(largeText1, largeText2);
        
        qint64 elapsed = timer.elapsed();
        size_t memAfterDiff = getCurrentMemoryUsage();
        
        qDebug() << "Memory after diff operation:" << formatMemory(memAfterDiff);
        qDebug() << "Diff completed in:" << elapsed << "ms";
        qDebug() << "Number of diff operations:" << diffs.size();
        
        // Calculate memory efficiency
        size_t totalTextSize = largeText1.toUtf8().size() + largeText2.toUtf8().size();
        size_t memoryUsed = memAfterDiff - memBefore;
        double efficiency = (double)totalTextSize / memoryUsed;
        
        qDebug() << "Total text size:" << formatMemory(totalTextSize);
        qDebug() << "Memory used:" << formatMemory(memoryUsed);
        qDebug() << "Memory efficiency ratio:" << QString::number(efficiency, 'f', 2);
        
        // Clean up
        largeText1.clear();
        largeText2.clear();
        diffs.clear();
        
        size_t memAfterCleanup = getCurrentMemoryUsage();
        qDebug() << "Memory after cleanup:" << formatMemory(memAfterCleanup);
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    qDebug() << "🧠 diff-match-patch-qt6 Memory Test";
    qDebug() << "===================================";
    qDebug() << "Platform:" << 
#ifdef Q_OS_WIN
        "Windows";
#elif defined(Q_OS_LINUX)
        "Linux";
#elif defined(Q_OS_MAC)
        "macOS";
#else
        "Unknown";
#endif
    
    MemoryTest memTest;
    memTest.runMemoryTests();
    
    return 0;
}
