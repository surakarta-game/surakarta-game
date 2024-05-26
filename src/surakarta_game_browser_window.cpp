#include "surakarta_game_browser_window.h"
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QThreadPool>
#include <QTimer>
#include <list>
#include <thread>
#include "ui_surakarta_game_browser_window.h"

SurakartaGameBrowserWindow::SurakartaGameBrowserWindow(
    std::string manual,
    QWidget* parent)
    : QWidget(parent),
      ui(new Ui::SurakartaGameBrowserWindow),
      black_handler_(std::make_shared<SurakartaAgentInteractiveHandler>()),
      white_handler_(std::make_shared<SurakartaAgentInteractiveHandler>()) {
    // parse manual
    PieceColor current_color = PieceColor::BLACK;
    for (int i = 0; i < manual.size();) {
        if (i + 1 < manual.size() && manual[i] == '#') {
            break;
        }
        if (i + 6 < manual.size()) {
            int from_x = manual[i] - 'A';
            int from_y = manual[i + 1] - '1';
            int to_x = manual[i + 3] - 'A';
            int to_y = manual[i + 4] - '1';
            if (manual[i + 2] != '-' || manual[i + 5] != ' ' ||
                from_x < 0 || from_x >= BOARD_SIZE || from_y < 0 || from_y >= BOARD_SIZE) {
                printf("Warning: Invalid manual\n");
            }
            SurakartaMove move = SurakartaMove(SurakartaPosition(from_x, from_y), SurakartaPosition(to_x, to_y), current_color);
            (current_color == PieceColor::BLACK ? &black_moves : &white_moves)->push_back(move);
            current_color = ReverseColor(current_color);
            i += 6;
            continue;
        }
        break;
    }

    // set up daemon
    black_handler_->BlockAgentCreation();
    white_handler_->BlockAgentCreation();
    daemon_thread_ = std::make_unique<SurakartaDaemonThread>(
        std::make_unique<SurakartaDaemon>(BOARD_SIZE, MAX_NO_CAPTURE_ROUND,
                                          black_handler_->GetAgentFactory(), white_handler_->GetAgentFactory()));
    daemon_thread_->start();

    // set up UI
    ui->setupUi(this);
    ui->surakarta_board->LoadN(BOARD_SIZE);

    // set up event handlers

    // ui events
    connect(ui->commitButton, &QPushButton::clicked, this, &SurakartaGameBrowserWindow::OnCommitButtonClicked);
    ui->surakarta_board->UsePieceUpdater(
        [&]() {
            return black_handler_->MyColor() == PieceColor::BLACK ? black_handler_->CopyMyPieces() : black_handler_->CopyOpponentPieces();
        },
        [&]() {
            return black_handler_->MyColor() == PieceColor::BLACK ? black_handler_->CopyOpponentPieces() : black_handler_->CopyMyPieces();
        });

    // handler event chains
    // since event from the handler may come from another thread, we need to use signal-slot mechanism

    black_handler_->OnAgentCreated.AddListener([&]() {
        onAgentCreated();
    });
    connect(this, &SurakartaGameBrowserWindow::onAgentCreated, this, &SurakartaGameBrowserWindow::OnAgentCreated);

    black_handler_->OnWaitingForMove.AddListener([&]() {
        onWaitingForMove();
    });
    white_handler_->OnWaitingForMove.AddListener([&]() {
        onWaitingForMove();
    });
    connect(this, &SurakartaGameBrowserWindow::onWaitingForMove, this, &SurakartaGameBrowserWindow::OnWaitingForMove);

    black_handler_->OnMoveCommitted.AddListener([&](SurakartaMoveTrace trace) {
        onMoveCommitted(trace);
    });
    connect(this, &SurakartaGameBrowserWindow::onMoveCommitted, this, &SurakartaGameBrowserWindow::OnMoveCommitted);

    black_handler_->OnGameEnded.AddListener([&](SurakartaMoveResponse response) {
        onGameEnded(response);
    });
    connect(this, &SurakartaGameBrowserWindow::onGameEnded, this, &SurakartaGameBrowserWindow::OnGameEnded);

    // Allow agent creation
    black_handler_->UnblockAgentCreation();
    white_handler_->UnblockAgentCreation();
}

SurakartaGameBrowserWindow::~SurakartaGameBrowserWindow() {
    daemon_thread_->wait();
    delete ui;
}

void SurakartaGameBrowserWindow::closeEvent(QCloseEvent* event) {
    daemon_thread_->terminate();
    closed();
    event->accept();
}

void SurakartaGameBrowserWindow::UpdateInfo() {
    auto& current_list = black_handler_->CopyGameInfo().current_player_ == PieceColor::BLACK ? black_moves : white_moves;
    if (current_list.size() > 0) {
        ui->surakarta_board->SelectPiece(current_list.front().from.x, current_list.front().from.y);
        ui->surakarta_board->SelectDestination(current_list.front().to.x, current_list.front().to.y);
        auto current_handler = black_handler_->CopyGameInfo().current_player_ == PieceColor::BLACK ? black_handler_ : white_handler_;
        current_handler->SelectPiece(current_list.front().from);
        current_handler->SelectDestination(current_list.front().to);
    }
}

void SurakartaGameBrowserWindow::OnCommitButtonClicked() {
    auto current_handler = black_handler_->CopyGameInfo().current_player_ == PieceColor::BLACK ? black_handler_ : white_handler_;
    auto& current_list = black_handler_->CopyGameInfo().current_player_ == PieceColor::BLACK ? black_moves : white_moves;
    if (current_list.empty()) {
        QMessageBox::warning(this, "Error", "Manual ended, but game is not ended");
        return;
    }
    current_list.pop_front();
    const auto ret = current_handler->CommitMove();
    assert(ret);
}

void SurakartaGameBrowserWindow::OnAgentCreated() {
    ui->surakarta_board->ReloadPieces(black_handler_->CopyMyPieces(), white_handler_->CopyMyPieces());
    UpdateInfo();
}

void SurakartaGameBrowserWindow::OnWaitingForMove() {
    UpdateInfo();
}

void SurakartaGameBrowserWindow::OnMoveCommitted(SurakartaMoveTrace trace) {
    ui->surakarta_board->OnMoveCommitted(trace);
    UpdateInfo();
}

void SurakartaGameBrowserWindow::OnGameEnded(SurakartaMoveResponse response) {
    QMessageBox msgBox;
    msgBox.setWindowTitle("Manual Game Ended");
    auto winner_str = std::string("Winner: ") + (response.GetWinner() == PieceColor::NONE
                                                     ? "Stalemate"
                                                     : (response.GetWinner() == PieceColor::BLACK ? "Black" : "White"));
    auto end_reason_str = std::string("End reason: ") + SurakartaToString(response.GetEndReason());
    auto last_move_str = std::string("Last move type: ") + SurakartaToString(response.GetMoveReason());
    auto game_info = black_handler_->CopyGameInfo();
    auto total_move_str = std::string("Total moves: ") + std::to_string(game_info.num_round_);
    msgBox.setText(QString::fromStdString(winner_str + "\n" + end_reason_str + "\n" + last_move_str + "\n" + total_move_str));
    msgBox.exec();
    close();
}
