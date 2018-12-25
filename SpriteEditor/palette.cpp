#include "palette.h"

QColor rgb[COLORS] = {QColor(0, 0, 0), QColor(255, 255, 255)};

Palette::Palette(QWidget *parent)
{
    color = 0;

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

QSize Palette::minimumSizeHint() const
{
    return QSize(COLORS * 20 + 2, 22);
}

void Palette::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setPen(QColor(0, 0, 0));
    int pixelW = (width() - 2) / COLORS;
    int pixelH = height() - 2;
    for (int x = 0 ; x < COLORS ; ++x)
    {
        QBrush brush(rgb[x]);
        painter.setBrush(brush);
        painter.drawRect(QRect(1 + x * pixelW, 1, pixelW, pixelH));
        painter.setBrush(Qt::NoBrush);
    }
}

void Palette::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        int pixelW = (width() - 2) / COLORS;
        int x = (event->x() - 1) / pixelW;
        if (x >= 0 && x < COLORS)
        {
            color = x;
        }
        update();
    }
}
