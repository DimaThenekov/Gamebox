#include "window.h"
#include "sprite.h"
#include "palette.h"

Window::Window()
{
    palette = new Palette;
    sprite = new Sprite(palette);
    code = new QTextEdit;

    code->setReadOnly(true);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(palette);
    mainLayout->addSpacing(1);
    mainLayout->addWidget(sprite);
    mainLayout->addSpacing(1);
    mainLayout->addWidget(code);
    setLayout(mainLayout);

    setWindowTitle(tr("Gamebox sprite editor"));

    connect(sprite, SIGNAL(spriteUpdated()), this, SLOT(updateSprite()));
}

void Window::updateSprite()
{
    static int palette[COLORS] = {0, 1};

    QString s;
    int left = WIDTH, right = 0, top = HEIGHT, bottom = 0;
    for (int r = 0 ; r < HEIGHT ; ++r)
    {
        for (int c = 0 ; c < WIDTH ; ++c)
        {
            if (sprite->getPixel(r, c))
            {
                if (left > c)
                    left = c;
                if (right < c)
                    right = c;
                if (top > r)
                    top = r;
                if (bottom < r)
                    bottom = r;
            }
        }
    }
    if (left > right || top > bottom)
    {
        code->setPlainText("");
    }
    else
    {
        s = "const uint8_t spriteLines[] PROGMEM = {\n";
        for (int r = top ; r <= bottom ; ++r)
        {
            s += "    ";
            QString bytes;
            for (int c = left ; c <= right ; ++c)
            {
                int p = sprite->getPixel(r, c);
                bytes += p ? "1" : "0";
                if (bytes.length() == 8)
                {
                    s += "B" + bytes + ", ";
                    bytes.clear();
                }
            }
            if (bytes.length())
            {
                while (bytes.length() < 8)
                    bytes += "0";
                s += "B" + bytes + ", ";
            }
            s += "\n";
        }
        s += "};\n";
        s += QString("const game_sprite sprite PROGMEM = {%1, %2, spriteLines};\n")
            .arg(right - left + 1).arg(bottom - top + 1);
        code->setPlainText(s);
    }
}
