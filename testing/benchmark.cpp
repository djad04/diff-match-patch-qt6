#include "../diff_match_patch.h"
#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QFile>
#include <QTextStream>
#include <QRandomGenerator>
#include <QString>
#include <QList>
#include <QVector>
#include <QPair>
#include <iostream>

class DiffBenchmark {
private:
    diff_match_patch dmp;

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
                // Insert character
                modified.insert(pos, 'X');
            } else if (QRandomGenerator::global()->bounded(2) == 0 && pos < modified.length()) {
                // Delete character
                modified.remove(pos, 1);
            } else if (pos < modified.length()) {
                // Modify character
                modified[pos] = 'Y';
            }
        }
        return modified;
    }

    QString modifyTextPreserveLength(const QString& original, double changePercent) {
        QString modified = original;
        int changeCount = static_cast<int>(original.length() * changePercent);

        for (int i = 0; i < changeCount; ++i) {
            int pos = QRandomGenerator::global()->bounded(modified.length());
            // Only modify characters, don't insert/delete to preserve structure
            modified[pos] = 'X';
        }
        return modified;
    }

    void printResult(const QString& testName, int fileSize, qint64 timeMs, int operations = 0) {
        QString sizeStr;
        if (fileSize < 1024) {
            sizeStr = QString("%1 B").arg(fileSize);
        } else if (fileSize < 1024 * 1024) {
            sizeStr = QString("%1 KB").arg(fileSize / 1024.0, 0, 'f', 1);
        } else {
            sizeStr = QString("%1 MB").arg(fileSize / (1024.0 * 1024.0), 0, 'f', 1);
        }

        qDebug().noquote() << QString("%1 | %2 | %3 ms").arg(testName, -25).arg(sizeStr, 8).arg(timeMs, 6);
        if (operations > 0) {
            qDebug().noquote() << QString("%1 | %2 | %3 operations").arg("", -25).arg("", 8).arg(operations);
        }
    }

