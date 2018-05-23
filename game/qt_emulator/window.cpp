#ifndef ARDUINO

#include "renderarea.h"
#include "window.h"

#include <QtWidgets>

const int IdRole = Qt::UserRole;

Window::Window()
{
    renderArea = new RenderArea;

    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    mainLayout->addWidget(renderArea);
    setLayout(mainLayout);

    setWindowTitle(tr("Gamebox emulator"));
}

#endif
