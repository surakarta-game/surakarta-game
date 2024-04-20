#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "surakarta_daemon_thread.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::StartSession);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::StartSession() {
    const auto ai_agent_factory = std::make_shared<SurakartaAgentMineFactory>(
        std::make_shared<SurakartaAgentMineFactory::SurakartaMoveWeightUtilFactory>());
    const auto handler = std::make_shared<SurakartaAgentInteractiveHandler>();
    const auto my_agent_factory = handler->GetAgentFactory();
    QThread* daemon = new SurakartaDaemonThread(
        std::make_unique<SurakartaDaemon>(BOARD_SIZE, MAX_NO_CAPTURE_ROUND, my_agent_factory, ai_agent_factory));
    daemon->start();
    sessionWindow = std::make_unique<SurakartaSessionWindow>(handler);
    sessionWindow->show();
    this->hide();
    connect(sessionWindow.get(), &SurakartaSessionWindow::closed, this, &MainWindow::show);
}
