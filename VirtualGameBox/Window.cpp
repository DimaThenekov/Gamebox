#include "Window.h"
#include <QtWidgets>
#include <thread>

RenderArea::RenderArea(Display &d, QWidget *parent)
    : QWidget(parent), display(d)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
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
    QColor *screen = display.getScreen();
    for (int y = 0 ; y < HEIGHT ; ++y)
    {
        for (int x = 0 ; x < WIDTH ; ++x)
        {
            QBrush brush;
            brush = QBrush(screen[y * WIDTH + x]);
            painter.setBrush(brush);
            painter.drawRect(QRect(x * pixelW, y * pixelH, pixelW, pixelH));
            painter.setBrush(Qt::NoBrush);
        }
    }
}


Window::Window(std::vector<uint8_t> &buf)
    : avr(display)
{
    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    renderArea = new RenderArea(display, this);
    mainLayout->addWidget(renderArea);
    setLayout(mainLayout);

    setWindowTitle(tr("VirtualGameBox"));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateScreen()));
    timer->start(1000 / 30);

    avr.flash.fill(0x00);

    for (size_t i = 0; i < buf.size(); i += 2) {
        avr.flash[i >> 1] =
                (((uint8_t)buf[i + 1]) << 8) | (uint8_t)buf[i];
    }

    new std::thread(&AVR::run, &avr);
}

void Window::updateScreen()
{
    renderArea->update();
}

bool Window::button_process(int key, bool press)
{
    switch (key)
    {
    case Qt::Key_Down:
        display.button_update(BUTTON_DOWN, press);
        return true;
    case Qt::Key_Up:
        display.button_update(BUTTON_UP, press);
        return true;
    case Qt::Key_Left:
        display.button_update(BUTTON_LEFT, press);
        return true;
    case Qt::Key_Right:
        display.button_update(BUTTON_RIGHT, press);
        return true;
    case Qt::Key_Q:
        display.button_update(BUTTON_NW, press);
        return true;
    case Qt::Key_A:
        display.button_update(BUTTON_SW, press);
        return true;
    case Qt::Key_P:
        display.button_update(BUTTON_NE, press);
        return true;
    case Qt::Key_L:
        display.button_update(BUTTON_SE, press);
        return true;
    case Qt::Key_X:
        display.button_update(BUTTON_A, press);
        return true;
    case Qt::Key_Z:
        display.button_update(BUTTON_B, press);
        return true;
    case Qt::Key_Space:
        display.button_update(BUTTON_SELECT, press);
        return true;
    case Qt::Key_Return:
        display.button_update(BUTTON_START, press);
        return true;
    }

    return false;
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

