#include <QtTest/QtTest>
#include "../../diff_match_patch.h"

class TestMatch : public QObject
{
    Q_OBJECT

private:
    diff_match_patch dmp;

private slots:
    void initTestCase();
    
    void testExactMatch();
    void testFuzzyMatch();
    void testNoMatch();
    void testMatchAtBeginning();
    void testMatchAtEnd();
    void testSpecialCharacters();
    void testUnicodeMatch();
};

void TestMatch::initTestCase()
{
    dmp.Diff_Timeout = 0.0f;
}

void TestMatch::testExactMatch()
{
    QString text = "Hello World, this is a test string for matching";
    QString pattern = "World";
    
    int location = dmp.match_main(text, pattern, 6);
    
    QCOMPARE(location, 6);
}

void TestMatch::testFuzzyMatch()
{
    QString text = "Hello World, this is a test string for matching";
    QString pattern = "World";
    
    // Search near the correct location
    int location = dmp.match_main(text, pattern, 10);
    
    QVERIFY(location >= 0);
    QVERIFY(location <= 10);  // Should find it nearby
}

void TestMatch::testNoMatch()
{
    QString text = "Hello World, this is a test string";
    QString pattern = "NonExistent";
    
    int location = dmp.match_main(text, pattern, 0);
    
    QCOMPARE(location, -1);
}

void TestMatch::testMatchAtBeginning()
{
    QString text = "Hello World, this is a test";
    QString pattern = "Hello";
    
    int location = dmp.match_main(text, pattern, 0);
    
    QCOMPARE(location, 0);
}

void TestMatch::testMatchAtEnd()
{
    QString text = "This is a test string";
    QString pattern = "string";
    
    int location = dmp.match_main(text, pattern, text.length() - pattern.length());
    
    QCOMPARE(location, text.length() - pattern.length());
}

void TestMatch::testSpecialCharacters()
{
    QString text = "Special chars: @#$%^&*()";
    QString pattern = "@#$%";
    
    int location = dmp.match_main(text, pattern, 15);
    
    QVERIFY(location >= 0);
}

void TestMatch::testUnicodeMatch()
{
    QString text = "Unicode test: 世界 🌍 مرحبا";
    QString pattern = "世界";
    
    int location = dmp.match_main(text, pattern, 14);
    
    QVERIFY(location >= 0);
    QVERIFY(text.mid(location, pattern.length()) == pattern);
}

QTEST_MAIN(TestMatch)
#include "test_match.moc"
