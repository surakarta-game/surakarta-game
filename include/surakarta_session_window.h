#pragma once

#include <QDebug>
#include <QFile>
#include <QResource>
#include <QString>
#include <QTextStream>
#include <QTimer>
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
        int max_time,
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
    QTimer* timer = new QTimer(this);
    int max_time;
    int r_time;
    void WriteManual(SurakartaMoveTrace trace);
    QFile Manual;

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
    void StartTimer();
    void onTimeout();
    void UpdateTime();
    // {
    //     if (r_time > 0) {
    //         std::cout << "Countdown: " << r_time << std::endl;
    //         r_time--;
    //     }
    //     else {
    //         timer->stop();
    //         std::cout << "Time's up!" << std::endl;
    //         // 这里可以添加倒计时结束时需要执行的代码
    //     }
    // }
    void OnBoardClicked(int x, int y);
    void OnCommitButtonClicked();
    void OnAiSuggestionButtonClicked();

    void OnAgentCreated();
    void OnWaitingForMove();
    void OnMoveCommitted(SurakartaMoveTrace trace);
};
