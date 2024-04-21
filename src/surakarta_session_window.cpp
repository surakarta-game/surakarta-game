#include "surakarta_session_window.h"
#include <QCloseEvent>
#include <thread>
#include "ui_surakarta_session_window.h"

SurakartaSessionWindow::SurakartaSessionWindow(
    std::shared_ptr<SurakartaAgentInteractiveHandler> handler,
    QWidget* parent)
    : QWidget(parent),
      ui(new Ui::SurakartaSessionWindow),
      handler_(handler) {
    ui->setupUi(this);
    ui->surakarta_board->LoadN(BOARD_SIZE);
    ui->surakarta_board->UsePieceUpdater(
        [&]() { return handler_->CopyMyPieces(); },
        [&]() { return handler_->CopyOpponentPieces(); });
    while (!handler_->IsAgentCreated())
        std::this_thread::sleep_for(std::chrono::milliseconds(0));
    handler_->OnMoveCommitted.AddListener([&](SurakartaMoveTrace trace) {
        std::lock_guard<std::mutex> lock(mutex_);
        move_queue_.push(trace);
        onMoveCommitted();
    });
    ui->surakarta_board->ReloadPieces(handler_->CopyMyPieces(), handler_->CopyOpponentPieces());
    connect(this, &SurakartaSessionWindow::onMoveCommitted, this, &SurakartaSessionWindow::OnMoveCommitted);
    connect(ui->surakarta_board, &SurakartaBoardWidget::onBoardClicked, this, &SurakartaSessionWindow::OnBoardClicked);
    connect(ui->commitButton, &QPushButton::clicked, this, &SurakartaSessionWindow::OnCommitButtonClicked);
    UpdateInfo();
}

SurakartaSessionWindow::~SurakartaSessionWindow() {
    delete ui;
}

void SurakartaSessionWindow::closeEvent(QCloseEvent* event) {
    closed();
    event->accept();
}

void SurakartaSessionWindow::OnMoveCommitted() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (move_queue_.empty())
        return;
    const auto trace = move_queue_.front();
    move_queue_.pop();
    ui->surakarta_board->OnMoveCommitted(trace);
    UpdateInfo();
}

void SurakartaSessionWindow::UpdateInfo() {
    const auto piece = handler_->SelectedPiece();
    if (piece.id != -1) {
        ui->surakarta_board->SelectPiece(piece.x, piece.y);
    } else {
        ui->surakarta_board->UnselectPiece();
    }
    const auto destination = handler_->SelectedDestination();
    if (destination.has_value()) {
        ui->surakarta_board->SelectDestination(destination->x, destination->y);
    } else {
        ui->surakarta_board->UnselectDestination();
    }
    if (handler_->IsMyTurn()) {
        ui->currentEdit->setText("You");
    } else {
        ui->currentEdit->setText("Opponent");
    }
    if (handler_->CanCommitMove()) {
        ui->commitButton->setEnabled(true);
    } else {
        ui->commitButton->setEnabled(false);
    }
}

void SurakartaSessionWindow::OnBoardClicked(int x, int y) {
    if (handler_->CanSelectPiece(SurakartaPosition(x, y))) {
        assert(handler_->SelectPiece(SurakartaPosition(x, y)));
        UpdateInfo();
    } else if (handler_->CanSelectDestination(SurakartaPosition(x, y))) {
        assert(handler_->SelectDestination(SurakartaPosition(x, y)));
        UpdateInfo();
    }
}

void SurakartaSessionWindow::OnCommitButtonClicked() {
    assert(handler_->CommitMove());
}
