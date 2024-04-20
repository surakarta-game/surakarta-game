#pragma once

#include <QMainWindow>
#include <memory>
#include "surakarta_session_window.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   private:
    Ui::MainWindow* ui;
    std::unique_ptr<SurakartaSessionWindow> sessionWindow;

   private slots:
    void StartSession();
};
