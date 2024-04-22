#pragma once

#include <QWidget>
#include <queue>
#include "surakarta.h"
#include "surakarta_daemon_thread.h"

namespace Ui {
class SurakartaSessionWindow;
}

class SurakartaSessionWindow : public QWidget {
    Q_OBJECT

   public:
    explicit SurakartaSessionWindow(
        std::shared_ptr<SurakartaAgentInteractiveHandler> handler,
        std::unique_ptr<SurakartaDaemonThread> daemon_thread,
        QWidget* parent = nullptr);
    ~SurakartaSessionWindow();

    void UpdateInfo();
    bool SelectPiece(SurakartaPosition pos);
    bool SelectDestination(SurakartaPosition pos);

   private:
    Ui::SurakartaSessionWindow* ui;
    std::shared_ptr<SurakartaAgentInteractiveHandler> handler_;
    std::unique_ptr<SurakartaDaemonThread> daemon_thread_;
    void closeEvent(QCloseEvent* event) override;

   signals:
    // emulation signals are connected to the corresponding slots
    // they are used to avoid cross-thread UI operations
    void emulateBoardClicked(int x, int y);
    void emulateCommitButtonClicked();

    void closed();
    void onAgentCreated();
    void onWaitingForMove();
    void onMoveCommitted(SurakartaMoveTrace trace);

   private slots:
    void OnBoardClicked(int x, int y);
    void OnCommitButtonClicked();
    void OnAiSuggestionButtonClicked();

    void OnAgentCreated();
    void OnWaitingForMove();
    void OnMoveCommitted(SurakartaMoveTrace trace);
};
