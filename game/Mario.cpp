#pragma GCC optimize ("-O3")

#include "libgame.h"
#include "graphics.h"
#include "binary.h"
#include "controls.h"
#include "music.h"
/* Встроенные цвета:

    BLACK - Чёрный
    BLUE - Синий
    GREEN - Зелёный
    RED - Красный
    CYAN - Циановый
    PURPLE - Фиолетовый
    BROWN - Коричневый
    WHITE - Белый

    Для использования 64-х цветной палитры, укажите в game.ino COLOR_6BIT = 1

 * */

/* Кнопки:

   НА КОРПУСЕ:
   BUTTON_SW, BUTTON_NW, BUTTON_SE, BUTTON_NE

   NW              NE
    SW            SE


   НА ДЖОЙСТИКЕ:
   BUTTON_UP, BUTTON_LEFT, BUTTON_RIGHT, BUTTON_DOWN, BUTTON_SELECT, BUTTON_START, BUTTON_A, BUTTON_B

        UP
    LEFT+RIGHT     SELECT  START      B  A
       DOWN

 * */

/* Спрайты

   максимальная высота - 16 пикселей

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
const uint8_t MarioRed_lines[] PROGMEM = {
  B00011111, B00000000,
  B00111111, B11100000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00001000, B00000000,
  B00001001, B00000000,
  B00001111, B00000000,
  B00010110, B10000000,
  B00011111, B10000000,
  B00111111, B11000000,
  B00111001, B11000000,
  B00000000, B00000000,
  B00000000, B00000000
};

const game_sprite MarioRed PROGMEM = {
  // ШИРИНА, ВЫСОТА, КОЛИЧЕСТВО БАЙТ НА СТРОКУ, ДАННЫЕ
  16, 16, 2, MarioRed_lines
};

const uint8_t MarioYellow_lines[] PROGMEM = {
  B00000000, B00000000,
  B00000000, B00000000,
  B00000110, B10000000,
  B00101110, B11100000,
  B00100111, B01110000,
  B00011110, B00000000,
  B00011111, B11000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B11001001, B00110000,
  B11100000, B01110000,
  B11000000, B00110000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000
};

const game_sprite MarioYellow PROGMEM = {
  // ШИРИНА, ВЫСОТА, КОЛИЧЕСТВО БАЙТ НА СТРОКУ, ДАННЫЕ
  16, 16, 2, MarioYellow_lines
};

const uint8_t MarioGreen_lines[] PROGMEM = {
  B00000000, B00000000,
  B00000000, B00000000,
  B00111001, B00000000,
  B01010001, B00000000,
  B01011000, B10000000,
  B01100001, B11100000,
  B00000000, B00000000,
  B00110111, B00000000,
  B01110110, B11100000,
  B11110000, B11110000,
  B00100000, B01000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B00000000, B00000000,
  B01110001, B11000000,
  B11110001, B11100000
};

const game_sprite MarioGreen PROGMEM = {
  // ШИРИНА, ВЫСОТА, КОЛИЧЕСТВО БАЙТ НА СТРОКУ, ДАННЫЕ
  16, 16, 2, MarioGreen_lines
};
const uint8_t StoneGreen_lines[] PROGMEM = {
  B10000000, B00100001,
  B01111111, B10011110,
  B01111111, B10011110,
  B01111111, B10011110,
  B01111111, B10001110,
  B01111111, B10100001,
  B01111111, B10000000,
  B01111111, B10011110,
  B01111111, B10011110,
  B00111111, B00011110,
  B00001111, B00111110,
  B00000000, B00111110,
  B01000000, B01111110,
  B01110000, B01111110,
  B01111110, B01111100,
  B10000001, B00000001
};

const game_sprite StoneGreen PROGMEM = {
  // ШИРИНА, ВЫСОТА, КОЛИЧЕСТВО БАЙТ НА СТРОКУ, ДАННЫЕ
  16, 16, 2, StoneGreen_lines
};
const uint8_t StoneWhite_lines[] PROGMEM = {
  B01111111, B10011110,
  B10000000, B00100000,
  B10000000, B00100000,
  B10000000, B00100000,
  B10000000, B00100000,
  B10000000, B00000000,
  B10000000, B00111110,
  B10000000, B00100000,
  B10000000, B00100000,
  B10000000, B00100000,
  B00000000, B01000000,
  B11000000, B01000000,
  B10110000, B10000000,
  B10001110, B10000000,
  B10000000, B10000000,
  B00000000, B10000000
};

const game_sprite StoneWhite PROGMEM = {
  // ШИРИНА, ВЫСОТА, КОЛИЧЕСТВО БАЙТ НА СТРОКУ, ДАННЫЕ
  16, 16, 2, StoneWhite_lines
};
/* Функции отрисовки

   game_draw_pixel(x, y, color) - Красит точку (x, y) в цвет color
   game_draw_char(char, x, y, color) - Выводит символ char с левым верхним углом в точке (x, y) с цветом color
   game_draw_text(string, x, y, color) - Выводит строку string с левым верхним углом в точке (x, y) с цветом color
   game_draw_sprite(sprite, x, y, color) - Выводит спрайт sprite с левым верхним углом в точке (x, y) с цветом color

 * */

