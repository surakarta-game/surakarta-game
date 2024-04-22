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
    // set up UI
    ui->setupUi(this);
    ui->surakarta_board->LoadN(BOARD_SIZE);

    // set up event handlers

    // ui events
    connect(ui->surakarta_board, &SurakartaBoardWidget::onBoardClicked, this, &SurakartaSessionWindow::OnBoardClicked);
    connect(ui->commitButton, &QPushButton::clicked, this, &SurakartaSessionWindow::OnCommitButtonClicked);
    ui->surakarta_board->UsePieceUpdater(
        [&]() { return handler_->CopyMyPieces(); },
        [&]() { return handler_->CopyOpponentPieces(); });

    // handler event chains
    // since event from the handler may come from another thread, we need to use signal-slot mechanism

    handler_->OnAgentCreated.AddListener([&]() {
        onAgentCreated();
    });
    connect(this, &SurakartaSessionWindow::onAgentCreated, this, &SurakartaSessionWindow::OnAgentCreated);
    
    handler_->OnWaitingForMove.AddListener([&]() {
        onWaitingForMove();
    });
    connect(this, &SurakartaSessionWindow::onWaitingForMove, this, &SurakartaSessionWindow::OnWaitingForMove);

    handler_->OnMoveCommitted.AddListener([&](SurakartaMoveTrace trace) {
        onMoveCommitted(trace);
    });
    connect(this, &SurakartaSessionWindow::onMoveCommitted, this, &SurakartaSessionWindow::OnMoveCommitted);

    // Allow agent creation
    handler_->UnblockAgentCreation();
}

SurakartaSessionWindow::~SurakartaSessionWindow() {
    delete ui;
}

void SurakartaSessionWindow::closeEvent(QCloseEvent* event) {
    closed();
    event->accept();
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
        const auto ret = handler_->SelectPiece(SurakartaPosition(x, y));
        assert(ret);
        UpdateInfo();
    } else if (handler_->CanSelectDestination(SurakartaPosition(x, y))) {
        const auto ret = handler_->SelectDestination(SurakartaPosition(x, y));
        assert(ret);
        UpdateInfo();
    }
}

void SurakartaSessionWindow::OnCommitButtonClicked() {
    const auto ret = handler_->CommitMove();
    assert(ret);
}

void SurakartaSessionWindow::OnAgentCreated() {
    ui->surakarta_board->ReloadPieces(handler_->CopyMyPieces(), handler_->CopyOpponentPieces());
    UpdateInfo();
}

void SurakartaSessionWindow::OnWaitingForMove() {
    UpdateInfo();
}

void SurakartaSessionWindow::OnMoveCommitted(SurakartaMoveTrace trace) {
    ui->surakarta_board->OnMoveCommitted(trace);
    UpdateInfo();
}