public:
    void runAllBenchmarks() {
        qDebug() << "\n=== diff-match-patch-qt6 Performance Benchmark ===\n";
        qDebug().noquote() << QString("%1 | %2 | %3").arg("Test", -25).arg("Size", 8).arg("Time", 8);
        qDebug() << "------------------------------------------------";

        // Basic performance tests
        testBasicPerformance();

        qDebug() << "------------------------------------------------";

        // Controlled noise rate analysis
        testControlledNoiseRates();

        qDebug() << "------------------------------------------------";

        // Unicode tests
        testUnicodePerformance();

        qDebug() << "------------------------------------------------";

        // Real-world scenarios
        testRealWorldScenarios();

        qDebug() << "------------------------------------------------";

        // Investigation of suspicious large file behavior
        testSuspiciousLargeFiles();

        qDebug() << "------------------------------------------------";

        // Test comparison: Old vs New parameters
        testParameterComparison();

        qDebug() << "------------------------------------------------";

        // Stress tests
        testStressScenarios();

        qDebug() << "\n=== Benchmark Complete ===\n";
    }

    void testBasicPerformance() {
        QList<int> sizes = {100, 1024, 10240, 102400}; // 100B, 1KB, 10KB, 100KB (remove suspicious 1MB)

        for (int size : sizes) {
            QString text1 = generateRandomText(size);
            QString text2 = modifyText(text1, 0.1); // 10% changes

            QElapsedTimer timer;
            timer.start();

            QList<Diff> diffs = dmp.diff_main(text1, text2);

            qint64 elapsed = timer.elapsed();
            printResult("Basic Diff", size, elapsed, diffs.size());
        }
    }

    void testControlledNoiseRates() {
        qDebug() << "=== Controlled Noise Rate Analysis ===";

        const int testSize = 50000; // 50KB for consistent comparison
        QList<double> noiseRates = {0.01, 0.05, 0.10, 0.20, 0.50}; // 1%, 5%, 10%, 20%, 50%

        for (double noiseRate : noiseRates) {
            QString text1 = generateRandomText(testSize);
            QString text2 = modifyTextPreserveLength(text1, noiseRate); // Use length-preserving for fair comparison

            // Count actual changes
            int actualChanges = 0;
            for (int i = 0; i < text1.length() && i < text2.length(); ++i) {
                if (text1[i] != text2[i]) actualChanges++;
            }
            double actualRate = (double)actualChanges / text1.length() * 100.0;

            QElapsedTimer timer;
            timer.start();

            QList<Diff> diffs = dmp.diff_main(text1, text2);

            qint64 elapsed = timer.elapsed();

            // Analyze operations
            int equalOps = 0, insertOps = 0, deleteOps = 0;
            int totalEqualChars = 0, totalInsertChars = 0, totalDeleteChars = 0;
            for (const Diff& diff : diffs) {
                switch (diff.operation) {
                    case EQUAL: equalOps++; totalEqualChars += diff.text.length(); break;
                    case INSERT: insertOps++; totalInsertChars += diff.text.length(); break;
                    case DELETE: deleteOps++; totalDeleteChars += diff.text.length(); break;
                }
            }

            qDebug().noquote() << QString("Noise %1%% | %2 KB | %3 ms | %4 ops | Actual: %5%%")
                .arg(noiseRate * 100, 4, 'f', 1)
                .arg(testSize / 1024.0, 4, 'f', 1)
                .arg(elapsed, 6)
                .arg(diffs.size(), 4)
                .arg(actualRate, 5, 'f', 1);

            qDebug().noquote() << QString("         | EQUAL: %1 ops (%2 chars) | INS: %3 ops (%4 chars) | DEL: %5 ops (%6 chars)")
                .arg(equalOps, 2).arg(totalEqualChars, 6)
                .arg(insertOps, 2).arg(totalInsertChars, 6)
                .arg(deleteOps, 2).arg(totalDeleteChars, 6);
        }
    }

    void testUnicodePerformance() {
        // Test with Unicode characters
        QString unicode1 = QString("Hello 世界 🌍 مرحبا ").repeated(100);
        QString unicode2 = unicode1;
        unicode2.replace("世界", "Universe");
        unicode2.replace("🌍", "🚀");

        QElapsedTimer timer;
        timer.start();

        QList<Diff> diffs = dmp.diff_main(unicode1, unicode2);

        qint64 elapsed = timer.elapsed();
        printResult("Unicode Diff", unicode1.toUtf8().size(), elapsed, diffs.size());
    }



    void testRealWorldScenarios() {
        // Code comparison scenario
        QString code1 = R"(
function calculateTotal(items) {
    let total = 0;
    for (let item of items) {
        total += item.price;
    }
    return total;
}

class ShoppingCart {
    constructor() {
        this.items = [];
    }

    addItem(item) {
        this.items.push(item);
    }

    getTotal() {
        return calculateTotal(this.items);
    }
}
)";

        QString code2 = R"(
function calculateTotal(items, tax = 0.1) {
    let subtotal = 0;
    for (let item of items) {
        subtotal += item.price * item.quantity;
    }
    return subtotal * (1 + tax);
}

class ShoppingCart {
    constructor() {
        this.items = [];
        this.discount = 0;
    }

    addItem(item) {
        this.items.push(item);
    }

    applyDiscount(percent) {
        this.discount = percent;
    }

