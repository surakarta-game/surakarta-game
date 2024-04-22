#include "mainwindow.h"

#include <QApplication>
#include <QMetaClassInfo>
#include "surakarta.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    // Let SurakartaMoveTrace be able to be used in signal-slot mechanism
    qRegisterMetaType<SurakartaMoveTrace>("SurakartaMoveTrace");

    MainWindow w;
    w.show();
    return a.exec();
}
