#pragma GCC optimize ("-O3")

#include "libgame.h"
#include "graphics.h"
#include "binary.h"
#include "controls.h"
#define B BLUE
#define G GREEN
#define R RED
#define S CYAN
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
   O, O, O, O, P, P, P, P, P, P, O, O, O, 
   O, O, O, P, P, P, P, P, P, P, P, P, O, 
   O, P, P, Y, Y, Y, Y, Y, L, Y, P, O, O, 
   P, Y, P, Y, Y, Y, Y, Y, L, Y, Y, Y, O, 
   P, Y, P, P, Y, Y, Y, Y, Y, Y, Y, Y, Y,  
   O, B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, O, 
   B, B, S, S, W, S, S, S, S, O, O, O, O, 
   B, S, S, S, W, R, R, W, S, S, S, O, O, 
   S, S, S, S, R, W, O, R, S, S, S, S, O, 
   Y, B, S, W, R, O, W, R, W, S, B, Y, O, 
   Y, B, B, B, B, B, B, B, B, B, B, B, B, 
   Y, Y, B, B, B, B, B, B, B, B, B, B, B, 
   O, O, W, W, W, O, O, W, W, W, B, O, O, 
   O, P, P, P, O, O, O, O, P, P, P, O, O, 
   P, P, P, P, O, O, O, O, P, P, P, P, O 
 };
  
  
 const game_color_sprite player PROGMEM = {
    13,15,player_lines
 };

 const uint8_t player_left_lines[] PROGMEM = {

    0, 0, 0, P, P, P, P, P, P, 0, 0, 0, 0,
    0, P, P, P, P, P, P, P, P, P, 0, 0, 0,
    0,0 ,P ,Y ,L ,Y ,Y ,Y ,Y ,Y ,P ,P , 0,
    0, Y, Y, Y, L, Y, Y, Y, Y, Y, P, Y, P,
    Y, Y, Y, Y, Y, Y, Y, Y, Y, P, P, Y, P,
    0, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B, 0,
    0, 0, 0, 0, S, S, S, S, W, S, S, B, B,
    0, 0, S, S, S, W, R, R, W, S, S, S, B,
    0, S, S, S, S, R, 0, W, R, S, S, S, S,
    0, Y, B, S, W, R, W, 0, R, W, S, B, Y,
    B, B, B, B, B, B, B, B, B, B, B, B, Y,
    B, B, B, B, B, B, B, B, B, B, B, Y, Y,
    0, 0, B, W, W, W, 0, 0, W, W, W, 0, 0,
    0, 0, P, P, P, 0, 0, 0, 0, P, P, P, 0,
    0, P, P, P, P, 0, 0, 0, 0, P, P, P, P
  
 };

 const game_color_sprite player_left PROGMEM = {
  13,15,player_left_lines
 };

 const uint8_t slimer_lines[] PROGMEM = {
    0,0,G,G,G,0,0,
    0,G,G,G,G,G,0,
    G,G,0,G,0,G,G,
    G,G,G,G,G,G,G,
    G,W,W,W,W,G,G,
    G,R,R,R,R,G,G,
    G,W,R,R,W,G,G,
    G,G,R,R,G,G,G,
    0,G,G,G,G,G,0,
    0,0,0,0,G,G,0,
    0,0,0,0,0,G,G,
    0,0,0,0,0,0,G
 };

 const game_color_sprite slimer PROGMEM = {

    7,12,slimer_lines

 };

 const uint8_t shoot_lines[] PROGMEM = {

    S,R,
    R,S
  
 };

 const game_color_sprite shoot PROGMEM = {

    2,2,shoot_lines
  
 };
 
struct GhostbusterData
{
   int8_t pposy, ShootCounter;
   int8_t sposy,ShootPosy[5];
   bool flag,isShoot;
    /* Объявляйте ваши переменные здесь */
    /* Чтобы потом обращаться к ним, пишите data->ПЕРЕМЕННАЯ */
};
static GhostbusterData* data; /* Эта переменная - указатель на структуру, которая содержит ваши переменные */

static void Ghostbuster_prepare()
{
    /* Здесь код, который будет исполнятся один раз */
    /* Здесь нужно инициализировать переменные */
    data->flag = true;
    data->isShoot = false;
    data->ShootCounter = 0;
}

static void Ghostbuster_render()
{
    /* Здесь код, который будет вывзваться для отрисовки кадра */
    /* Он не должен менять состояние игры, для этого есть функция update */
    game_draw_color_sprite(&slimer, data->sposy, 52);
    if(data->flag) game_draw_color_sprite(&player, data->pposy, 49);
    else game_draw_color_sprite(&player_left, data->pposy, 49);
    if( (data->isShoot) && (data->ShootCounter<6) ) {
      for(int i = 0; i <= data->ShootCounter*4; i++) 
        if(i%4 == 0)
          game_draw_color_sprite(&shoot, data->ShootPosy[i/4], 59);
    }
    else if(data->isShoot) 
      for(int i = 0; i <= 6; i++) {
        game_draw_color_sprite(&shoot, data->ShootPosy[i], 59);
    }
      
    /* Здесь (и только здесь) нужно вызывать функции game_draw_??? */
}

static void Ghostbuster_update(unsigned long delta)
{
  if( (game_is_button_pressed(BUTTON_A)) && (data->flag) ) {

    data->isShoot = true;
    for(int i = 0; i < 6; i++) {
      data->ShootPosy[i] = data->pposy + 13 + i*2;
    }
    data->ShootCounter++;
    
  }
  else if( (game_is_button_pressed(BUTTON_A)) && (!data->flag) ) {
    data->isShoot = true;
    for(int i = 0; i < 6; i++) {
      data->ShootPosy[i] = data->pposy - i;
    }

    data->ShootCounter++;
    
  }
  else { data->isShoot = false; data->ShootCounter = 0; }
  
  if( (game_is_button_pressed(BUTTON_LEFT)) && (!data->isShoot) ) {
    data->pposy--;
    data->flag = false;
  }
  else if (game_is_button_pressed(BUTTON_LEFT)) data->flag = false;
   if( (game_is_button_pressed(BUTTON_RIGHT)) && (!data->isShoot) ) {
      data->pposy++;
      data->flag = true;
   }

   
   
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
