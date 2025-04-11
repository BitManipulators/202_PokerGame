#include <QApplication>
#include "mainwindow.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyleSheet("QMessageBox { background-color: #1a5c35; }");
    MainWindow window;
    window.show();
    return app.exec();
}
