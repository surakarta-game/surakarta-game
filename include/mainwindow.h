#pragma once

#include <QMainWindow>
#include <QTimer>
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class SurakartaSessionWindow;

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   private:
    Ui::MainWindow* ui;
    std::unique_ptr<SurakartaSessionWindow> sessionWindow;
    QTimer* timer;
    int circle_time = 1000;

   private slots:
    void StartSession();
    void OnTimerTimeout();
    void ReShow();
};
