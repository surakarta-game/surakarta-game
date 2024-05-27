#pragma once

#include <QDebug>
#include <QFile>
#include <QResource>
#include <QString>
#include <QTextStream>
#include <QTimer>
#include <QWidget>
#include <list>
#include <queue>
#include "surakarta.h"
#include "surakarta_daemon_thread.h"

namespace Ui {
class SurakartaGameBrowserWindow;
}

class SurakartaGameBrowserWindow : public QWidget {
    Q_OBJECT

   public:
    explicit SurakartaGameBrowserWindow(
        std::string manual,
        QWidget* parent = nullptr);
    ~SurakartaGameBrowserWindow();

    void UpdateInfo();

   private:
    Ui::SurakartaGameBrowserWindow* ui;
    std::shared_ptr<SurakartaAgentInteractiveHandler> black_handler_;
    std::shared_ptr<SurakartaAgentInteractiveHandler> white_handler_;
    std::unique_ptr<SurakartaDaemonThread> daemon_thread_;
    std::list<SurakartaMove> black_moves;
    std::list<SurakartaMove> white_moves;
    void closeEvent(QCloseEvent* event) override;

   signals:
    void closed();
    void onAgentCreated();
    void onWaitingForMove();
    void onMoveCommitted(SurakartaMoveTrace trace);
    void onGameEnded(SurakartaMoveResponse response);

   private slots:
    void OnCommitButtonClicked();

    void OnAgentCreated();
    void OnWaitingForMove();
    void OnMoveCommitted(SurakartaMoveTrace trace);
    void OnGameEnded(SurakartaMoveResponse response);
};
