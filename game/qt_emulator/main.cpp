#ifndef ARDUINO
#include "../libgame.h"
#include "../graphics.h"
//#include "../controls.h"
//#include "../font.h"
//#include "../storage.h"
//#include "../font_data.h"

#include "window.h"

#include <QApplication>
#include <QtWidgets>

/*  Static functions for Arduino project */

void application_setup();

unsigned long millis()
{
    return QDateTime::currentDateTimeUtc().toTime_t();
}

void storage_init() {}
void storage_format() {}

void storage_read(const char *name, void *buffer, size_t size){};
bool storage_write(const char *name, const void *buffer, size_t size){return 0;};

uint8_t game_sprite_width(const struct game_sprite *s)
{
    return s->width;
}

uint8_t game_sprite_height(const struct game_sprite *s)
{
    return s->height;
}

/*****************************************/

int main(int argc, char *argv[])
{
    application_setup();

    QApplication app(argc, argv);
    Window window;
    window.show();
    return app.exec();
}
#endif
