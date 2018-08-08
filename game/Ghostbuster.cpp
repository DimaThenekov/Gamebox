#pragma GCC optimize ("-O3")

#include "libgame.h"
#include "graphics.h"
#include "binary.h"
#include "controls.h"
#define B BLUE
#define P GREEN
#define S RED
#define C CYAN
#define P PURPLE
#define W WHITE
#define O TRANSPARENT
#define Y YELLOW
#define L BLACK
/* Встроенные цвета:
 *
 *  BLACK - Чёрный
 *  BLUE - Синий
 *  GREEN - Зелёный
 *  RED - Красный
 *  CYAN - Циановый
 *  PURPLE - Фиолетовый
 *  BROWN - Коричневый
 *  WHITE - Белый
 *
 *  Для использования 64-х цветной палитры, укажите в game.ino COLOR_6BIT = 1
 *
 * */

/* Кнопки:
 *
 * НА КОРПУСЕ:
 * BUTTON_SW, BUTTON_NW, BUTTON_SE, BUTTON_NE
 *
 * NW              NE
 *  SW            SE
 *
 *
 * НА ДЖОЙСТИКЕ:
 * BUTTON_UP, BUTTON_LEFT, BUTTON_RIGHT, BUTTON_DOWN, BUTTON_SELECT, BUTTON_START, BUTTON_A, BUTTON_B
 *
 *      UP
 *  LEFT+RIGHT     SELECT  START      B  A
 *     DOWN
 *
 * */

/* Спрайты
 *
 * максимальная высота - 16 пикселей

 определение спрайта


   x     x
    x   x
     x x
  xxxxxxxxx
 xxxxxxxxxxx
xxx  xxx  xxx
 xxxxxxxxxxx
  xxxxxxxxx
    x x x
   x     x


 --------------------------------

const uint8_t YourSprite_lines[] PROGMEM = {
    B00010000, B01000000,
    B00001000, B10000000,
    B00000101, B00000000,
    B00111111, B11100000,
    B01111111, B11110000,
    B11100111, B00111000,
    B01111111, B11110000,
    B00111111, B11100000,
    B00001010, B10000000,
    B00010000, B01000000
};

const game_sprite YourSprite PROGMEM = {
    // ШИРИНА, ВЫСОТА, КОЛИЧЕСТВО БАЙТ НА СТРОКУ, ДАННЫЕ
    13, 10, 2, YourSprite_lines
};

*/

/* Функции отрисовки
 *
 * game_draw_pixel(x, y, color) - Красит точку (x, y) в цвет color
 * game_draw_char(char, x, y, color) - Выводит символ char с левым верхним углом в точке (x, y) с цветом color
 * game_draw_text(string, x, y, color) - Выводит строку string с левым верхним углом в точке (x, y) с цветом color
 * game_draw_sprite(sprite, x, y, color) - Выводит спрайт sprite с левым верхним углом в точке (x, y) с цветом color
 *
 * */

/* Функции ввода
 *
 * game_is_button_pressed(button) - Нажата ли кнопка? Например: game_is_button_pressed(BUTTON_START)
 * game_is_any_button_pressed(mask) - Нажата ли хотя бы одна кнопка? Например: game_is_any_button_pressed(BITMASK(BUTTON_SW) | BITMASK(BUTTON_DOWN))
 *
 * */
 const uint8_t player_lines[] PROGMEM = {
   O, O, O, O, G, G, G, G, G, G, O, O, O,
   O, O, O, G, G, G, G, G, G, G, G, G, O,
   O, G, G, G, Y, Y, Y, Y, L, Y, G, O, O,
   G, Y, G, Y, Y, Y, Y, Y, L, Y, Y, Y, O,
   G, Y, G, G, Y, Y, Y, Y, Y, Y, Y, Y, Y,
   O, L, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, O,
   L, L, W, W, W, W, W, W, W, O, O, O, O,
   L, W, W, W, W, S, S, W, W, W, W, O, O,
   W, W, W, W, S, S, S, S, W, W, W, W, O,
   Y, L, W, W, S, S, S, S, W, W, L, Y, O,
   Y, L, L, L, L, L, L, L, L, L, L, L, L,
   Y, Y, L, L, L, L, L, L, L, L, L, L, L,
   O, O, W, W, W, O, O, W, W, W, L, O, O,
   O, G, G, G, O, O, O, O, G, G, G, O, O,
   G, G, G, G, O, O, O, O, G, G, G, G, O
 };
struct GhostbusterData
{
   int8_t pposy;
    /* Объявляйте ваши переменные здесь */
    /* Чтобы потом обращаться к ним, пишите data->ПЕРЕМЕННАЯ */
};
static GhostbusterData* data; /* Эта переменная - указатель на структуру, которая содержит ваши переменные */

static void Ghostbuster_prepare()
{
    /* Здесь код, который будет исполнятся один раз */
    /* Здесь нужно инициализировать переменные */
}

static void Ghostbuster_render()
{
    /* Здесь код, который будет вывзваться для отрисовки кадра */
    /* Он не должен менять состояние игры, для этого есть функция update */
    game_draw_color_sprite(&snail, 10, data->pposy);
    /* Здесь (и только здесь) нужно вызывать функции game_draw_??? */
}

static void Ghostbuster_update(unsigned long delta)
{
  if(game_is_button_pressed(BUTTON_LEFT))
    date->pposy--;
    /* Здесь код, который будет выполняться в цикле */
    /* Переменная delta содержит количество миллисекунд с последнего вызова */

    /* Здесь можно работать с кнопками и обновлять переменные */
}

game_instance Ghostbuster = {
    "Ghostbuster",         /* Имя, отображаемое в меню */
    Ghostbuster_prepare,
    Ghostbuster_render,
    Ghostbuster_update,
    sizeof(GhostbusterData),
    (void**)(&data)
};


/* Не забудьте зарегистрировать игру в application.cpp */
