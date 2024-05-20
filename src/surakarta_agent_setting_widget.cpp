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

        virtual std::unique_ptr<SurakartaMoveWeightUtilBase> CreateUtil(std::shared_ptr<SurakartaBoard> board, PieceColor curr_colour) override {
            return std::make_unique<SurakartaLetYouWinMoveWeightUtil>(board, curr_colour, inner_factory_->CreateUtil(board, curr_colour));
        }

       private:
        std::shared_ptr<SurakartaAgentMineFactory::SurakartaMoveWeightUtilBaseFactory> inner_factory_;
    };

   private:
    std::shared_ptr<SurakartaMoveWeightUtilBase> util;
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
        ui->label_2->setVisible(true);
        ui->lineEdit->setVisible(true);
        ui->lineEdit->setEnabled(true);
    } else {
        ui->label_2->setVisible(false);
        ui->lineEdit->setVisible(false);
        ui->lineEdit->setEnabled(false);
    }
    if (index == 3) {
        ui->label_server_address->setVisible(true);
        ui->lineEdit_server_address->setVisible(true);
        ui->label_port->setVisible(true);
        ui->lineEdit_port->setVisible(true);
        ui->label_color_request->setVisible(true);
        ui->comboBox_color_request->setVisible(true);
        ui->label_room->setVisible(true);
        ui->lineEdit_room->setVisible(true);
        ui->label_username->setVisible(true);
        ui->lineEdit_username->setVisible(true);
    } else {
        ui->label_server_address->setVisible(false);
        ui->lineEdit_server_address->setVisible(false);
        ui->label_port->setVisible(false);
        ui->lineEdit_port->setVisible(false);
        ui->label_color_request->setVisible(false);
        ui->comboBox_color_request->setVisible(false);
        ui->label_room->setVisible(false);
        ui->lineEdit_room->setVisible(false);
        ui->label_username->setVisible(false);
        ui->lineEdit_username->setVisible(false);
    }
}
