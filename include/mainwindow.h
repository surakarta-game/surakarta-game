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
class SurakartaGameBrowserWindow;

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   private:
    Ui::MainWindow* ui;
    std::shared_ptr<SurakartaSessionWindow> sessionWindow;
    std::shared_ptr<SurakartaGameBrowserWindow> browserWindow;
    std::vector<std::shared_ptr<SurakartaSessionWindow>> garbage;  // to prevent old session window from being deleted
                                                                   // deleteing old session window may cause crash
    std::vector<std::shared_ptr<SurakartaGameBrowserWindow>> garbage2;
    QTimer* timer;
    int circle_time = 1000;
    int max_time = 15000;

   private slots:
    void StartSession();
    void OpenManual();
    void OnTimerTimeout();
    void OnSessionWindowClosed();
    void OnManualWindowClosed();
};
