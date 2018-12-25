#pragma GCC optimize ("-O3")

#include "libgame.h"
#include "graphics.h"
#include "binary.h"
#include "controls.h"

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
    // ШИРИНА, ВЫСОТА, ДАННЫЕ
    13, 10, YourSprite_lines
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

#define PLANES_MAX_COUNT 5
#define DOODLE_WIDTH 4
#define DOODLE_HEIGHT 4
#define PLANE_WIDTH 5
#define PLANE_HEIGHT 2

#define DEBUG_ENABLE true
#define DEBUG(...) do {          \
        if (DEBUG_ENABLE) {      \
            printf(__VA_ARGS__); \
            fflush(stdout);      \
        }                        \
    } while (0)

struct Entity {
    uint8_t x;
    uint32_t y;
    uint8_t w;
    uint8_t mx;
    uint8_t my;
};

struct DoodleJumpData
{
    uint32_t scene_height;
    Entity doodle;
    Entity planes[PLANES_MAX_COUNT];
    uint32_t planes_size;
};
static DoodleJumpData* data;

static void DoodleJump_remove_plank(uint32_t index)
{
    int last = data->planes_size - 1;
    Entity *planes = data->planes;
    Entity *tmp = &planes[index];
    planes[index] = planes[last];
    planes[last] = *tmp;
    data->planes_size--;
}

static void DoodleJump_add_plank(uint8_t x, uint8_t y, uint8_t w)
{
    if (data->planes_size >= PLANES_MAX_COUNT) {
        DEBUG("Add plane failed! (%d:%d) \n", x, y);
        return;
    }

    Entity *plane = &data->planes[data->planes_size];
    data->planes_size++;
    plane->x = x;
    plane->y = y;
    plane->w = w;

    DEBUG("Add plane (%d:%d)\n", plane->x, plane->y);
}

static void DoodleJump_reset()
{
    data->scene_height = 0;
    data->planes_size = 0;

    data->doodle.w = DOODLE_WIDTH;
    data->doodle.y = 50;
    data->doodle.x = (WIDTH - DOODLE_WIDTH) / 2;

    DoodleJump_add_plank(0, 5, WIDTH);
}

static void DoodleJump_render_plane(Entity *obj)
{
    int y = HEIGHT - obj->y - data->scene_height;
    game_draw_rect(obj->x, y, obj->w, PLANE_HEIGHT, WHITE);
}

static void DoodleJump_render_doodle(Entity *obj)
{
    int y = HEIGHT - obj->y - data->scene_height - DOODLE_HEIGHT;
    game_draw_rect(obj->x, y, obj->w, DOODLE_HEIGHT, GREEN);
    DEBUG("%d\n", obj->y);
}

static void DoodleJump_update_doodle(Entity *obj)
{
    obj->x += obj->mx;
    obj->y += obj->my;

    // gravity
    if (obj->my > -1) {
        
    }
    obj->my = -1;
}

static void DoodleJump_prepare()
{
    DoodleJump_reset();
}

static void DoodleJump_render()
{
    Entity *plane;
    for (int i = 0; i < data->planes_size; ++i) {
        DoodleJump_render_plane(&data->planes[i]);
    }

    DoodleJump_render_doodle(&data->doodle);
}

static void DoodleJump_update(unsigned long delta)
{
    DoodleJump_update_doodle(&data->doodle);
}

const game_instance DoodleJump PROGMEM = {
    "DoodleJump",
    DoodleJump_prepare,
    DoodleJump_render,
    DoodleJump_update,
    sizeof(DoodleJumpData),
    (void**)(&data)
};
