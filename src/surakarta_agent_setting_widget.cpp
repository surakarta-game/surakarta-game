#include "surakarta_agent_setting_widget.h"

#include "surakarta.h"
#include "ui_surakarta_agent_setting_widget.h"

class SurakartaLetYouWinMoveWeightUtil : public SurakartaMoveWeightUtilBase {
   public:
    SurakartaLetYouWinMoveWeightUtil(
        std::shared_ptr<SurakartaBoard> board,
        PieceColor curr_colour,
        std::shared_ptr<SurakartaMoveWeightUtilBase> inner_util)
        : SurakartaMoveWeightUtilBase(board, curr_colour), util(inner_util) {}

    virtual double CalculateMoveWeight(SurakartaMove move) override {
        return 0 - util->CalculateMoveWeight(move);
    }

    class SurakartaLetYouWinMoveWeightUtilFactory : public SurakartaAgentMineFactory::SurakartaMoveWeightUtilBaseFactory {
       public:
        SurakartaLetYouWinMoveWeightUtilFactory(
            std::shared_ptr<SurakartaAgentMineFactory::SurakartaMoveWeightUtilBaseFactory> inner_factory)
            : inner_factory_(inner_factory) {}

        virtual std::unique_ptr<SurakartaMoveWeightUtilBase> CreateUtil(SurakartaDaemon& daemon, PieceColor my_color) override {
            return std::make_unique<SurakartaLetYouWinMoveWeightUtil>(daemon.Board(), my_color, inner_factory_->CreateUtil(daemon, my_color));
        }

       private:
        std::shared_ptr<SurakartaAgentMineFactory::SurakartaMoveWeightUtilBaseFactory> inner_factory_;
    };

   private:
    std::shared_ptr<SurakartaMoveWeightUtilBase>
        util;
};

SurakartaAgentSettingWidget::SurakartaAgentSettingWidget(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::SurakartaAgentSettingWidget) {
    ui->setupUi(this);
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SurakartaAgentSettingWidget::OnAgentTypeChanged);
    OnAgentTypeChanged(ui->comboBox->currentIndex());
}

SurakartaAgentSettingWidget::~SurakartaAgentSettingWidget() {
    delete ui;
}

std::optional<std::unique_ptr<SurakartaDaemon::AgentFactory>> SurakartaAgentSettingWidget::CreateAgentFactory() {
    const auto index = ui->comboBox->currentIndex();
    if (index == 0) {  // Smart Agent
        const auto text = ui->lineEdit->text().toStdString();
        // is valid number
        if (std::stoi(text) > 0) {
            return std::make_unique<SurakartaAgentMineFactory>(
                std::make_shared<SurakartaAgentMineFactory::SurakartaMoveWeightUtilFactory>(std::stoi(text)));
        } else {
            return std::nullopt;
        }
    } else if (index == 1) {  // Random Agent
        return std::make_unique<SurakartaAgentRandomFactory>();
    } else if (index == 2) {  // Let You Win
        const auto text = ui->lineEdit->text().toStdString();
        // is valid number
        if (std::stoi(text) > 0) {
            return std::make_unique<SurakartaAgentMineFactory>(
                std::make_shared<SurakartaLetYouWinMoveWeightUtil::SurakartaLetYouWinMoveWeightUtilFactory>(
                    std::make_shared<SurakartaAgentMineFactory::SurakartaMoveWeightUtilFactory>(std::stoi(text))));
        } else {
            return std::nullopt;
        }
    } else {
        return std::nullopt;
    }
}

void SurakartaAgentSettingWidget::OnAgentTypeChanged(int index) {
    if (index == 0 || index == 2) {
        ui->lineEdit->setEnabled(true);
    } else {
        ui->lineEdit->setEnabled(false);
    }
}
