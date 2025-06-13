#include "diff_match_patch.h"

#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>

// Demo of diff-match-patch-qt6 capabilities
void runDemo() {
    qDebug() << "diff-match-patch-qt6 Demo";
    qDebug() << "========================\n";
    
    diff_match_patch dmp;
    
    // Demo 1: Basic text comparison
    qDebug() << "📝 Basic Text Comparison:";
    QString text1 = "The quick brown fox jumps over the lazy dog";
    QString text2 = "The fast red fox leaps over the sleepy cat";
    
    QList<Diff> diffs = dmp.diff_main(text1, text2);
    qDebug() << "Original:" << text1;
    qDebug() << "Modified:" << text2;
    qDebug() << "Changes detected:" << diffs.size() << "operations\n";
    
    // Demo 2: Unicode support
    qDebug() << "🌍 Unicode Support:";
    QString unicode1 = "Hello 世界 🌍";
    QString unicode2 = "Hello 世界 🚀 Universe";
    diffs = dmp.diff_main(unicode1, unicode2);
    qDebug() << "Unicode diff successful:" << (diffs.size() > 0 ? "✅" : "❌");
    
    // Demo 3: Performance test
    qDebug() << "\n⚡ Performance Test:";
    QString largeText1 = QString("Performance test line %1\n").repeated(1000);
    QString largeText2 = largeText1;
    largeText2.replace("line 500", "MODIFIED line 500");
    
    QElapsedTimer timer;
    timer.start();
    diffs = dmp.diff_main(largeText1, largeText2);
    qint64 elapsed = timer.elapsed();
    
    qDebug() << "Processed 1000+ lines in:" << elapsed << "ms ⚡";
    
    // Demo 4: Patch operations
    qDebug() << "\n🔧 Patch Operations:";
    QString original = "function hello() { return 'world'; }";
    QString modified = "function hello(name) { return 'Hello ' + name; }";
    
    QList<Patch> patches = dmp.patch_make(original, modified);
    QString patchText = dmp.patch_toText(patches);
    
    QPair<QString, QVector<bool>> result = dmp.patch_apply(patches, original);
    bool patchSuccess = (result.first == modified);
    
    qDebug() << "Patch generation and application:" << (patchSuccess ? "✅" : "❌");
    qDebug() << "Patch size:" << patchText.length() << "bytes";
    
    qDebug() << "\n🎉 QDiffX Demo Complete! All systems operational.\n";
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Run quick demo
    runQuickDemo();
    
    // Ask user if they want to see the GUI
    QTextStream stream(stdin);
    std::cout << "Would you like to see the QDiffWidget GUI? (y/n): ";
    std::cout.flush();
    
    QString response;
    response = stream.readLine().toLower();
    
    if (response == "y" || response == "yes") {
        // Setup translations
        QTranslator translator;
        const QStringList uiLanguages = QLocale::system().uiLanguages();
        for (const QString &locale : uiLanguages) {
            const QString baseName = "QDiffX_" + QLocale(locale).name();
            if (translator.load(":/i18n/" + baseName)) {
                a.installTranslator(&translator);
                break;
            }
        }
        
        // Create and show the diff widget
        QDiffWidget w;
        w.show();
        
        qDebug() << "🎨 Launching QDiffWidget GUI...";
        qDebug() << "💡 Try loading different files to see the diff visualization!";
        
        return a.exec();
    } else {
        qDebug() << "👋 Demo completed. Thank you for trying QDiffX!";
        qDebug() << "⭐ Star us on GitHub if you found this useful!";
        return 0;
    }
}
