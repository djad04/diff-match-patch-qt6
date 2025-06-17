#include "../diff_match_patch.h"
#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QString>
#include <QList>
#include <QVector>
#include <QPair>

class DiffTestSuite {
private:
    diff_match_patch dmp;
    int testsPassed = 0;
    int testsTotal = 0;
    
    void assert_test(bool condition, const QString& testName) {
        testsTotal++;
        if (condition) {
            testsPassed++;
            qDebug() << " PASS:" << testName;
        } else {
            qDebug() << " FAIL:" << testName;
        }
    }

public:
    void runAllTests() {
        qDebug() << "\n=== diff-match-patch-qt6 Test Suite ===\n";
        
        testBasicDiffs();
        testComplexDiffs();
        testUnicodeHandling();
        testPatchOperations();
        testMatchOperations();
        testEdgeCases();
        testRealWorldScenarios();
        
        printSummary();
    }
    
    void testBasicDiffs() {
        qDebug() << "📝 Testing Basic Diff Operations...";
        
        // Test 1: Simple insertion
        QString text1 = "Hello World";
        QString text2 = "Hello Beautiful World";
        QList<Diff> diffs = dmp.diff_main(text1, text2);
        assert_test(diffs.size() == 3, "Simple insertion should have 3 parts");
        assert_test(diffs[1].operation == INSERT, "Middle part should be INSERT");
        assert_test(diffs[1].text == "Beautiful ", "Inserted text should be 'Beautiful '");
        
        // Test 2: Simple deletion
        text1 = "Hello Beautiful World";
        text2 = "Hello World";
        diffs = dmp.diff_main(text1, text2);
        assert_test(diffs.size() == 3, "Simple deletion should have 3 parts");
        assert_test(diffs[1].operation == DELETE, "Middle part should be DELETE");
        
        // Test 3: Simple replacement
        text1 = "Hello World";
        text2 = "Hello Universe";
        diffs = dmp.diff_main(text1, text2);
        assert_test(diffs.size() >= 2, "Replacement should have at least 2 parts");
        
        // Test 4: No changes
        text1 = "Identical Text";
        text2 = "Identical Text";
        diffs = dmp.diff_main(text1, text2);
        assert_test(diffs.size() == 1 && diffs[0].operation == EQUAL, "Identical texts should have one EQUAL diff");
    }
    
    void testComplexDiffs() {
        qDebug() << "\n🔧 Testing Complex Diff Operations...";
        
        // Test 1: Multiple changes
        QString text1 = "The quick brown fox jumps over the lazy dog";
        QString text2 = "The fast red fox leaps over the sleepy cat";
        QList<Diff> diffs = dmp.diff_main(text1, text2);
        assert_test(diffs.size() > 3, "Multiple changes should produce multiple diff parts");
        
        // Test 2: Line-based diff
        text1 = "Line 1\nLine 2\nLine 3\nLine 4";
        text2 = "Line 1\nModified Line 2\nLine 3\nNew Line\nLine 4";
        diffs = dmp.diff_main(text1, text2);
        assert_test(!diffs.isEmpty(), "Line-based diff should work");
        
        // Test 3: Large text with small changes
        QString largeText1 = QString("Lorem ipsum dolor sit amet, consectetur adipiscing elit. ").repeated(100);
        QString largeText2 = largeText1;
        largeText2.replace("consectetur", "MODIFIED");
        diffs = dmp.diff_main(largeText1, largeText2);
        assert_test(!diffs.isEmpty(), "Large text diff should not be empty");
        
        // Test 4: Whitespace handling
        text1 = "Hello    World";
        text2 = "Hello\tWorld";
        diffs = dmp.diff_main(text1, text2);
        assert_test(!diffs.isEmpty(), "Whitespace diff should work");
    }
    
    void testUnicodeHandling() {
        qDebug() << "\n🌍 Testing Unicode and Special Characters...";
        
        // Test 1: Unicode characters
        QString text1 = "Hello 世界";
        QString text2 = "Hello 🌍 世界";
        QList<Diff> diffs = dmp.diff_main(text1, text2);
        assert_test(!diffs.isEmpty(), "Unicode diff should work");
        
        // Test 2: Emojis
        text1 = "Happy 😊 face";
        text2 = "Happy 😢 face";
        diffs = dmp.diff_main(text1, text2);
        assert_test(!diffs.isEmpty(), "Emoji diff should work");
        
        // Test 3: Mixed scripts
        text1 = "English العربية 中文 русский";
        text2 = "English العربية CHANGED 中文 русский";
        diffs = dmp.diff_main(text1, text2);
        assert_test(!diffs.isEmpty(), "Mixed scripts diff should work");
        
        // Test 4: Special characters
        text1 = "Special: @#$%^&*()_+-=[]{}|;':\",./<>?";
        text2 = "Special: @#$%^&*()_+-=[]{}|;':\"MODIFIED,./<>?";
        diffs = dmp.diff_main(text1, text2);
        assert_test(!diffs.isEmpty(), "Special characters diff should work");
    }
    
