#include "surakarta_agent_setting_widget.h"

#include "surakarta.h"
#include "surakarta_network.h"
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
    connect(ui->pushButton_connect, &QPushButton::clicked, this, &SurakartaAgentSettingWidget::OnConnectButtonClicked);
    connect(this, &SurakartaAgentSettingWidget::onConnectionSuccess, this, &SurakartaAgentSettingWidget::OnConnectionSuccess);
    connect(this, &SurakartaAgentSettingWidget::onConnectionFailed, this, &SurakartaAgentSettingWidget::OnConnectionFailed);
    OnAgentTypeChanged(ui->comboBox->currentIndex());
}

SurakartaAgentSettingWidget::~SurakartaAgentSettingWidget() {
    if (connect_thread && connect_thread->joinable())
        connect_thread->join();
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
    } else if (index == 3) {
        if (remote_agent_factory) {
            return std::move(remote_agent_factory);
        } else {
            return std::nullopt;
        }
    } else {
        return std::nullopt;
    }
}

PieceColor SurakartaAgentSettingWidget::GetPieceColorRistriction() const {
    if (ui->comboBox->currentIndex() == 3) {
        if (remote_agent_factory)
            return remote_agent_factory->AssignedColor();
        else
            return PieceColor::UNKNOWN;
    } else {
        return ui->comboBox_color_request->currentIndex() == 1
                   ? PieceColor::BLACK
               : ui->comboBox_color_request->currentIndex() == 2
                   ? PieceColor::WHITE
                   : PieceColor::NONE;
    }
}

void SurakartaAgentSettingWidget::DisableRemote() {
    ui->comboBox->removeItem(3);
}

void SurakartaAgentSettingWidget::OnConnectButtonClicked() {
    ui->pushButton_connect->setEnabled(false);
    connect_message_box = std::make_unique<QMessageBox>(this);
    connect_message_box->setWindowTitle("Connect to Server");
    connect_message_box->setText("Connecting and waiting for peer...");
    connect_message_box->show();
    if (connect_thread)
        connect_thread->join();
    connect_thread = std::make_unique<std::thread>([this]() {
        try {
            int port;
            try {
                port = std::stoi(ui->lineEdit_port->text().toStdString());
            } catch (const std::exception& e) {
                throw std::runtime_error("Port must be a number");
            }
            int room;
            try {
                room = std::stoi(ui->lineEdit_room->text().toStdString());
            } catch (const std::exception& e) {
                throw std::runtime_error("Room must be a number");
            }
            remote_agent_factory = std::make_unique<SurakartaAgentRemoteFactory>(
                ui->lineEdit_server_address->text().toStdString(),
                port,
                ui->lineEdit_username->text().toStdString(),
                room,
                ui->comboBox_color_request->currentIndex() == 1
                    ? PieceColor::BLACK
                : ui->comboBox_color_request->currentIndex() == 2
                    ? PieceColor::WHITE
                    : PieceColor::NONE);
            onConnectionSuccess();
        } catch (const std::exception& e) {
            onConnectionFailed(QString(e.what()));
        }
    });
}

void SurakartaAgentSettingWidget::OnConnectionSuccess() {
    if (connect_message_box) {
        connect_message_box->setWindowTitle("Success");
        connect_message_box->setText("Connected");
        connect_message_box->close();
        connect_message_box.reset();
    }
    ui->pushButton_connect->setEnabled(true);
}

void SurakartaAgentSettingWidget::OnConnectionFailed(QString message) {
    if (connect_message_box) {
        connect_message_box->setWindowTitle("Error");
        connect_message_box->setText(message);
        // connect_message_box.reset();
    }
    ui->pushButton_connect->setEnabled(true);
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
        ui->label_room->setVisible(true);
        ui->lineEdit_room->setVisible(true);
        ui->label_username->setVisible(true);
        ui->lineEdit_username->setVisible(true);
        ui->pushButton_connect->setVisible(true);
    } else {
        ui->label_server_address->setVisible(false);
        ui->lineEdit_server_address->setVisible(false);
        ui->label_port->setVisible(false);
        ui->lineEdit_port->setVisible(false);
        ui->label_room->setVisible(false);
        ui->lineEdit_room->setVisible(false);
        ui->label_username->setVisible(false);
        ui->lineEdit_username->setVisible(false);
        ui->pushButton_connect->setVisible(false);
    }
}
