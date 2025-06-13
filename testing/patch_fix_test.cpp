#include "../diff_match_patch.h"
#include <QCoreApplication>
#include <QDebug>
#include <QString>
#include <QList>
#include <QPair>
#include <QVector>

class PatchFixTest {
private:
    diff_match_patch dmp;
    
    QString generateTestText(int size) {
        QString chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 \n";
        QString result;
        result.reserve(size);
        
        for (int i = 0; i < size; ++i) {
            result += chars[i % chars.length()];
        }
        return result;
    }
    
    QString modifyText(const QString& original, double changePercent) {
        QString modified = original;
        int changeCount = static_cast<int>(original.length() * changePercent);
        
        // Make predictable changes for testing
        for (int i = 0; i < changeCount && i < modified.length(); i += 10) {
            if (i < modified.length()) {
                modified[i] = 'X';
            }
        }
        return modified;
    }
    
public:
    void testPatchWithDifferentMaxBits() {
        qDebug() << "=== Testing Patch Application with Different Match_MaxBits ===\n";
        
        QString text1 = generateTestText(1000);
        QString text2 = modifyText(text1, 0.1); // 10% changes
        
        // Test with original small MaxBits (32)
        qDebug() << "Testing with Match_MaxBits = 32:";
        dmp.Match_MaxBits = 32;
        
        QList<Patch> patches = dmp.patch_make(text1, text2);
        QPair<QString, QVector<bool>> result = dmp.patch_apply(patches, text1);
        
        bool success32 = (result.first == text2);
        int successCount32 = 0;
        for (bool patchResult : result.second) {
            if (patchResult) successCount32++;
        }
        
        qDebug() << QString("  Patches created: %1").arg(patches.size());
        qDebug() << QString("  Patches applied successfully: %1/%2").arg(successCount32).arg(patches.size());
        qDebug() << QString("  Overall success: %1").arg(success32 ? "YES" : "NO");
        
        // Test with larger MaxBits (1024)
        qDebug() << "\nTesting with Match_MaxBits = 1024:";
        dmp.Match_MaxBits = 1024;
        
        patches = dmp.patch_make(text1, text2);
        result = dmp.patch_apply(patches, text1);
        
        bool success1024 = (result.first == text2);
        int successCount1024 = 0;
        for (bool patchResult : result.second) {
            if (patchResult) successCount1024++;
        }
        
        qDebug() << QString("  Patches created: %1").arg(patches.size());
        qDebug() << QString("  Patches applied successfully: %1/%2").arg(successCount1024).arg(patches.size());
        qDebug() << QString("  Overall success: %1").arg(success1024 ? "YES" : "NO");
        
        // Summary
        qDebug() << "\n=== Summary ===";
        qDebug() << QString("Match_MaxBits = 32:   %1").arg(success32 ? "PASS" : "FAIL");
        qDebug() << QString("Match_MaxBits = 1024: %1").arg(success1024 ? "PASS" : "FAIL");
        
        if (success1024 && !success32) {
            qDebug() << "✅ Fix confirmed: Increasing Match_MaxBits resolves patch application issues!";
        } else if (success32 && success1024) {
            qDebug() << "ℹ️  Both configurations work for this test case.";
        } else {
            qDebug() << "❌ Issue persists even with increased Match_MaxBits.";
        }
    }
    
    void testLargeFileDiffStructure() {
        qDebug() << "\n=== Testing Large File Diff Structure ===\n";
        
        // Create a large text with small changes
        QString largeText1 = generateTestText(100000); // 100KB
        QString largeText2 = modifyText(largeText1, 0.05); // 5% changes
        
        QList<Diff> diffs = dmp.diff_main(largeText1, largeText2);
        
        qDebug() << QString("Large file diff analysis (100KB with 5%% changes):");
        qDebug() << QString("Total operations: %1").arg(diffs.size());
        
        int equalOps = 0, insertOps = 0, deleteOps = 0;
        int totalEqualChars = 0, totalInsertChars = 0, totalDeleteChars = 0;
        int largestEqualBlock = 0;
        
        for (const Diff& diff : diffs) {
            switch (diff.operation) {
                case EQUAL:
                    equalOps++;
                    totalEqualChars += diff.text.length();
                    if (diff.text.length() > largestEqualBlock) {
                        largestEqualBlock = static_cast<int>(diff.text.length());
                    }
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
        
        qDebug() << QString("  EQUAL operations: %1 (total chars: %2)").arg(equalOps).arg(totalEqualChars);
        qDebug() << QString("  INSERT operations: %1 (total chars: %2)").arg(insertOps).arg(totalInsertChars);
        qDebug() << QString("  DELETE operations: %1 (total chars: %2)").arg(deleteOps).arg(totalDeleteChars);
        qDebug() << QString("  Largest EQUAL block: %1 characters").arg(largestEqualBlock);
        
        double efficiency = (double)totalEqualChars / (largeText1.length() + largeText2.length()) * 100;
        qDebug() << QString("  Algorithm efficiency: %.1f%% (higher is better)").arg(efficiency);
        
        if (diffs.size() <= 10 && largestEqualBlock > 50000) {
            qDebug() << "✅ This is normal behavior: Half-match optimization found large common sections.";
        } else {
            qDebug() << "ℹ️  Standard diff algorithm used (no large common sections found).";
        }
    }
    
    void runAllTests() {
        testPatchWithDifferentMaxBits();
        testLargeFileDiffStructure();
    }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    qDebug() << "Patch Fix Test - Analyzing diff-match-patch-qt6 Issues\n";
    
    PatchFixTest test;
    test.runAllTests();
    
    qDebug() << "\n=== Test Complete ===";
    
    return 0;
}
