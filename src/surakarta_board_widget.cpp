#include "surakarta_board_widget.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <chrono>
#include <iostream>

SurakartaBoardWidget::SurakartaBoardWidget(QWidget* parent)
    : QWidget(parent) {
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(0xEA, 0xD7, 0xAF));
    setAutoFillBackground(true);
    setPalette(pal);
    total_length = 0;
    pieces = std::make_unique<std::vector<PieceStatus>>();
    animations = std::make_unique<std::vector<PieceAnimation>>();
    timer = std::make_unique<QTimer>(this);
    timer->setInterval(1000 / 60);
    timer->start();
    connect(timer.get(), &QTimer::timeout, this, &SurakartaBoardWidget::OnTimerTick);
}

void SurakartaBoardWidget::LoadSize(int size, int n_board) {
    if (n_board % 2 != 0) {
        QMessageBox::warning(this, "Error", "The number of boards must be even.");
    }
    size_ = size;
    n_board_ = n_board;
    gap_ = size / ((n_board_ - 1) + 2 * (n_board_ / 2 - 1) + 0.5 + 0.5);
    length = (n_board_ - 1) * gap_;
    rect_corner = gap_ * (0.5 + n_board_ / 2 - 1);
    pixel = gap_ / 10;
    setFixedSize(size_, size_);
}

void SurakartaBoardWidget::ReloadPieces(
    std::unique_ptr<std::vector<SurakartaPositionWithId>> black_pieces,
    std::unique_ptr<std::vector<SurakartaPositionWithId>> white_pieces) {
    total_length = black_pieces->size() + white_pieces->size();
    pieces = std::make_unique<std::vector<PieceStatus>>(total_length);
    for (int i = 0; i < black_pieces->size(); i++) {
        PieceStatus status{
            .x = static_cast<double>(black_pieces->at(i).x),
            .y = static_cast<double>(black_pieces->at(i).y),
            .color = PieceColor::BLACK};
        pieces->at(i) = status;
    }
    for (int i = 0; i < white_pieces->size(); i++) {
        PieceStatus status{
            .x = static_cast<double>(white_pieces->at(i).x),
            .y = static_cast<double>(white_pieces->at(i).y),
            .color = PieceColor::WHITE};
        pieces->at(black_pieces->size() + i) = status;
    }
    animations = std::make_unique<std::vector<PieceAnimation>>(total_length);
    for (int i = 0; i < total_length; i++) {
        PieceAnimation animation{
            .piece_index = i,
            .start_time = 0,
            .animation = SurakartaAnimation(),
        };
        animations->at(i) = std::move(animation);
    }
}

void SurakartaBoardWidget::SelectPiece(int x, int y) {
    is_piece_selected = true;
    selected_piece_x = x;
    selected_piece_y = y;
    update();
}

void SurakartaBoardWidget::UnselectPiece() {
    is_piece_selected = false;
    update();
}

void SurakartaBoardWidget::SelectDestination(int x, int y) {
    is_destination_selected = true;
    selected_destination_x = x;
    selected_destination_y = y;
    update();
}

void SurakartaBoardWidget::UnselectDestination() {
    is_destination_selected = false;
    update();
}

SurakartaBoardWidget::~SurakartaBoardWidget() {
}

void SurakartaBoardWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setPen(QPen(Qt::gray, 2 * pixel));
    for (int i = 0; i < n_board_; i++) {
        painter.drawLine(rect_corner + i * gap_, rect_corner, rect_corner + i * gap_, rect_corner + length);
        painter.drawLine(rect_corner, rect_corner + i * gap_, rect_corner + length, rect_corner + i * gap_);
    }
    for (int r = 1; r <= n_board_ / 2 - 1; r++) {
        painter.drawArc(rect_corner - r * gap_, rect_corner - r * gap_, 2 * r * gap_, 2 * r * gap_, 0, 90 * 3 * 16);
        painter.drawArc(rect_corner - r * gap_, rect_corner + length - r * gap_, 2 * r * gap_, 2 * r * gap_, 90 * 16, 90 * 3 * 16);
        painter.drawArc(rect_corner + length - r * gap_, rect_corner - r * gap_, 2 * r * gap_, 2 * r * gap_, 90 * 3 * 16, 90 * 3 * 16);
        painter.drawArc(rect_corner + length - r * gap_, rect_corner + length - r * gap_, 2 * r * gap_, 2 * r * gap_, 90 * 2 * 16, 90 * 3 * 16);
    }
    for (int i = 0; i < pieces->size(); i++) {
        if (pieces->at(i).color == PieceColor::BLACK) {
            PaintBlackPiece(pieces->at(i).x, pieces->at(i).y);
        } else if (pieces->at(i).color == PieceColor::WHITE) {
            PaintWhitePiece(pieces->at(i).x, pieces->at(i).y);
        }
    }
    if (is_piece_selected) {
        PaintSelection(selected_piece_x, selected_piece_y);
    }
    if (is_destination_selected) {
        PaintSelection(selected_destination_x, selected_destination_y);
    }
}

