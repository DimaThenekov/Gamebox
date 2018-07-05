#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QWidget>

class RenderArea : public QWidget
{
    Q_OBJECT

public:
    RenderArea(QWidget *parent = 0);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

    void beginRender();
    void endRender();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool framebuffer;
};

#endif // RENDERAREA_H