    void testPatchOperations() {
        qDebug() << "\n🔧 Testing Patch Operations...";
        
        // Test 1: Basic patch creation and application
        QString text1 = "The quick brown fox";
        QString text2 = "The fast red fox";
        QList<Patch> patches = dmp.patch_make(text1, text2);
        
        assert_test(!patches.isEmpty(), "Patch creation should produce patches");
        
        // Test patch toString
        QString patchText = dmp.patch_toText(patches);
        assert_test(!patchText.isEmpty(), "Patch text should not be empty");
        
        // Test patch application
        QPair<QString, QVector<bool>> result = dmp.patch_apply(patches, text1);
        QString patchedText = result.first;
        QVector<bool> results = result.second;
        
        assert_test(patchedText == text2, "Patch application should produce target text");
        assert_test(results.size() == patches.size(), "Results should match patch count");
        
        // Test 2: Patch from text
        QList<Patch> parsedPatches = dmp.patch_fromText(patchText);
        assert_test(parsedPatches.size() == patches.size(), "Parsed patches should match original count");
        
        // Test 3: Complex patch
        text1 = "Line 1\nLine 2\nLine 3\nLine 4\nLine 5";
        text2 = "Line 1\nModified Line 2\nLine 3\nNew Line\nLine 4\nLine 5";
        patches = dmp.patch_make(text1, text2);
        result = dmp.patch_apply(patches, text1);
        assert_test(result.first == text2, "Complex patch should work correctly");
    }
    
    void testMatchOperations() {
        qDebug() << "\n🎯 Testing Match Operations...";
        
        // Test 1: Exact match
        QString text = "Hello World, this is a test string for matching";
        QString pattern = "World";
        int location = dmp.match_main(text, pattern, 6);
        assert_test(location == 6, "Exact match should return correct position");
        
        // Test 2: Fuzzy match
        location = dmp.match_main(text, pattern, 10);
        assert_test(location >= 0, "Fuzzy match should find approximate position");
        
        // Test 3: No match
        location = dmp.match_main(text, "NonExistent", 0);
        assert_test(location == -1, "Non-existent pattern should return -1");
        
        // Test 4: Match at beginning
        location = dmp.match_main(text, "Hello", 0);
        assert_test(location == 0, "Match at beginning should return 0");
        
        // Test 5: Match with special characters
        text = "Special chars: @#$%^&*()";
        pattern = "@#$%";
        location = dmp.match_main(text, pattern, 15);
        assert_test(location >= 0, "Special character match should work");
    }
    
    void testEdgeCases() {
        qDebug() << "\n🔍 Testing Edge Cases...";
        
        // Test 1: Empty strings
        QString text1 = "";
        QString text2 = "Something";
        QList<Diff> diffs = dmp.diff_main(text1, text2);
        assert_test(diffs.size() == 1 && diffs[0].operation == INSERT, "Empty to non-empty should be INSERT");
        
        // Test 2: Non-empty to empty
        text1 = "Something";
        text2 = "";
        diffs = dmp.diff_main(text1, text2);
        assert_test(diffs.size() == 1 && diffs[0].operation == DELETE, "Non-empty to empty should be DELETE");
        
        // Test 3: Very long strings
        text1 = QString("A").repeated(10000);
        text2 = QString("A").repeated(9999) + "B";
        diffs = dmp.diff_main(text1, text2);
        assert_test(!diffs.isEmpty(), "Very long string diff should work");
        
        // Test 4: Single character
        text1 = "A";
        text2 = "B";
        diffs = dmp.diff_main(text1, text2);
        assert_test(diffs.size() == 2, "Single character change should have 2 parts");
        
        // Test 5: Null handling (should throw exception as designed)
        try {
            text1 = QString();
            text2 = QString();
            diffs = dmp.diff_main(text1, text2);
            assert_test(false, "Null string handling should throw exception");
        } catch (...) {
            assert_test(true, "Null string handling correctly throws exception");
        }
    }
    
    void testRealWorldScenarios() {
        qDebug() << "\n🌐 Testing Real-World Scenarios...";
        
        // Test 1: Code diff simulation
        QString code1 = R"(
function hello() {
    console.log("Hello World");
    return true;
}
)";
        QString code2 = R"(
function hello(name) {
    console.log("Hello " + name);
    return name !== undefined;
}
)";
        QList<Diff> diffs = dmp.diff_main(code1, code2);
        assert_test(!diffs.isEmpty(), "Code diff should work");
        
        // Test 2: Configuration file changes
        QString config1 = "server.port=8080\nserver.host=localhost\ndebug=false";
        QString config2 = "server.port=9090\nserver.host=0.0.0.0\ndebug=true\nlogging=enabled";
        diffs = dmp.diff_main(config1, config2);
        assert_test(!diffs.isEmpty(), "Config diff should work");
        
        // Test 3: Document editing
        QString doc1 = "This is the first paragraph.\n\nThis is the second paragraph with some content.";
        QString doc2 = "This is the modified first paragraph.\n\nThis is the second paragraph with updated content.\n\nThis is a new third paragraph.";
        diffs = dmp.diff_main(doc1, doc2);
        assert_test(!diffs.isEmpty(), "Document diff should work");
        
        // Test patch round-trip for real scenario
        QList<Patch> patches = dmp.patch_make(doc1, doc2);
        QPair<QString, QVector<bool>> result = dmp.patch_apply(patches, doc1);
        assert_test(result.first == doc2, "Real-world patch round-trip should work");
    }
    
    void printSummary() {
        qDebug() << "\n📊 Test Summary";
        qDebug() << "===============";
        qDebug() << "Tests Passed:" << testsPassed << "/" << testsTotal;
        qDebug() << "Success Rate:" << (testsPassed * 100.0 / testsTotal) << "%";
        
        if (testsPassed == testsTotal) {
            qDebug() << "🎉 All tests passed! The diff algorithm is working perfectly.";
        } else {
            qDebug() << "⚠️  Some tests failed. Please review the implementation.";
        }
        qDebug() << "\n";
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    qDebug() << "🧪 diff-match-patch-qt6 Test Suite";
    qDebug() << "===================================";
    
    DiffTestSuite testSuite;
    testSuite.runAllTests();
    
    return 0;
}
