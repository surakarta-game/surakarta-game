#pragma once

#include <QWidget>
#include <vector>
#include "surakarta.h"
#include "surakarta_animation.h"

class SurakartaBoardWidget : public QWidget {
    Q_OBJECT

   public:
    explicit SurakartaBoardWidget(QWidget* parent = nullptr);
    ~SurakartaBoardWidget();

    void LoadSize(int size = 500, int n_board = 6);
    void ReloadPieces(
        std::unique_ptr<std::vector<SurakartaPositionWithId>> black_pieces,
        std::unique_ptr<std::vector<SurakartaPositionWithId>> white_pieces);
    void SelectPiece(int x, int y);
    void UnselectPiece();
    void SelectDestination(int x, int y);
    void UnselectDestination();

   private:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

    void PaintBlackPiece(double x, double y);
    void PaintWhitePiece(double x, double y);
    void PaintSelection(int x, int y);

    int n_board_;
    int size_;
    double gap_;
    int length;
    int rect_corner;
    double pixel;

    struct PieceStatus {
        double x;
        double y;
        PieceColor color;
    };

    struct PieceAnimation {
        int piece_index;
        int start_time;
        SurakartaAnimation animation;
    };

    int total_length;
    std::unique_ptr<std::vector<PieceStatus>> pieces;
    std::unique_ptr<std::vector<PieceAnimation>> animations;
    std::unique_ptr<QTimer> timer;

    bool is_piece_selected;
    int selected_piece_x;
    int selected_piece_y;
    bool is_destination_selected;
    int selected_destination_x;
    int selected_destination_y;

   public slots:
    void OnTimerTick();
    void OnMoveCommitted(const SurakartaMoveTrace& trace);

   signals:
    void onBoardClicked(int x, int y);

   public:
    void RemovePiece(int index);
};
