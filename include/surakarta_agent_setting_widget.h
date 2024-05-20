#pragma once
#include <QWidget>
#include <optional>
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

   private slots:
    void OnAgentTypeChanged(int index);
};
