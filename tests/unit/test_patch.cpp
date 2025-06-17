#include <QtTest/QtTest>
#include "../../diff_match_patch.h"

class TestPatch : public QObject
{
    Q_OBJECT

private:
    diff_match_patch dmp;

private slots:
    void initTestCase();
    
    void testPatchCreation();
    void testPatchApplication();
    void testPatchRoundTrip();
    void testPatchSerialization();
    void testPatchWithLargeChanges();
    void testPatchFailure();
};

void TestPatch::initTestCase()
{
    dmp.Diff_Timeout = 0.0f;
}

void TestPatch::testPatchCreation()
{
    QString text1 = "The quick brown fox";
    QString text2 = "The fast red fox";
    
    QList<Patch> patches = dmp.patch_make(text1, text2);
    
    QVERIFY(!patches.isEmpty());
    QVERIFY(patches.size() >= 1);
}

void TestPatch::testPatchApplication()
{
    QString text1 = "The quick brown fox";
    QString text2 = "The fast red fox";
    
    QList<Patch> patches = dmp.patch_make(text1, text2);
    QPair<QString, QVector<bool>> result = dmp.patch_apply(patches, text1);
    
    QCOMPARE(result.first, text2);
    QVERIFY(result.second.size() == patches.size());
    
    // All patches should apply successfully
    for (bool success : result.second) {
        QVERIFY(success);
    }
}

void TestPatch::testPatchRoundTrip()
{
    QString original = "Line 1\nLine 2\nLine 3\nLine 4\nLine 5";
    QString modified = "Line 1\nModified Line 2\nLine 3\nNew Line\nLine 4\nLine 5";
    
    // Create patches
    QList<Patch> patches = dmp.patch_make(original, modified);
    
    // Apply patches
    QPair<QString, QVector<bool>> result = dmp.patch_apply(patches, original);
    
    QCOMPARE(result.first, modified);
}

void TestPatch::testPatchSerialization()
{
    QString text1 = "Hello World";
    QString text2 = "Hello Universe";
    
    QList<Patch> patches = dmp.patch_make(text1, text2);
    QString patchText = dmp.patch_toText(patches);
    
    QVERIFY(!patchText.isEmpty());
    
    // Parse back from text
    QList<Patch> parsedPatches = dmp.patch_fromText(patchText);
    
    QCOMPARE(parsedPatches.size(), patches.size());
    
    // Apply parsed patches
    QPair<QString, QVector<bool>> result = dmp.patch_apply(parsedPatches, text1);
    QCOMPARE(result.first, text2);
}

void TestPatch::testPatchWithLargeChanges()
{
    QString text1 = QString("Original content. ").repeated(100);
    QString text2 = QString("Modified content. ").repeated(100);
    
    QList<Patch> patches = dmp.patch_make(text1, text2);
    QPair<QString, QVector<bool>> result = dmp.patch_apply(patches, text1);
    
    QCOMPARE(result.first, text2);
}

void TestPatch::testPatchFailure()
{
    QString text1 = "Hello World";
    QString text2 = "Hello Universe";
    QString differentText = "Completely different text";
    
    QList<Patch> patches = dmp.patch_make(text1, text2);
    
    // Try to apply patches to different text
    QPair<QString, QVector<bool>> result = dmp.patch_apply(patches, differentText);
    
    // Some patches might fail
    bool anyFailed = false;
    for (bool success : result.second) {
        if (!success) {
            anyFailed = true;
            break;
        }
    }
    
    // This is expected behavior - patches should fail on incompatible text
    QVERIFY(anyFailed || result.first != text2);
}

QTEST_MAIN(TestPatch)
#include "test_patch.moc"
