#ifndef ARDUINO
#include "renderarea.h"

#include "../libgame.h"
#include "../graphics.h"
#include "../font.h"
#include "../font_data.h"

#include <QPainter>

/* static functions and data for Arduino project compatibility */
static QColor screen[HEIGHT][WIDTH];

void game_draw_pixel(int x, int y, uint8_t c)
{
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return;
    QColor color(((c >> 4) & 3) * 85, ((c >> 2) & 3) * 85, (c & 3) * 85);
    screen[y][x] = color;
}

void game_draw_vline(int x, int y1, int y2, uint8_t color)
{
    for (int i = y1 ; i <= y2 ; ++i)
        game_draw_pixel(x, i, color);
}

void game_draw_sprite(const struct game_sprite *s, int x, int y, uint8_t color)
{
    for (int dx = 0; dx < s->width; ++dx)
    {
        for (int dy = 0; dy < s->height; ++dy)
        {
            int xx = x + dx;
            int yy = y + dy;
            if (xx < 0 || xx >= WIDTH || yy < 0 || yy >= HEIGHT)
                continue;
            int byte = s->lineSize * dy + dx / 8;
            int bit = 7 - dx % 8;
            if ((s->lines[byte] >> bit) & 1)
                game_draw_pixel(xx, yy, color);
        }
    }
}

void game_draw_char(uint8_t c, int x, int y, uint8_t color, uint8_t bg)
{
    int pos = (int)c * FONT_HEIGHT;
    for (int i = 0; i < FONT_WIDTH; ++i)
    {
        for (int j = 0; j < FONT_HEIGHT; ++j)
        {
            uint8_t d = font_data[pos + j];
            if ((d >> (FONT_WIDTH - 1 - i)) & 1)
                game_draw_pixel(x + i, y + j, color);
            else if (bg != OPAQUE)
                game_draw_pixel(x + i, y + j, bg);
        }
    }
}

void game_draw_text(const uint8_t *s, int x, int y, uint8_t color, uint8_t bg)
{
    int xx = x;
    int yy = y;
    for (const uint8_t *c = s; *c; ++c)
    {
        game_draw_char(*c, xx, yy, color, bg);
        xx += FONT_WIDTH + 1;
    }
}

void game_draw_digits(uint16_t num, int len, int x, int y, uint8_t color)
{
    x += (len - 1) * (DIGIT_WIDTH + 1);
    for (int i = len - 1 ; i >= 0 ; --i, x -= DIGIT_WIDTH + 1)
    {
        uint8_t d = num % 10;
        num /= 10;
        for (int8_t dy = 0 ; dy < DIGIT_HEIGHT ; ++dy)
        {
            uint8_t dd = digits_data[d * DIGIT_HEIGHT + dy];
            for (int b = 0 ; b < DIGIT_WIDTH ; ++b)
            {
                if ((dd >> (DIGIT_WIDTH - 1 - b)) & 1)
                    game_draw_pixel(x + b, y + dy, color);
            }
        }
    }
}

bool game_is_drawing_lines(int y, int height)
{
    return true;
}

/**************************************************************/

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
}

void RenderArea::clear()
{
    for (int y = 0 ; y < HEIGHT ; ++y)
    {
        for (int x = 0 ; x < WIDTH ; ++x)
        {
            screen[y][x] = QColor(0, 0, 0);
        }
    }
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(WIDTH * 4 + 1, HEIGHT * 4 + 1);
}

QSize RenderArea::sizeHint() const
{
    return QSize(WIDTH * 10 + 1, HEIGHT * 10 + 1);
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setPen(QColor(0, 0, 0));
    int pixelW = width() / WIDTH;
    int pixelH = height() / HEIGHT;
    for (int y = 0 ; y < HEIGHT ; ++y)
    {
        for (int x = 0 ; x < WIDTH ; ++x)
        {
            QBrush brush(screen[y][x]);
            painter.setBrush(brush);
            painter.drawRect(QRect(x * pixelW, y * pixelH, pixelW, pixelH));
            painter.setBrush(Qt::NoBrush);
        }
    }
}
#endif
