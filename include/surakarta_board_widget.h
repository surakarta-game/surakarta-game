#pragma once

#include <QWidget>
#include <vector>
#include "surakarta.h"

class SurakartaBoardWidget : public QWidget {
    Q_OBJECT

   public:
    explicit SurakartaBoardWidget(
        QWidget* parent = nullptr,
        std::function<std::unique_ptr<std::vector<SurakartaPositionWithId>>()> black_pieces_getter = nullptr,
        std::function<std::unique_ptr<std::vector<SurakartaPositionWithId>>()> white_pieces_getter = nullptr);
    ~SurakartaBoardWidget();

    void LoadN(int n_board = 6);
    int GetN() const {
        return n_board_;
    }
    void SetAnimationMilliseconds(int milliseconds) {
        animation_milliseconds = milliseconds;
    }
    int GetAnimationMilliseconds() const {
        return animation_milliseconds;
    }
    void ReloadPieces(
        std::unique_ptr<std::vector<SurakartaPositionWithId>> black_pieces,
        std::unique_ptr<std::vector<SurakartaPositionWithId>> white_pieces);
    void UsePieceUpdater(
        std::function<std::unique_ptr<std::vector<SurakartaPositionWithId>>()> black_pieces_getter,
        std::function<std::unique_ptr<std::vector<SurakartaPositionWithId>>()> white_pieces_getter);
    void SelectPiece(int x, int y);
    void UnselectPiece();
    void SelectDestination(int x, int y);
    void UnselectDestination();
    //PieceColor GetColorOfPosition(int x, int y);

   private:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    void PaintBlackPiece(double x, double y);
    void PaintWhitePiece(double x, double y);
    void PaintSelection(int x, int y);

    void TrySyncPieces();

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
        std::shared_ptr<SurakartaAnimationBase> animation;
    };

    int total_length;
    std::unique_ptr<std::vector<PieceStatus>> pieces;
    std::unique_ptr<std::vector<PieceAnimation>> animations;
    std::unique_ptr<QTimer> timer;
    int animation_milliseconds = 1000;

    bool is_piece_selected = false;
    int selected_piece_x = 0;
    int selected_piece_y = 0;
    bool is_destination_selected = false;
    int selected_destination_x = 0;
    int selected_destination_y = 0;

    std::function<std::unique_ptr<std::vector<SurakartaPositionWithId>>()> black_pieces_getter_;
    std::function<std::unique_ptr<std::vector<SurakartaPositionWithId>>()> white_pieces_getter_;

    std::mutex mutex;

   public slots:
    void OnTimerTick();
    void OnMoveCommitted(const SurakartaMoveTrace& trace);

   signals:
    void onBoardClicked(int x, int y);

   public:
    void RemovePiece(int index);
};
