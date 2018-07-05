#ifndef ARDUINO

#include "renderarea.h"
#include "window.h"
#include "../controls.h"

#include <QtWidgets>

const int IdRole = Qt::UserRole;

/*  Static functions for Arduino project */

static uint16_t buttons; // buttons currently pressed

static QTimer *timer;

void render();
void update(unsigned long delta);

bool game_is_button_pressed(uint8_t button)
{
    return (buttons >> button) & 1;
}

bool game_is_any_button_pressed(uint16_t bitmask)
{
    return (buttons & bitmask);
}

void game_reset_buttons(uint16_t bitmask)
{
    buttons &= ~bitmask;
}

static void button_update(uint8_t button, bool press)
{
    if (press)
    {
        buttons |= 1 << button;
    }
    else
    {
        buttons &= ~(1 << button);
    }
}

static bool button_process(int key, bool press)
{
    switch (key)
    {
    case Qt::Key_Down:
        button_update(BUTTON_DOWN, press);
        return true;
    case Qt::Key_Up:
        button_update(BUTTON_UP, press);
        return true;
    case Qt::Key_Left:
        button_update(BUTTON_LEFT, press);
        return true;
    case Qt::Key_Right:
        button_update(BUTTON_RIGHT, press);
        return true;
    case Qt::Key_Q:
        button_update(BUTTON_NW, press);
        return true;
    case Qt::Key_A:
        button_update(BUTTON_SW, press);
        return true;
    case Qt::Key_P:
        button_update(BUTTON_NE, press);
        return true;
    case Qt::Key_L:
        button_update(BUTTON_SE, press);
        return true;
    case Qt::Key_X:
        button_update(BUTTON_A, press);
        return true;
    case Qt::Key_Z:
        button_update(BUTTON_B, press);
        return true;
    case Qt::Key_Space:
        button_update(BUTTON_SELECT, press);
        return true;
    case Qt::Key_Return:
        button_update(BUTTON_START, press);
        return true;
    }

    return false;
}

void game_set_ups(int ups)
{
    if (ups)
        timer->setInterval(1000 / ups);
    else
        timer->setInterval(1000 / 30);
}

/*****************************************/

Window::Window()
{
    renderArea = new RenderArea;

    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->addWidget(renderArea);
    setLayout(mainLayout);

    setWindowTitle(tr("Gamebox emulator"));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateScreen()));
    timer->start(1000 / 30);
}

void Window::updateScreen()
{
    ::update(timer->interval());
    renderArea->beginRender();
    ::render();
    renderArea->endRender();
    renderArea->update();
}

void Window::keyPressEvent(QKeyEvent *event)
{
    if (button_process(event->key(), true))
        return;
    if (event->key() == Qt::Key_Escape)
        close();

    QWidget::keyPressEvent(event);
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
    if (button_process(event->key(), false))
        return;
    QWidget::keyReleaseEvent(event);
}

#endif
