#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QDir>
#include <QStandardPaths>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Set application information
    QCoreApplication::setOrganizationName("Pebicoin");
    QCoreApplication::setOrganizationDomain("pebicoin.org");
    QCoreApplication::setApplicationName("PebiWallet");
    
    // Create application data directory if it doesn't exist
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
        qDebug() << "Created application data directory:" << dataPath;
    }
    
    // Load translations if needed
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "PebiWallet_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    
    // Create and show the main window
    MainWindow w;
    w.show();
    
    return a.exec();
}