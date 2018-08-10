#ifndef WINDOW_H
#define WINDOW_H

#include "periph/Display.h"
#include "AVR.h"
#include <QWidget>

class RenderArea : public QWidget
{
    Q_OBJECT

public:
    RenderArea(Display &d, QWidget *parent = 0);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Display &display;
};

class Window : public QWidget
{
    Q_OBJECT

public:
    Window(std::vector<uint8_t> &buf);
    virtual ~Window();

private slots:
    void updateScreen();

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;

private:
    bool button_process(int key, bool press);

private:
    RenderArea *renderArea;
    Display display;
    AVR avr;
};

#endif // WINDOW_H
