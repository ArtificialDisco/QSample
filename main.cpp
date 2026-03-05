#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "QtExample1_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    QFile qss(":/stylesheet/Darkeum.qss");
    bool what = qss.open(QFile::ReadOnly);
    a.setStyleSheet(qss.readAll());
    qss.close();

    MainWindow w;
    w.show();
    return a.exec();
}
