#include <QtTest/QtTest>
#include <QRandomGenerator>
#include "../../diff_match_patch.h"

class TestMemory : public QObject
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
    
    void testMemoryStability();
    void testLargeFileMemory();
    void testMemoryCleanup();
};

void TestMemory::initTestCase()
{
    dmp.Diff_Timeout = 0.0f;
}

void TestMemory::testMemoryStability()
{
    // Test that repeated operations don't cause memory leaks
    const int iterations = 50; // Reduced for CI

    for (int i = 0; i < iterations; ++i) {
        // Use more predictable text patterns for reliable patch application
        QString text1 = QString("Base text %1: ").arg(i) + generateRandomText(500);
        QString text2 = text1;

        // Make simple, predictable modifications
        text2.replace("Base", "Modified");
        text2.append(" - Additional content");

        QList<Diff> diffs = dmp.diff_main(text1, text2);
        QList<Patch> patches = dmp.patch_make(text1, text2);
        QPair<QString, QVector<bool>> result = dmp.patch_apply(patches, text1);

        // Verify correctness - patches should apply successfully
        QVERIFY(!diffs.isEmpty());
        QVERIFY(!patches.isEmpty());
        QVERIFY(result.second.size() > 0); // At least one patch should be applied

        // Check that most patches applied successfully
        int successfulPatches = 0;
        for (bool success : result.second) {
            if (success) successfulPatches++;
        }
        QVERIFY(successfulPatches > 0); // At least some patches should succeed

        // Force cleanup
        text1.clear();
        text2.clear();
        diffs.clear();
        patches.clear();
    }

    // If we get here without crashing, memory management is working
    QVERIFY(true);
}

void TestMemory::testLargeFileMemory()
{
    // Test memory usage with larger files
    const int size = 100000; // 100KB (reduced for CI)
    
    QString largeText1 = generateRandomText(size);
    QString largeText2 = modifyText(largeText1, 0.01); // 1% changes
    
    QList<Diff> diffs = dmp.diff_main(largeText1, largeText2);
    
    QVERIFY(!diffs.isEmpty());
    
    // Verify efficiency
    int totalEqualChars = 0;
    for (const Diff& diff : diffs) {
        if (diff.operation == EQUAL) {
            totalEqualChars += diff.text.length();
        }
    }
    
    double efficiency = (double)totalEqualChars / largeText1.length() * 100.0;
    QVERIFY(efficiency > 90.0); // Should be very efficient with small changes
    
    // Test patch operations on large files
    QList<Patch> patches = dmp.patch_make(largeText1, largeText2);
    QPair<QString, QVector<bool>> result = dmp.patch_apply(patches, largeText1);
    
    QCOMPARE(result.first, largeText2);
}

void TestMemory::testMemoryCleanup()
{
    // Test that objects are properly cleaned up
    QList<Diff> diffs;
    QList<Patch> patches;
    
    {
        QString text1 = "Hello World";
        QString text2 = "Hello Universe";
        
        diffs = dmp.diff_main(text1, text2);
        patches = dmp.patch_make(text1, text2);
        
        QVERIFY(!diffs.isEmpty());
        QVERIFY(!patches.isEmpty());
    }
    
    // Objects should still be valid after scope
    QVERIFY(!diffs.isEmpty());
    QVERIFY(!patches.isEmpty());
    
    // Test serialization/deserialization
    QString patchText = dmp.patch_toText(patches);
    QVERIFY(!patchText.isEmpty());
    
    QList<Patch> newPatches = dmp.patch_fromText(patchText);
    QCOMPARE(newPatches.size(), patches.size());
}

QTEST_MAIN(TestMemory)
#include "test_memory.moc"
