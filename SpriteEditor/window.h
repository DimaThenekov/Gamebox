#ifndef WINDOW_H
#define WINDOW_H

#include <QtWidgets>

class Sprite;

class Window : public QWidget
{
    Q_OBJECT

public:
    Window();

private slots:
    void updateSprite();

private:
    Sprite *sprite;
    QTextEdit *code;
};

#endif // WINDOW_H
