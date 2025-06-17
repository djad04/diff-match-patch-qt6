#include <QtTest/QtTest>
#include "../../diff_match_patch.h"

class TestDiff : public QObject
{
    Q_OBJECT

private:
    diff_match_patch dmp;

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    void testBasicInsertion();
    void testBasicDeletion();
    void testBasicReplacement();
    void testIdenticalTexts();
    void testComplexChanges();
    void testUnicodeSupport();
    void testLargeTexts();
    void testLineBasedDiff();
};

void TestDiff::initTestCase()
{
    // Setup test configuration
    dmp.Diff_Timeout = 0.0f;  // No timeout for tests
}

void TestDiff::cleanupTestCase()
{
    // Cleanup if needed
}

void TestDiff::testBasicInsertion()
{
    QString text1 = "Hello World";
    QString text2 = "Hello Beautiful World";
    
    QList<Diff> diffs = dmp.diff_main(text1, text2);
    
    QVERIFY(!diffs.isEmpty());
    QCOMPARE(diffs.size(), 3);
    QCOMPARE(diffs[0].operation, EQUAL);
    QCOMPARE(diffs[0].text, QString("Hello "));
    QCOMPARE(diffs[1].operation, INSERT);
    QCOMPARE(diffs[1].text, QString("Beautiful "));
    QCOMPARE(diffs[2].operation, EQUAL);
    QCOMPARE(diffs[2].text, QString("World"));
}

void TestDiff::testBasicDeletion()
{
    QString text1 = "Hello Beautiful World";
    QString text2 = "Hello World";
    
    QList<Diff> diffs = dmp.diff_main(text1, text2);
    
    QVERIFY(!diffs.isEmpty());
    QCOMPARE(diffs.size(), 3);
    QCOMPARE(diffs[1].operation, DELETE);
    QCOMPARE(diffs[1].text, QString("Beautiful "));
}

void TestDiff::testBasicReplacement()
{
    QString text1 = "Hello World";
    QString text2 = "Hello Universe";
    
    QList<Diff> diffs = dmp.diff_main(text1, text2);
    
    QVERIFY(!diffs.isEmpty());
    QVERIFY(diffs.size() >= 2);
    
    // Should contain both DELETE and INSERT operations
    bool hasDelete = false, hasInsert = false;
    for (const Diff& diff : diffs) {
        if (diff.operation == DELETE) hasDelete = true;
        if (diff.operation == INSERT) hasInsert = true;
    }
    QVERIFY(hasDelete && hasInsert);
}

void TestDiff::testIdenticalTexts()
{
    QString text = "Identical Text Content";
    
    QList<Diff> diffs = dmp.diff_main(text, text);
    
    QCOMPARE(diffs.size(), 1);
    QCOMPARE(diffs[0].operation, EQUAL);
    QCOMPARE(diffs[0].text, text);
}

void TestDiff::testComplexChanges()
{
    QString text1 = "The quick brown fox jumps over the lazy dog";
    QString text2 = "The fast red fox leaps over the sleepy cat";
    
    QList<Diff> diffs = dmp.diff_main(text1, text2);
    
    QVERIFY(!diffs.isEmpty());
    QVERIFY(diffs.size() > 3);  // Should have multiple operations
}

void TestDiff::testUnicodeSupport()
{
    QString text1 = "Hello 世界";
    QString text2 = "Hello 🌍 世界";
    
    QList<Diff> diffs = dmp.diff_main(text1, text2);
    
    QVERIFY(!diffs.isEmpty());
    
    // Should handle Unicode correctly
    bool foundInsert = false;
    for (const Diff& diff : diffs) {
        if (diff.operation == INSERT && diff.text.contains("🌍")) {
            foundInsert = true;
            break;
        }
    }
    QVERIFY(foundInsert);
}

void TestDiff::testLargeTexts()
{
    QString largeText1 = QString("Lorem ipsum dolor sit amet. ").repeated(1000);
    QString largeText2 = largeText1;
    largeText2.replace("dolor", "MODIFIED");
    
    QList<Diff> diffs = dmp.diff_main(largeText1, largeText2);
    
    QVERIFY(!diffs.isEmpty());
    
    // Should find the modification
    bool foundModification = false;
    for (const Diff& diff : diffs) {
        if (diff.text.contains("MODIFIED")) {
            foundModification = true;
            break;
        }
    }
    QVERIFY(foundModification);
}

void TestDiff::testLineBasedDiff()
{
    QString text1 = "Line 1\nLine 2\nLine 3\nLine 4";
    QString text2 = "Line 1\nModified Line 2\nLine 3\nNew Line\nLine 4";
    
    QList<Diff> diffs = dmp.diff_main(text1, text2);
    
    QVERIFY(!diffs.isEmpty());
    
    // Should handle line-based changes
    QString result = dmp.diff_prettyHtml(diffs);
    QVERIFY(!result.isEmpty());
}

QTEST_MAIN(TestDiff)
#include "test_diff.moc"
