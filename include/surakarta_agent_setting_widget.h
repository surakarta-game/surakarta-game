#pragma once
#include <QMessageBox>
#include <QWidget>
#include <optional>
#include <thread>
#include "surakarta_daemon.h"

namespace Ui {
class SurakartaAgentSettingWidget;
}

class SurakartaAgentSettingWidget : public QWidget {
    Q_OBJECT

   public:
    explicit SurakartaAgentSettingWidget(QWidget* parent = nullptr);
    ~SurakartaAgentSettingWidget();
    std::optional<std::unique_ptr<SurakartaDaemon::AgentFactory>> CreateAgentFactory();
    void DisableRemote();

   private:
    Ui::SurakartaAgentSettingWidget* ui;
    std::unique_ptr<SurakartaDaemon::AgentFactory> remote_agent_factory;
    std::unique_ptr<QMessageBox> connect_message_box;
    std::unique_ptr<std::thread> connect_thread;

   signals:
    void onConnectionSuccess();
    void onConnectionFailed(QString message);

   private slots:
    void OnAgentTypeChanged(int index);
    void OnConnectButtonClicked();
    void OnConnectionSuccess();
    void OnConnectionFailed(QString message);
};
