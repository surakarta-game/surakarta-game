#ifndef SURAKARTA_SESSION_WINDOW_H
#define SURAKARTA_SESSION_WINDOW_H

#include <QWidget>

namespace Ui {
class SurakartaSessionWindow;
}

class SurakartaSessionWindow : public QWidget {
    Q_OBJECT

   public:
    explicit SurakartaSessionWindow(QWidget* parent = nullptr);
    ~SurakartaSessionWindow();

   private:
    Ui::SurakartaSessionWindow* ui;

    void closeEvent(QCloseEvent* event) override;

   signals:
    void closed();
};

#endif  // SURAKARTA_SESSION_WINDOW_H
