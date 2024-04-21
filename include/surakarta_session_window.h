#pragma once

#include <QWidget>
#include <queue>
#include "surakarta.h"

namespace Ui {
class SurakartaSessionWindow;
}

class SurakartaSessionWindow : public QWidget {
    Q_OBJECT

   public:
    explicit SurakartaSessionWindow(
        std::shared_ptr<SurakartaAgentInteractiveHandler> handler,
        QWidget* parent = nullptr);
    ~SurakartaSessionWindow();

    void UpdateInfo();

   private:
    Ui::SurakartaSessionWindow* ui;
    std::shared_ptr<SurakartaAgentInteractiveHandler> handler_;
    std::mutex mutex_;
    std::queue<SurakartaMoveTrace> move_queue_;
    void closeEvent(QCloseEvent* event) override;

   signals:
    void closed();
    void onMoveCommitted();

   private slots:
    void OnBoardClicked(int x, int y);
    void OnCommitButtonClicked();
    void OnMoveCommitted();
};