/* Функции ввода

   game_is_button_pressed(button) - Нажата ли кнопка? Например: game_is_button_pressed(BUTTON_START)
   game_is_any_button_pressed(mask) - Нажата ли хотя бы одна кнопка? Например: game_is_any_button_pressed(BITMASK(BUTTON_SW) | BITMASK(BUTTON_DOWN))

 * */

struct MarioData
{
  int MarioX = 0;
  int MarioY = 0;
  int ButtonLeft = 0;
  int ButtonRight = 0;
  int ButtonDown = 0;
  int ButtonUp = 0;
  int MapX = 0;
  int MapY = 0;
  int SetUpMapY = 0;
  int i = 0;
  int Jamp = 0;
  int j = 0;
  
  int Map[100][10];
  //int Map[5][5] = {{ 1, 1, 1, 1, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 0, 0, 0, 0, 1 }, { 1, 1, 1, 1, 1 }};
  /* Объявляйте ваши переменные здесь */
  /* Чтобы потом обращаться к ним, пишите data->ПЕРЕМЕННАЯ */
};
static MarioData* data; /* Эта переменная - указатель на структуру, которая содержит ваши переменные */
//int Map[5][4] = {{ 1, 1, 1, 1 }, { 0, 0, 0, 1 }, { 0, 0, 0, 1 }, { 0, 0, 0, 1 }, { 1, 1, 1, 1 }};
/*Карта не может работать через data->*/


static void Mario_prepare()
{game_set_ups(60);
  /* Здесь код, который будет исполнятся один раз */
  /* Здесь нужно инициализировать переменные */
  data->MapX = 3;
  data->MapY = -64;
  data->MarioX = 30;
  data->MarioY = 32;
  //data->Map[2][2]=1;
  int MapReal[20][8] = {
    {1, 1, 1, 1, 1, 1, 1, 1, }, 
    {0, 0, 0, 0, 0, 0, 0, 1, }, 
    {0, 0, 0, 0, 0, 0, 0, 1, }, 
    {0, 0, 0, 0, 0, 0, 1, 1, }, 
    {0, 0, 0, 0, 0, 1, 1, 1, },
    {0, 0, 0, 0, 0, 0, 0, 0, }, 
    {0, 0, 0, 0, 0, 1, 1, 1, }, 
    {0, 0, 0, 0, 0, 0, 1, 1, }, 
    {0, 0, 0, 0, 0, 0, 0, 1, }, 
    {0, 0, 0, 0, 0, 0, 0, 1, },
    {0, 0, 0, 0, 0, 0, 0, 1, }, 
    {0, 0, 0, 0, 0, 1, 0, 1, }, 
    {0, 0, 0, 0, 1, 1, 0, 1, }, 
    {0, 0, 0, 1, 1, 1, 0, 1, }, 
    {0, 0, 1, 1, 1, 1, 0, 1, },
    {0, 0, 0, 0, 0, 0, 0, 0, }, 
    {0, 0, 0, 0, 0, 0, 0, 1, }, 
    {0, 0, 0, 0, 0, 0, 0, 1, }, 
    {1, 1, 1, 1, 1, 1, 1, 1, }, 
    {1, 1, 1, 1, 1, 1, 1, 1, }};
  for (int i = 0; i <= 19; i++) 
    for (int j = 0; j <= 7; j++)
    {
      data->Map[i][j]=MapReal[i][j];
      
      }
      data->SetUpMapY=data->MapY;
}



static void Mario_render()
{
  /* Здесь код, который будет вывзваться для отрисовки кадра */
  /* Он не должен менять состояние игры, для этого есть функция update */
  



  if (data->MarioY>0){
  game_draw_sprite(&MarioRed, data->MarioX, data->MarioY, RED);
  game_draw_sprite(&MarioYellow, data->MarioX, data->MarioY, YELLOW);
  game_draw_sprite(&MarioGreen , data->MarioX, data->MarioY, GREEN);
  }else{
  game_draw_sprite(&MarioRed, data->MarioX, 0, RED);
  game_draw_sprite(&MarioYellow, data->MarioX, 0, YELLOW);
  game_draw_sprite(&MarioGreen , data->MarioX, 0, GREEN); 
    }
  //game_draw_sprite(&StoneGreen, data->MapX, 48, RED);
  //game_draw_sprite(&StoneWhite, data->MapX, 48, WHITE);
  
  for (int i =max(((0-data->MapX)/16),0) ; i <= ((0-data->MapX)/16)+4; i++) 
    for (int j = 0; j <= 8; j++)
    if (data->Map[i][j] == 1){
    game_draw_sprite(&StoneGreen, data->MapX+(i*16), data->MapY+(j*16), RED);
    game_draw_sprite(&StoneWhite, data->MapX+(i*16), data->MapY+(j*16), WHITE);
    }
          
  /* Здесь (и только здесь) нужно вызывать функции game_draw_??? */
  /*game_draw_sprite(YourSprite, 0, 0, RED);*/
}

