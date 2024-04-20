#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::startSession);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::startSession() {
    sessionWindow = std::make_unique<SurakartaSessionWindow>();
    sessionWindow->show();
    this->hide();
    connect(sessionWindow.get(), &SurakartaSessionWindow::closed, this, &MainWindow::show);
}
