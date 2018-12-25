#ifndef SPRITE_H
#define SPRITE_H

#include <QWidget>

#define WIDTH 64
#define HEIGHT 16

class Sprite : public QWidget
{
    Q_OBJECT

public:
    Sprite(QWidget *parent = 0);

    int getPixel(int row, int col);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

signals:
    void spriteUpdated();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int pixels[HEIGHT][WIDTH];
};

#endif
