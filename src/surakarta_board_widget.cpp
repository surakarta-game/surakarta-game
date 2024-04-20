#include "surakarta_board_widget.h"
#include <QMessageBox>
#include <QPainter>
#include <iostream>

SurakartaBoardWidget::SurakartaBoardWidget(QWidget* parent)
    : QWidget(parent) {
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(0xEA, 0xD7, 0xAF));
    setAutoFillBackground(true);
    setPalette(pal);
}

void SurakartaBoardWidget::loadSize(int size, int n_board) {
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

    paintWhitePiece(0, 0);
    paintWhitePiece(1, 0);
    paintSelection(1, 0);
    paintWhitePiece(n_board_ - 1, 0);
    paintBlackPiece(0, n_board_ - 1);
    paintSelection(1, n_board_ - 1);
}

void SurakartaBoardWidget::paintBlackPiece(double x, double y) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rect_corner + x * gap_ - 3 * pixel, rect_corner + y * gap_ - 3 * pixel, 6 * pixel, 6 * pixel);
}

void SurakartaBoardWidget::paintWhitePiece(double x, double y) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rect_corner + x * gap_ - 3 * pixel, rect_corner + y * gap_ - 3 * pixel, 6 * pixel, 6 * pixel);
}

void SurakartaBoardWidget::paintSelection(int x, int y) {
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
