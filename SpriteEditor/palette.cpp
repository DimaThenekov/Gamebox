#include "palette.h"

int gameboxPalette[COLORS] = {0, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};

QColor getRGB(int col)
{
    int c = gameboxPalette[col];
    int r = (c & 0x11) << 3;
    int g = (c & 0x22) << 2;
    int b = (c & 0x44) << 1;
    return QColor(r, g, b);
}

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
        QBrush brush(getRGB(x));
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
