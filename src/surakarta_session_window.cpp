#include "surakarta_session_window.h"
#include <QCloseEvent>
#include "ui_surakarta_session_window.h"

SurakartaSessionWindow::SurakartaSessionWindow(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::SurakartaSessionWindow) {
    ui->setupUi(this);
    ui->surakarta_board->loadSize(800, 6);
}

SurakartaSessionWindow::~SurakartaSessionWindow() {
    delete ui;
}

void SurakartaSessionWindow::closeEvent(QCloseEvent* event) {
    closed();
    event->accept();
}
