
#include <QApplication>
#include "MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("Trailo");
    QApplication::setOrganizationName("Trailo");

    const QSize appSize(1100, 720);

    MainWindow w;
    w.resize(appSize);
    w.show();
    return app.exec();
}
