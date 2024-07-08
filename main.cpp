#include "includes/mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    MainWindow ww;
    ww.show();
    ww.show_w();
    return a.exec();
}
