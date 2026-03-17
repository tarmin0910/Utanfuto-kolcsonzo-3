#include <QApplication>
#include <QMessageBox>
#include <QApplication>
#include <QGuiApplication>

#include "MainWindow.h"
#include "DatabaseManager.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication a(argc, argv);

    if (!DatabaseManager::instance().initialize()) {
        QMessageBox::critical(nullptr, QStringLiteral("Adatbázis hiba"),
                              QStringLiteral("Nem sikerült megnyitni az adatbázist.\n%1")
                                  .arg(DatabaseManager::instance().lastError()));
        return 1;
    }

    MainWindow w;
    w.resize(1280, 860);
    w.show();
    return a.exec();
}
