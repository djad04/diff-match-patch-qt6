#include <QtTest/QtTest>
#include "../../diff_match_patch.h"

class TestEdgeCases : public QObject
{
    Q_OBJECT

private:
    diff_match_patch dmp;

private slots:
    void initTestCase();
    
    void testEmptyStrings();
    void testEmptyToNonEmpty();
    void testNonEmptyToEmpty();
    void testVeryLongStrings();
    void testSingleCharacter();
    void testWhitespaceOnly();
    void testSpecialCharacters();
    void testNewlineHandling();
};

void TestEdgeCases::initTestCase()
{
    dmp.Diff_Timeout = 0.0f;
}

void TestEdgeCases::testEmptyStrings()
{
    QString empty1 = "";
    QString empty2 = "";
    
    QList<Diff> diffs = dmp.diff_main(empty1, empty2);
    
    QVERIFY(diffs.isEmpty() || (diffs.size() == 1 && diffs[0].operation == EQUAL && diffs[0].text.isEmpty()));
}

void TestEdgeCases::testEmptyToNonEmpty()
{
    QString empty = "";
    QString nonEmpty = "Something";
    
    QList<Diff> diffs = dmp.diff_main(empty, nonEmpty);
    
    QCOMPARE(diffs.size(), 1);
    QCOMPARE(diffs[0].operation, INSERT);
    QCOMPARE(diffs[0].text, nonEmpty);
}

void TestEdgeCases::testNonEmptyToEmpty()
{
    QString nonEmpty = "Something";
    QString empty = "";
    
    QList<Diff> diffs = dmp.diff_main(nonEmpty, empty);
    
    QCOMPARE(diffs.size(), 1);
    QCOMPARE(diffs[0].operation, DELETE);
    QCOMPARE(diffs[0].text, nonEmpty);
}

void TestEdgeCases::testVeryLongStrings()
{
    QString longText1 = QString("A").repeated(10000);
    QString longText2 = QString("A").repeated(9999) + "B";
    
    QList<Diff> diffs = dmp.diff_main(longText1, longText2);
    
    QVERIFY(!diffs.isEmpty());
    
    // Should detect the change at the end
    bool foundChange = false;
    for (const Diff& diff : diffs) {
        if (diff.operation != EQUAL) {
            foundChange = true;
            break;
        }
    }
    QVERIFY(foundChange);
}

void TestEdgeCases::testSingleCharacter()
{
    QString char1 = "A";
    QString char2 = "B";
    
    QList<Diff> diffs = dmp.diff_main(char1, char2);
    
    QCOMPARE(diffs.size(), 2);
    QCOMPARE(diffs[0].operation, DELETE);
    QCOMPARE(diffs[0].text, QString("A"));
    QCOMPARE(diffs[1].operation, INSERT);
    QCOMPARE(diffs[1].text, QString("B"));
}

void TestEdgeCases::testWhitespaceOnly()
{
    QString spaces1 = "    ";
    QString spaces2 = "\t\t";
    
    QList<Diff> diffs = dmp.diff_main(spaces1, spaces2);
    
    QVERIFY(!diffs.isEmpty());
    
    // Should detect whitespace differences
    bool foundDifference = false;
    for (const Diff& diff : diffs) {
        if (diff.operation != EQUAL) {
            foundDifference = true;
            break;
        }
    }
    QVERIFY(foundDifference);
}

void TestEdgeCases::testSpecialCharacters()
{
    QString special1 = "!@#$%^&*()_+-=[]{}|;':\",./<>?";
    QString special2 = "!@#$%^&*()_+-=[]{}|;':\"MODIFIED,./<>?";
    
    QList<Diff> diffs = dmp.diff_main(special1, special2);
    
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

void TestEdgeCases::testNewlineHandling()
{
    QString text1 = "Line 1\nLine 2\nLine 3";
    QString text2 = "Line 1\r\nLine 2\r\nLine 3";
    
    QList<Diff> diffs = dmp.diff_main(text1, text2);
    
    // Should handle different newline formats
    QVERIFY(!diffs.isEmpty());
}

QTEST_MAIN(TestEdgeCases)
#include "test_edge_cases.moc"
