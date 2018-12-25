#include <QPainter>
#include <QMouseEvent>
#include "sprite.h"

/*static int color_map[4] = {0, 128, 192, 255};

static int rev_color_map(int c)
{
    switch (c)
    {
    case 128:
        return 1;
    case 192:
        return 2;
    case 255:
        return 3;
    }
    return 0;
}

static QColor map_color(uint8_t c)
{
    int r = c & RED;
    r = (r & 0xf) + ((r & 0xf0) >> 3);
    int g = c & GREEN;
    g = ((g & 0xf) >> 1) + ((g & 0xf0) >> 4);
    int b = c & BLUE;
    b = ((b & 0xf) >> 2) + ((b & 0xf0) >> 5);
    QColor color(color_map[r], color_map[g], color_map[b]);
    return color;
}*/

Sprite::Sprite(QWidget *parent)
    : QWidget(parent)
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
            QBrush brush(pixels[y][x] ? QColor(255, 255, 255) : QColor(0, 0, 0));
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
        pixels[y][x] = 1;
        update();
        emit spriteUpdated();
    }
}
