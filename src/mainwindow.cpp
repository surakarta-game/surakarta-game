#include "mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QThreadPool>
#include "./ui_mainwindow.h"
#include "surakarta_daemon_thread.h"
#include "surakarta_game_browser_window.h"
#include "surakarta_session_window.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), timer(new QTimer(this)) {
    ui->setupUi(this);
    ui->surakarta_board->LoadN(BOARD_SIZE);
    SurakartaMoveTrace trace;
    timer->setInterval(0);
    timer->start();
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::StartSession);
    connect(ui->openButton, &QPushButton::clicked, this, &MainWindow::OpenManual);
    connect(timer, &QTimer::timeout, this, &MainWindow::OnTimerTimeout);
}

MainWindow::~MainWindow() {
    delete ui;
    delete timer;
}

void MainWindow::OnTimerTimeout() {
    const auto N = ui->surakarta_board->GetN();
    auto black_pieces = std::make_unique<std::vector<SurakartaPositionWithId>>(1);
    auto white_pieces = std::make_unique<std::vector<SurakartaPositionWithId>>(1);
    black_pieces->at(0) = SurakartaPositionWithId(1, 0, 0);
    white_pieces->at(0) = SurakartaPositionWithId(N - 1, N - 3, 1);
    auto black_trace = SurakartaMoveTrace();
    auto white_trace = SurakartaMoveTrace();
    black_trace.moved_piece = black_pieces->at(0);
    black_trace.is_capture = false;
    black_trace.captured_piece.id = -1;
    black_trace.path.push_back(SurakartaMovePathFragment(0, 0, 1, 0, 1, true));  // (1, 0) -> (0, 1)
    black_trace.path.push_back(SurakartaMovePathFragment(0, 1, N - 1, 1));
    black_trace.path.push_back(SurakartaMovePathFragment(N - 1, 0, 1, 1, 2, true));  // (N-1, 1) -> (N-2, 0)
    black_trace.path.push_back(SurakartaMovePathFragment(N - 2, 0, N - 2, N - 1));
    black_trace.path.push_back(SurakartaMovePathFragment(N - 1, N - 1, 1, 2, 3, true));  // (N-2, N-1) -> (N-1, N-2)
    black_trace.path.push_back(SurakartaMovePathFragment(N - 1, N - 2, 0, N - 2));
    black_trace.path.push_back(SurakartaMovePathFragment(0, N - 1, 1, 3, 0, true));  // (0, N-2) -> (1, N-1)
    black_trace.path.push_back(SurakartaMovePathFragment(1, N - 1, 1, 0));
    white_trace.moved_piece = white_pieces->at(0);
    white_trace.is_capture = false;
    white_trace.captured_piece.id = -1;
    white_trace.path.push_back(SurakartaMovePathFragment(N - 1, N - 1, 2, 3, 2, false));  // (N-1, N-3) -> (N-3, N-1)
    white_trace.path.push_back(SurakartaMovePathFragment(N - 3, N - 1, N - 3, 0));
    white_trace.path.push_back(SurakartaMovePathFragment(N - 1, 0, 2, 2, 1, false));  // (N-3, 0) -> (N-1, 3)
    white_trace.path.push_back(SurakartaMovePathFragment(N - 1, 3, 0, 3));
    white_trace.path.push_back(SurakartaMovePathFragment(0, 0, 2, 1, 0, false));  // (0, 3) -> (3, 0)
    white_trace.path.push_back(SurakartaMovePathFragment(3, 0, 3, N - 1));
    white_trace.path.push_back(SurakartaMovePathFragment(0, N - 1, 2, 0, 3, false));  // (3, N-1) -> (0, N-3)
    white_trace.path.push_back(SurakartaMovePathFragment(0, N - 3, N - 1, N - 3));
    ui->surakarta_board->SetAnimationMilliseconds(circle_time);
    ui->surakarta_board->ReloadPieces(std::move(black_pieces), std::move(white_pieces));
    ui->surakarta_board->OnMoveCommitted(black_trace);
    ui->surakarta_board->OnMoveCommitted(white_trace);
    ui->surakarta_board->SelectPiece(GlobalRandomGenerator::getInstance()() % N, GlobalRandomGenerator::getInstance()() % N);
    ui->surakarta_board->SelectDestination(GlobalRandomGenerator::getInstance()() % N, GlobalRandomGenerator::getInstance()() % N);
    timer->setInterval(circle_time);
}

void MainWindow::StartSession() {
    const auto color_ristriction = ui->agent_setting->GetPieceColorRistriction();
    auto ai_agent_factory_opt = ui->agent_setting->CreateAgentFactory();
    if (!ai_agent_factory_opt.has_value()) {
        QMessageBox::warning(this, "Error", "Invalid agent setting");
        return;
    }
    const auto handler = std::make_shared<SurakartaAgentInteractiveHandler>();
    handler->BlockAgentCreation();
    const auto my_agent_factory = handler->GetAgentFactory();
    // Todo: add a setting to choose the color of the player
    PieceColor my_color = color_ristriction == PieceColor::NONE
                              ? (GlobalRandomGenerator::getInstance()() % 2 == 0 ? PieceColor::BLACK : PieceColor::WHITE)
                              : color_ristriction;
    auto daemon = my_color == PieceColor::BLACK
                      ? std::make_unique<SurakartaDaemonThread>(
                            std::make_unique<SurakartaDaemon>(BOARD_SIZE, MAX_NO_CAPTURE_ROUND, my_agent_factory, std::move(ai_agent_factory_opt.value())))
                      : std::make_unique<SurakartaDaemonThread>(
                            std::make_unique<SurakartaDaemon>(BOARD_SIZE, MAX_NO_CAPTURE_ROUND, std::move(ai_agent_factory_opt.value()), my_agent_factory));
    daemon->start();
    sessionWindow = std::make_unique<SurakartaSessionWindow>(handler, std::move(daemon), 15);
    sessionWindow->show();
    this->hide();
    this->timer->stop();
    connect(sessionWindow.get(), &SurakartaSessionWindow::closed, this, &MainWindow::OnSessionWindowClosed);
}

void MainWindow::OpenManual() {
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Manual"), "", tr("Text Files (*.txt)"));
    if (filename.isEmpty()) {
        return;
    }
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Cannot open file");
        return;
    }
    QTextStream in(&file);
    std::string content = in.readAll().toStdString();

    browserWindow = std::make_unique<SurakartaGameBrowserWindow>(content);
    browserWindow->show();
    this->hide();
    this->timer->stop();
    connect(browserWindow.get(), &SurakartaGameBrowserWindow::closed, this, &MainWindow::OnManualWindowClosed);
}

void MainWindow::OnSessionWindowClosed() {
    garbage.push_back(std::move(sessionWindow));  // move the window object to garbage
    this->show();
    this->timer->setInterval(0);
    this->timer->start();
}

void MainWindow::OnManualWindowClosed() {
    garbage2.push_back(std::move(browserWindow));  // move the window object to garbage
    this->show();
    this->timer->setInterval(0);
    this->timer->start();
}