    getTotal() {
        const total = calculateTotal(this.items);
        return total * (1 - this.discount);
    }
}
)";

        QElapsedTimer timer;
        timer.start();

        QList<Diff> diffs = dmp.diff_main(code1, code2);

        qint64 elapsed = timer.elapsed();
        printResult("Code Diff", code1.toUtf8().size(), elapsed, diffs.size());

        // Configuration file scenario
        QString config1 = QString("server.port=8080\nserver.host=localhost\ndebug=false\n").repeated(50);
        QString config2 = config1;
        config2.replace("8080", "9090");
        config2.replace("localhost", "0.0.0.0");
        config2.replace("false", "true");
        config2.append("logging.level=info\nlogging.file=/var/log/app.log\n");

        timer.restart();
        diffs = dmp.diff_main(config1, config2);
        elapsed = timer.elapsed();

        printResult("Config Diff", config1.toUtf8().size(), elapsed, diffs.size());
    }

    void testSuspiciousLargeFiles() {
        qDebug() << "=== Investigation: Large Files with NO TIMEOUT ===";

        QList<int> largeSizes = {102400, 512000, 1048576, 2097152}; // 100KB, 500KB, 1MB, 2MB

        for (int size : largeSizes) {
            QString text1 = generateRandomText(size);
            QString text2 = modifyTextPreserveLength(text1, 0.05); // 5% changes only

            // Count actual changes
            int actualChanges = 0;
            for (int i = 0; i < text1.length() && i < text2.length(); ++i) {
                if (text1[i] != text2[i]) actualChanges++;
            }
            double actualRate = (double)actualChanges / text1.length() * 100.0;

            QElapsedTimer timer;
            timer.start();

            QList<Diff> diffs = dmp.diff_main(text1, text2);

            qint64 elapsed = timer.elapsed();

            // Detailed analysis
            int equalOps = 0, insertOps = 0, deleteOps = 0;
            int totalEqualChars = 0, totalInsertChars = 0, totalDeleteChars = 0;
            for (const Diff& diff : diffs) {
                switch (diff.operation) {
                    case EQUAL: equalOps++; totalEqualChars += diff.text.length(); break;
                    case INSERT: insertOps++; totalInsertChars += diff.text.length(); break;
                    case DELETE: deleteOps++; totalDeleteChars += diff.text.length(); break;
                }
            }

            double efficiency = (double)totalEqualChars / size * 100.0;

            qDebug().noquote() << QString("Size %1 KB | %2 ms | %3 ops | %4%% actual changes | %5%% efficiency")
                .arg(size / 1024, 4)
                .arg(elapsed, 6)
                .arg(diffs.size(), 4)
                .arg(actualRate, 4, 'f', 1)
                .arg(efficiency, 4, 'f', 1);

            if (diffs.size() <= 10) {
                qDebug().noquote() << QString("  ⚠️  SUSPICIOUS: Only %1 operations for %2KB file!")
                    .arg(diffs.size()).arg(size / 1024);
                qDebug().noquote() << QString("  EQUAL: %1 chars (%2%%) | INSERT: %3 chars | DELETE: %4 chars")
                    .arg(totalEqualChars).arg(efficiency, 0, 'f', 1)
                    .arg(totalInsertChars).arg(totalDeleteChars);
            }
        }
    }

    void testParameterComparison() {
        qDebug() << "=== Parameter Comparison: Old vs New Settings ===";

        const int testSize = 1048576; // 1MB test file
        QString text1 = generateRandomText(testSize);
        QString text2 = modifyTextPreserveLength(text1, 0.05); // 5% changes

        // Test with OLD parameters (timeout-limited)
        diff_match_patch oldDmp;
        oldDmp.Diff_Timeout = 1.0f;        // Original timeout
        oldDmp.Match_Distance = 1000;      // Original distance
        oldDmp.Match_MaxBits = 32;         // Original max bits

        QElapsedTimer timer;
        timer.start();
        QList<Diff> oldDiffs = oldDmp.diff_main(text1, text2);
        qint64 oldTime = timer.elapsed();

        // Analyze old results
        int oldEqual = 0, oldInsert = 0, oldDelete = 0;
        int oldEqualChars = 0, oldInsertChars = 0, oldDeleteChars = 0;
        for (const Diff& diff : oldDiffs) {
            switch (diff.operation) {
                case EQUAL: oldEqual++; oldEqualChars += diff.text.length(); break;
                case INSERT: oldInsert++; oldInsertChars += diff.text.length(); break;
                case DELETE: oldDelete++; oldDeleteChars += diff.text.length(); break;
            }
        }
        double oldEfficiency = (double)oldEqualChars / testSize * 100.0;

        // Test with NEW parameters (no timeout)
        diff_match_patch newDmp; // Uses new default parameters

        timer.restart();
        QList<Diff> newDiffs = newDmp.diff_main(text1, text2);
        qint64 newTime = timer.elapsed();

        // Analyze new results
        int newEqual = 0, newInsert = 0, newDelete = 0;
        int newEqualChars = 0, newInsertChars = 0, newDeleteChars = 0;
        for (const Diff& diff : newDiffs) {
            switch (diff.operation) {
                case EQUAL: newEqual++; newEqualChars += diff.text.length(); break;
                case INSERT: newInsert++; newInsertChars += diff.text.length(); break;
                case DELETE: newDelete++; newDeleteChars += diff.text.length(); break;
            }
        }
        double newEfficiency = (double)newEqualChars / testSize * 100.0;

        // Display comparison
        qDebug().noquote() << QString("OLD (timeout=1s)  | 1024 KB | %1 ms | %2 ops | %3%% efficiency")
            .arg(oldTime, 6).arg(oldDiffs.size(), 4).arg(oldEfficiency, 4, 'f', 1);
        qDebug().noquote() << QString("NEW (no timeout)  | 1024 KB | %1 ms | %2 ops | %3%% efficiency")
            .arg(newTime, 6).arg(newDiffs.size(), 4).arg(newEfficiency, 4, 'f', 1);

        // Show improvement
        if (newEfficiency > oldEfficiency + 10) {
            qDebug().noquote() << QString("✅ IMPROVEMENT: +%1%% efficiency, %2x more operations")
                .arg(newEfficiency - oldEfficiency, 0, 'f', 1)
                .arg((double)newDiffs.size() / oldDiffs.size(), 0, 'f', 1);
        } else if (newTime > oldTime * 3) {
            qDebug().noquote() << QString("⚠️  SLOWER: %1x longer processing time")
                .arg((double)newTime / oldTime, 0, 'f', 1);
        } else {
            qDebug().noquote() << QString("ℹ️  Similar results with different parameters");
        }
    }

    void testStressScenarios() {
        // Large file test - use length-preserving changes for better algorithm performance
        QString largeText1 = generateRandomText(5 * 1024 * 1024); // 5MB
        QString largeText2 = modifyTextPreserveLength(largeText1, 0.05); // 5% changes

        // Debug: Check actual change pattern
        int actualChanges = 0;
        int sampleSize = (largeText1.length() < 1000) ? largeText1.length() : 1000;
        for (int i = 0; i < sampleSize; ++i) {
            if (i < largeText2.length() && largeText1[i] != largeText2[i]) {
                actualChanges++;
            }
        }
        double actualChangeRate = (double)actualChanges / sampleSize * 100.0;

        QElapsedTimer timer;
        timer.start();

        QList<Diff> diffs = dmp.diff_main(largeText1, largeText2);

        qint64 elapsed = timer.elapsed();
        printResult("Large File (5MB)", largeText1.size(), elapsed, diffs.size());

        // Analyze the diff structure for large files
        int equalOps = 0, insertOps = 0, deleteOps = 0;
        int totalEqualChars = 0, totalInsertChars = 0, totalDeleteChars = 0;
        for (const Diff& diff : diffs) {
            switch (diff.operation) {
                case EQUAL:
                    equalOps++;
                    totalEqualChars += diff.text.length();
                    break;
                case INSERT:
                    insertOps++;
                    totalInsertChars += diff.text.length();
                    break;
                case DELETE:
                    deleteOps++;
                    totalDeleteChars += diff.text.length();
                    break;
            }
        }
        qDebug().noquote() << QString("%1 | %2 | EQUAL: %3 ops (%4 chars)").arg("", -25).arg("", 8).arg(equalOps).arg(totalEqualChars);
        qDebug().noquote() << QString("%1 | %2 | INSERT: %3 ops (%4 chars)").arg("", -25).arg("", 8).arg(insertOps).arg(totalInsertChars);
        qDebug().noquote() << QString("%1 | %2 | DELETE: %3 ops (%4 chars)").arg("", -25).arg("", 8).arg(deleteOps).arg(totalDeleteChars);
        qDebug().noquote() << QString("%1 | %2 | Actual change rate: %3%").arg("", -25).arg("", 8).arg(actualChangeRate, 0, 'f', 1);

        // Many small changes
        QString text1 = generateRandomText(100000); // 100KB
        QString text2 = modifyText(text1, 0.5); // 50% changes

        timer.restart();
        diffs = dmp.diff_main(text1, text2);
        elapsed = timer.elapsed();

        printResult("Many Changes (50%)", text1.size(), elapsed, diffs.size());

        // Compare with destructive changes (original method)
        qDebug() << "------------------------------------------------";
        qDebug() << "Comparison: Length-changing vs Length-preserving modifications";

        QString testText = generateRandomText(1024 * 1024); // 1MB for comparison
        QString destructiveChanges = modifyText(testText, 0.05);
        QString preservingChanges = modifyTextPreserveLength(testText, 0.05);

        timer.restart();
        QList<Diff> destructiveDiffs = dmp.diff_main(testText, destructiveChanges);
        qint64 destructiveTime = timer.elapsed();

        timer.restart();
        QList<Diff> preservingDiffs = dmp.diff_main(testText, preservingChanges);
        qint64 preservingTime = timer.elapsed();

        qDebug().noquote() << QString("Length-changing (5%%)    | %1 MB | %2 ms | %3 ops").arg(1.0, 0, 'f', 1).arg(destructiveTime, 6).arg(destructiveDiffs.size());
        qDebug().noquote() << QString("Length-preserving (5%%) | %1 MB | %2 ms | %3 ops").arg(1.0, 0, 'f', 1).arg(preservingTime, 6).arg(preservingDiffs.size());

        // Demonstrate ideal half-match scenario
        qDebug() << "------------------------------------------------";
        qDebug() << "Ideal Half-Match Scenario (localized changes):";

        QString idealText1 = generateRandomText(2 * 1024 * 1024); // 2MB
        QString idealText2 = idealText1;
        // Make changes only in the middle 10% of the file
        int startPos = idealText1.length() * 0.45; // Start at 45%
        int endPos = idealText1.length() * 0.55;   // End at 55%
        for (int i = startPos; i < endPos && i < idealText2.length(); i += 10) {
            idealText2[i] = 'X'; // Change every 10th character in the middle section
        }

        timer.restart();
        QList<Diff> idealDiffs = dmp.diff_main(idealText1, idealText2);
        qint64 idealTime = timer.elapsed();

        qDebug().noquote() << QString("Localized changes (2MB)  | %1 MB | %2 ms | %3 ops").arg(2.0, 0, 'f', 1).arg(idealTime, 6).arg(idealDiffs.size());

        // Analyze this ideal case
        int idealEqual = 0, idealInsert = 0, idealDelete = 0;
        int idealEqualChars = 0, idealInsertChars = 0, idealDeleteChars = 0;
        for (const Diff& diff : idealDiffs) {
            switch (diff.operation) {
                case EQUAL: idealEqual++; idealEqualChars += diff.text.length(); break;
                case INSERT: idealInsert++; idealInsertChars += diff.text.length(); break;
                case DELETE: idealDelete++; idealDeleteChars += diff.text.length(); break;
            }
        }
        qDebug().noquote() << QString("%1 | %2 | EQUAL: %3 ops (%4 chars)").arg("", -25).arg("", 8).arg(idealEqual).arg(idealEqualChars);
        qDebug().noquote() << QString("%1 | %2 | INSERT: %3 ops (%4 chars)").arg("", -25).arg("", 8).arg(idealInsert).arg(idealInsertChars);
        qDebug().noquote() << QString("%1 | %2 | DELETE: %3 ops (%4 chars)").arg("", -25).arg("", 8).arg(idealDelete).arg(idealDeleteChars);
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    qDebug() << "Starting diff-match-patch-qt6 benchmark...";
    qDebug() << "Qt Version:" << QT_VERSION_STR;
    qDebug() << "Compiler:" <<
#ifdef __GNUC__
        "GCC" << __GNUC__ << "." << __GNUC_MINOR__;
#elif defined(_MSC_VER)
        "MSVC" << _MSC_VER;
#else
        "Unknown";
#endif

    DiffBenchmark benchmark;
    benchmark.runAllBenchmarks();

    return 0;
}
