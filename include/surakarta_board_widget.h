#ifndef SURAKARTA_BOARD_VIEW_H
#define SURAKARTA_BOARD_VIEW_H

#include <QWidget>

class SurakartaBoardWidget : public QWidget {
    Q_OBJECT

   public:
    explicit SurakartaBoardWidget(QWidget* parent = nullptr);
    ~SurakartaBoardWidget();

    void loadSize(int size = 500, int n_board = 6);

   private:
    void paintEvent(QPaintEvent* event) override;

    void paintBlackPiece(double x, double y);
    void paintWhitePiece(double x, double y);
    void paintSelection(int x, int y);

    int n_board_;
    int size_;
    double gap_;
    int length;
    int rect_corner;
    double pixel;
};

#endif  // SURAKARTA_BOARD_VIEW_H
