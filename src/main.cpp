#include "mainwindow.h"

#include <QApplication>
#include <QMetaClassInfo>
#include <exception>
#include "surakarta.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    // Let these classes can be used in signal-slot mechanism
    qRegisterMetaType<SurakartaMoveTrace>("SurakartaMoveTrace");
    qRegisterMetaType<SurakartaMoveResponse>("SurakartaMoveResponse");
    qRegisterMetaType<std::exception>("std::exception");

    MainWindow w;
    w.show();
    return a.exec();
}
