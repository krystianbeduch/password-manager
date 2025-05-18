#include "mainwindow.h"
// #include <sodium.h>
// #include <QDebug>
#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "password_manager_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    // if (sodium_init() < 0) {
    //     qFatal("libsodium initialization failed");
    //     return 1;
    // }


    MainWindow w;
    w.show();
    return a.exec();
}
