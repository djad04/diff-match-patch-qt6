#include <QtTest/QtTest>
#include <QRandomGenerator>
#include "../../diff_match_patch.h"

class TestPerformance : public QObject
{
    Q_OBJECT

private:
    diff_match_patch dmp;
    
    QString generateRandomText(int size) {
        const QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 \n\t";
        QString result;
        result.reserve(size);
        
        for (int i = 0; i < size; ++i) {
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

private slots:
    void initTestCase();
    
    void testSmallFiles();
    void testMediumFiles();
    void testLargeFiles();
    void testNoiseSensitivity();
    void testRealWorldScenarios();
};

void TestPerformance::initTestCase()
{
    dmp.Diff_Timeout = 0.0f;  // No timeout for maximum precision
}

void TestPerformance::testSmallFiles()
{
    QList<int> sizes = {100, 1024, 10240}; // 100B, 1KB, 10KB
    
    for (int size : sizes) {
        QString text1 = generateRandomText(size);
        QString text2 = modifyText(text1, 0.1); // 10% changes
        
        QBENCHMARK {
            QList<Diff> diffs = dmp.diff_main(text1, text2);
            Q_UNUSED(diffs)
        }
        
        // Verify it actually works
        QList<Diff> diffs = dmp.diff_main(text1, text2);
        QVERIFY(!diffs.isEmpty());
    }
}

void TestPerformance::testMediumFiles()
{
    QList<int> sizes = {51200, 102400}; // 50KB, 100KB
    
    for (int size : sizes) {
        QString text1 = generateRandomText(size);
        QString text2 = modifyText(text1, 0.05); // 5% changes
        
        QBENCHMARK {
            QList<Diff> diffs = dmp.diff_main(text1, text2);
            Q_UNUSED(diffs)
        }
        
        // Verify correctness
        QList<Diff> diffs = dmp.diff_main(text1, text2);
        QVERIFY(!diffs.isEmpty());
        
        // Check efficiency
        int totalEqualChars = 0;
        for (const Diff& diff : diffs) {
            if (diff.operation == EQUAL) {
                totalEqualChars += diff.text.length();
            }
        }
        double efficiency = (double)totalEqualChars / text1.length() * 100.0;
        QVERIFY(efficiency > 80.0); // Should maintain good efficiency
    }
}

void TestPerformance::testLargeFiles()
{
    const int size = 512000; // 500KB
    QString text1 = generateRandomText(size);
    QString text2 = modifyText(text1, 0.01); // 1% changes (realistic)
    
    QBENCHMARK {
        QList<Diff> diffs = dmp.diff_main(text1, text2);
        Q_UNUSED(diffs)
    }
    
    // Verify it works and is efficient
    QList<Diff> diffs = dmp.diff_main(text1, text2);
    QVERIFY(!diffs.isEmpty());
    
    int totalEqualChars = 0;
    for (const Diff& diff : diffs) {
        if (diff.operation == EQUAL) {
            totalEqualChars += diff.text.length();
        }
    }
    double efficiency = (double)totalEqualChars / text1.length() * 100.0;
    QVERIFY(efficiency > 90.0); // Should be very efficient with small changes
}

void TestPerformance::testNoiseSensitivity()
{
    const int testSize = 50000; // 50KB
    QList<double> noiseRates = {0.01, 0.05, 0.10, 0.20};
    
    for (double rate : noiseRates) {
        QString text1 = generateRandomText(testSize);
        QString text2 = modifyText(text1, rate);
        
        QBENCHMARK {
            QList<Diff> diffs = dmp.diff_main(text1, text2);
            Q_UNUSED(diffs)
        }
        
        // Verify correctness
        QList<Diff> diffs = dmp.diff_main(text1, text2);
        QVERIFY(!diffs.isEmpty());
    }
}

void TestPerformance::testRealWorldScenarios()
{
    // JavaScript code comparison
    QString code1 = R"(
function calculateTotal(items) {
    let total = 0;
    for (let item of items) {
        total += item.price;
    }
    return total;
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
)";
    
    QBENCHMARK {
        QList<Diff> diffs = dmp.diff_main(code1, code2);
        Q_UNUSED(diffs)
    }
    
    // Configuration file
    QString config1 = QString("server.port=8080\nserver.host=localhost\ndebug=false\n").repeated(100);
    QString config2 = config1;
    config2.replace("8080", "9090");
    config2.replace("localhost", "0.0.0.0");
    
    QBENCHMARK {
        QList<Diff> diffs = dmp.diff_main(config1, config2);
        Q_UNUSED(diffs)
    }
    
    // Unicode text
    QString unicode1 = QString("Hello 世界 🌍 مرحبا ").repeated(200);
    QString unicode2 = unicode1;
    unicode2.replace("世界", "Universe");
    unicode2.replace("🌍", "🚀");
    
    QBENCHMARK {
        QList<Diff> diffs = dmp.diff_main(unicode1, unicode2);
        Q_UNUSED(diffs)
    }
}

QTEST_MAIN(TestPerformance)
#include "test_performance.moc"