static void Mario_update(unsigned long delta)
{
  if (game_is_button_pressed (BUTTON_LEFT)) {
    data->ButtonLeft = 1;
  } else {
    data->ButtonLeft = 0;
  }
  if (game_is_button_pressed (BUTTON_RIGHT)) {
    data->ButtonRight = 1;
  } else {
    data->ButtonRight = 0;
  }
  //BUTTON_UP, BUTTON_LEFT, BUTTON_RIGHT, BUTTON_DOWN, BUTTON_SELECT, BUTTON_START, BUTTON_A, BUTTON_B
    if (game_is_button_pressed (BUTTON_A)) {
    data->ButtonUp = 1;
  } else {
    data->ButtonUp = 0;
  }
  if (game_is_button_pressed (BUTTON_DOWN)) {
    data->ButtonDown = 1;
  } else {
    data->ButtonDown = 0;
  }
  data->Jamp= data->Jamp - ((delta / 10) );
  if ((data->Map[(((0-data->MapX)+data->MarioX+11) / 16 )][((data->MarioY+16-data->MapY)/16)]!=0)||(data->Map[(((0-data->MapX)+data->MarioX) / 16 )][((data->MarioY+16-data->MapY)/16)]!=0)){
  data->Jamp=0;
  }
  
  if ((data->Map[(((0-data->MapX)+data->MarioX+11) / 16 )][((data->MarioY+16-data->MapY)/16)]!=0)||(data->Map[(((0-data->MapX)+data->MarioX) / 16 )][((data->MarioY+16-data->MapY)/16)]!=0))
if((data->Jamp == 0)&&(data->ButtonUp == 1)){
  data->Jamp=8;
 Player_setup_random_melody();
  }
  
   data->MarioY = data->MarioY - data->Jamp;
 if ((data->Map[(((0-data->MapX)+data->MarioX+11) / 16 )][((data->MarioY-data->MapY)/16)]!=0)||(data->Map[(((0-data->MapX)+data->MarioX) / 16 )][((data->MarioY-data->MapY)/16)]!=0)){
  data->MarioY = data->MarioY + data->Jamp;
  }

while ((data->Map[(((0-data->MapX)+data->MarioX+11) / 16 )][((data->MarioY+15-data->MapY)/16)]!=0)||(data->Map[(((0-data->MapX)+data->MarioX) / 16 )][((data->MarioY+15-data->MapY)/16)]!=0)){
  data->MarioY = data->MarioY -1;
  }




  
  data->MapX = data->MapX + ((delta / 10) * data->ButtonLeft);
  data->MapX = data->MapX - ((delta / 10) * data->ButtonRight);
  if ((((0-data->MapX)+data->MarioX) / 16 )>-1)
if ((data->Map[(((0-data->MapX)+data->MarioX) / 16 )][((data->MarioY+15-data->MapY)/16)]!=0)||(data->Map[(((0-data->MapX)+data->MarioX) / 16 )][((data->MarioY-data->MapY)/16)]!=0)){
  data->MapX = data->MapX - ((delta / 10) * data->ButtonLeft);
}
if ((((0-data->MapX)+data->MarioX+11) / 16 )>-1)
if ((data->Map[(((0-data->MapX)+data->MarioX+11) / 16 )][((data->MarioY+15-data->MapY)/16)]!=0)||(data->Map[(((0-data->MapX)+data->MarioX+11) / 16 )][((data->MarioY-data->MapY)/16)]!=0)){
  data->MapX = data->MapX + ((delta / 10) * data->ButtonRight);
}
//data->MarioY = data->MarioY - ((delta / 10) * data->ButtonUp);
//data->MarioY = data->MarioY + ((delta / 10) * data->ButtonDown);

  //BUTTON_LEFT, BUTTON_RIGHT,
  /* Здесь код, который будет выполняться в цикле */
  /* Переменная delta содержит количество миллисекунд с последнего вызова */

  /* Здесь можно работать с кнопками и обновлять переменные */
  if (data->MapX >16){
    data->MapX = 3;
  data->MarioX = 30;
  data->MarioY = 32;
   }
    if (data->MarioY <0){
  data->MarioY = data->MarioY+1;
 data->MapY = data->MapY + 1;
    }

        if (data->MarioY >64-32){
  data->MarioY = data->MarioY-1;
  data->MapY = data->MapY -1;
    }
      if (data->MarioY >128){
  data->MapX = 3;
  data->MarioX = 30;
  data->MarioY = 32;
    }
    if (data->MapY <data->SetUpMapY){
     data->MapX = 3;
  data->MarioX = 30;
  data->MarioY = 32; 
      data->MapY= data->SetUpMapY+1;
      }
}

game_instance Mario = {
  "Mario",         /* Имя, отображаемое в меню */
  Mario_prepare,
  Mario_render,
  Mario_update,
  sizeof(MarioData),
  (void**)(&data)
};


/* Не забудьте зарегистрировать игру в application.cpp */
