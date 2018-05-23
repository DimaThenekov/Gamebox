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

void render();
void update(unsigned long delta);
void prepare();

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

#if 0

void clear_screen()
{
    for (int x = 0; x < HEIGHT; ++x)
    {
        for (int y = 0; y < WIDTH; ++y)
        {
            game_draw_pixel(x, y, BLACK);
        }
    }
}

bool game_is_button_pressed(uint8_t button)
{
    return (buttons >> button) & 1;
}

bool game_is_any_button_pressed(uint16_t bitmask)
{
    return buttons & bitmask;
}

void set_pressed(uint8_t button, bool pressed)
{
    if (pressed)
    {
        buttons |= (1 << button);
    }
    else
    {
        buttons &= ~(1 << button);
    }
}

void game_set_ups(int ups)
{
/*    if (ups)
        window->setFramerateLimit(ups);
    else
        window->setFramerateLimit(30);*/
}


#endif

int main(int argc, char *argv[])
{
    //game_setup();
    /*
    running_clock.restart();
    sf::Clock elapsed;
    while (window->isOpen())
    {
        elapsed.restart();
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window->close();
            }
            if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
            {
                bool status = event.type == sf::Event::KeyPressed;
                if (event.key.code == sf::Keyboard::Q) set_pressed(BUTTON_NW, status);
                if (event.key.code == sf::Keyboard::A) set_pressed(BUTTON_SW, status);
                if (event.key.code == sf::Keyboard::P) set_pressed(BUTTON_NE, status);
                if (event.key.code == sf::Keyboard::L) set_pressed(BUTTON_SE, status);

                if (event.key.code == sf::Keyboard::Left) set_pressed(BUTTON_LEFT, status);
                if (event.key.code == sf::Keyboard::Right) set_pressed(BUTTON_RIGHT, status);
                if (event.key.code == sf::Keyboard::Up) set_pressed(BUTTON_UP, status);
                if (event.key.code == sf::Keyboard::Down) set_pressed(BUTTON_DOWN, status);
                if (event.key.code == sf::Keyboard::X) set_pressed(BUTTON_A, status);
                if (event.key.code == sf::Keyboard::Z) set_pressed(BUTTON_B, status);
                if (event.key.code == sf::Keyboard::Comma) set_pressed(BUTTON_SELECT, status);
                if (event.key.code == sf::Keyboard::Period) set_pressed(BUTTON_START, status);
                
                if (status && event.key.code == sf::Keyboard::Space) paused = !paused;
                if (status && event.key.code == sf::Keyboard::Escape) window->close();
            }
        }

        window->clear(sf::Color::Black);
        window->draw(*screen);
        window->display();

        if (!paused)
        {
            update(elapsed.getElapsedTime().asMilliseconds());
            clear_screen();
            render();
        }
    }
*/
    QApplication app(argc, argv);
    Window window;
    window.show();
    return app.exec();
}
#endif
