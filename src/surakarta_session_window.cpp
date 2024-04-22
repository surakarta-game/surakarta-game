#include "surakarta_session_window.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <QThreadPool>
#include <thread>
#include "ui_surakarta_session_window.h"

SurakartaSessionWindow::SurakartaSessionWindow(
    std::shared_ptr<SurakartaAgentInteractiveHandler> handler,
    std::unique_ptr<SurakartaDaemonThread> daemon_thread,
    QWidget* parent)
    : QWidget(parent),
      ui(new Ui::SurakartaSessionWindow),
      handler_(handler),
      daemon_thread_(std::move(daemon_thread)) {
    // set up UI
    ui->setupUi(this);
    ui->surakarta_board->LoadN(BOARD_SIZE);

    // set up event handlers

    // ui events
    connect(ui->surakarta_board, &SurakartaBoardWidget::onBoardClicked, this, &SurakartaSessionWindow::OnBoardClicked);
    connect(ui->commitButton, &QPushButton::clicked, this, &SurakartaSessionWindow::OnCommitButtonClicked);
    connect(ui->ai_suggest_button, &QPushButton::clicked, this, &SurakartaSessionWindow::OnAiSuggestionButtonClicked);
    ui->surakarta_board->UsePieceUpdater(
        [&]() { return handler_->CopyMyPieces(); },
        [&]() { return handler_->CopyOpponentPieces(); });

    // emulation events
    connect(this, &SurakartaSessionWindow::emulateBoardClicked, this, &SurakartaSessionWindow::OnBoardClicked);
    connect(this, &SurakartaSessionWindow::emulateCommitButtonClicked, this, &SurakartaSessionWindow::OnCommitButtonClicked);

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
    daemon_thread_->wait();
    delete ui;
}

void SurakartaSessionWindow::closeEvent(QCloseEvent* event) {
    daemon_thread_->terminate();
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

bool SurakartaSessionWindow::SelectPiece(SurakartaPosition pos) {
    if (handler_->CanSelectPiece(pos)) {
        const auto ret = handler_->SelectPiece(pos);
        assert(ret);
        UpdateInfo();
        return true;
    }
    return false;
}

bool SurakartaSessionWindow::SelectDestination(SurakartaPosition pos) {
    if (handler_->CanSelectDestination(pos)) {
        const auto ret = handler_->SelectDestination(pos);
        assert(ret);
        UpdateInfo();
        return true;
    }
    return false;
}

void SurakartaSessionWindow::OnBoardClicked(int x, int y) {
    const auto ret1 = SelectPiece(SurakartaPosition(x, y));
    const auto ret2 = SelectDestination(SurakartaPosition(x, y));
    assert(!(ret1 && ret2));
}

void SurakartaSessionWindow::OnCommitButtonClicked() {
    const auto ret = handler_->CommitMove();
    assert(ret);
}

void SurakartaSessionWindow::OnAiSuggestionButtonClicked() {
    auto factory = ui->ai_helper_setting->CreateAgentFactory();
    if (!factory.has_value()) {
        return;
    }
    std::shared_ptr<SurakartaAgentBase> temp_agent =
        daemon_thread_->CreateAgentFactoryApplicator()->CreateAgentFromFactory(*factory.value(), handler_->MyColor());

    const bool auto_commit = ui->ai_auto_commit->isChecked();

    // Asynchronously calculate the move
    QThreadPool::globalInstance()->start([=]() {
        const auto move = temp_agent->CalculateMove();        // This may take a long time
        this->emulateBoardClicked(move.from.x, move.from.y);  // Use signal to avoid cross-thread operation
        this->emulateBoardClicked(move.to.x, move.to.y);
        if (auto_commit) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));  // Let the selection square be shown
            this->emulateCommitButtonClicked();
        }
    });
}

void SurakartaSessionWindow::OnAgentCreated() {
    ui->surakarta_board->ReloadPieces(handler_->CopyMyPieces(), handler_->CopyOpponentPieces());
    UpdateInfo();
}

void SurakartaSessionWindow::OnWaitingForMove() {
    UpdateInfo();
    if (ui->ai_auto_suggest->isChecked()) {
        OnAiSuggestionButtonClicked();
    }
}

void SurakartaSessionWindow::OnMoveCommitted(SurakartaMoveTrace trace) {
    ui->surakarta_board->OnMoveCommitted(trace);
    UpdateInfo();
}
