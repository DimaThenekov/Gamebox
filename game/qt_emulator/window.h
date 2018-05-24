#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

class RenderArea;

class Window : public QWidget
{
    Q_OBJECT

public:
    Window();

private slots:
    void updateScreen();

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;

private:
    RenderArea *renderArea;
};

#endif // WINDOW_H
