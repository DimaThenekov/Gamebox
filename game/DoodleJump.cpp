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
#define PLANE_WIDTH 5
#define PLANE_HEIGHT 2

#define DEBUG_ENABLE true
#define DEBUG(...) do {          \
        if (DEBUG_ENABLE) {      \
            printf(__VA_ARGS__); \
            fflush(stdout);      \
        }                        \
    } while (0)

typedef struct _Plane {
    uint8_t x;
    uint8_t y;
} Plane;

struct DoodleJumpData
{
    uint32_t scene_height;
    uint32_t doodle_x;
    uint32_t doodle_y;
    Plane planes[PLANES_MAX_COUNT];
    uint32_t planes_size;
    /* Объявляйте ваши переменные здесь */
    /* Чтобы потом обращаться к ним, пишите data->ПЕРЕМЕННАЯ */
};
static DoodleJumpData* data;

static void DoodleJump_remove_plank(uint32_t index)
{
    int last = data->planes_size - 1;
    Plane *planes = data->planes;
    planes[index].x = planes[last].x;
    planes[index].y = planes[last].y;
    data->planes_size--;
}

static void DoodleJump_add_plank(uint8_t x, uint8_t y)
{
    if (data->planes_size >= PLANES_MAX_COUNT) {
        DEBUG("Add plane failed! (%d:%d) \n", x, y);
        return;
    }

    Plane *plane = &data->planes[data->planes_size];
    data->planes_size++;
    plane->x = x;
    plane->y = y;

    DEBUG("Add plane (%d:%d)\n", plane->x, plane->y);
}

static void DoodleJump_reset()
{
    Plane *planes = data->planes;

    data->scene_height = 0;
    data->planes_size = 0;

    DoodleJump_add_plank(5, 5);
}

static void DoodleJump_prepare()
{
    DoodleJump_reset();
}

static void DoodleJump_render()
{
    Plane *plane;
    for (int i = 0; i < data->planes_size; ++i) {

        plane = &data->planes[i];
        game_draw_rect(plane->x, plane->y, PLANE_WIDTH, PLANE_HEIGHT, WHITE);
    }

    DEBUG("rand %d\n", rand());
}

static void DoodleJump_update(unsigned long delta)
{
    
}

const game_instance DoodleJump PROGMEM = {
    "DoodleJump",
    DoodleJump_prepare,
    DoodleJump_render,
    DoodleJump_update,
    sizeof(DoodleJumpData),
    (void**)(&data)
};