void SurakartaBoardWidget::mousePressEvent(QMouseEvent* event) {
    int x = (event->pos().x() - rect_corner + gap_ / 2) / gap_;
    int y = (event->pos().y() - rect_corner + gap_ / 2) / gap_;
    if (x >= 0 && x < n_board_ && y >= 0 && y < n_board_) {
        onBoardClicked(x, y);
    }
}

void SurakartaBoardWidget::PaintBlackPiece(double x, double y) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rect_corner + x * gap_ - 3 * pixel, rect_corner + y * gap_ - 3 * pixel, 6 * pixel, 6 * pixel);
}

void SurakartaBoardWidget::PaintWhitePiece(double x, double y) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rect_corner + x * gap_ - 3 * pixel, rect_corner + y * gap_ - 3 * pixel, 6 * pixel, 6 * pixel);
}

void SurakartaBoardWidget::PaintSelection(int x, int y) {
    if (x < 0 || x >= n_board_ || y < 0 || y >= n_board_) {
        QMessageBox::warning(this, "Error", "Invalid position.");
        return;
    }
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setBrush(Qt::red);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect_corner + x * gap_ - 4 * pixel, rect_corner + y * gap_ - 4 * pixel, 2 * pixel, 1 * pixel);
    painter.drawRect(rect_corner + x * gap_ - 4 * pixel, rect_corner + y * gap_ - 3 * pixel, 1 * pixel, 1 * pixel);
    painter.drawRect(rect_corner + x * gap_ + 2 * pixel, rect_corner + y * gap_ - 4 * pixel, 2 * pixel, 1 * pixel);
    painter.drawRect(rect_corner + x * gap_ + 3 * pixel, rect_corner + y * gap_ - 3 * pixel, 1 * pixel, 1 * pixel);
    painter.drawRect(rect_corner + x * gap_ - 4 * pixel, rect_corner + y * gap_ + 2 * pixel, 1 * pixel, 2 * pixel);
    painter.drawRect(rect_corner + x * gap_ - 3 * pixel, rect_corner + y * gap_ + 3 * pixel, 1 * pixel, 1 * pixel);
    painter.drawRect(rect_corner + x * gap_ + 3 * pixel, rect_corner + y * gap_ + 2 * pixel, 1 * pixel, 2 * pixel);
    painter.drawRect(rect_corner + x * gap_ + 2 * pixel, rect_corner + y * gap_ + 3 * pixel, 1 * pixel, 1 * pixel);
}

void SurakartaBoardWidget::RemovePiece(int index) {
    pieces->at(index).color = PieceColor::NONE;
    update();
}

void SurakartaBoardWidget::OnTimerTick() {
    for (int i = 0; i < total_length; i++) {
        if (!animations->at(i).animation.Empty()) {
            PieceStatus status = pieces->at(i);
            const int time = std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::system_clock::now().time_since_epoch())
                                 .count() -
                             animations->at(i).start_time;
            SurakartaAnimation::Point point = animations->at(i).animation.PositionAt(time);
            // printf("(%d)  x: %f, y: %f\n", time, point.x, point.y);
            status.x = point.x;
            status.y = point.y;
            pieces->at(i) = status;
            if (animations->at(i).animation.Finished()) {
                animations->at(i).animation = SurakartaAnimation();
            }
        }
    }
    update();
}

void SurakartaBoardWidget::OnMoveCommitted(const SurakartaMoveTrace& trace) {
    PieceAnimation animation{
        .piece_index = trace.moved_piece.id,
        .start_time = (int)std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count(),
        .animation = SurakartaAnimation(trace.path, 1000),
    };
    (*animations)[trace.moved_piece.id] = std::move(animation);
    if (trace.captured_piece.id != -1)
        QTimer::singleShot(1000, this, [=]() {
            this->RemovePiece(trace.captured_piece.id);
        });
}
