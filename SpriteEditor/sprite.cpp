#include <QPainter>
#include <QMouseEvent>
#include "sprite.h"
#include "palette.h"

Sprite::Sprite(Palette *p, QWidget *parent)
    : QWidget(parent), pal(p)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

int Sprite::getPixel(int row, int col)
{
    Q_ASSERT(row >= 0 && row < HEIGHT && col >= 0 && col < WIDTH);

    return pixels[row][col];
}

QSize Sprite::minimumSizeHint() const
{
    return QSize(WIDTH * 10 + 1, HEIGHT * 10 + 1);
}

QSize Sprite::sizeHint() const
{
    return QSize(WIDTH * 20 + 1, HEIGHT * 20 + 1);
}

void Sprite::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setPen(QColor(0, 0, 0));
    int pixelW = width() / WIDTH;
    int pixelH = height() / HEIGHT;
    for (int y = 0 ; y < HEIGHT ; ++y)
    {
        for (int x = 0 ; x < WIDTH ; ++x)
        {
            QBrush brush(getRGB(pixels[y][x]));
            painter.setBrush(brush);
            painter.drawRect(QRect(x * pixelW, y * pixelH, pixelW, pixelH));
            painter.setBrush(Qt::NoBrush);
        }
    }
}

void Sprite::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        int pixelW = width() / WIDTH;
        int pixelH = height() / HEIGHT;
        int x = event->x() / pixelW;
        int y = event->y() / pixelH;
        pixels[y][x] = pal->getColor();
        update();
        emit spriteUpdated();
    }
}
