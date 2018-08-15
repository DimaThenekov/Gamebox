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
    // ШИРИНА, ВЫСОТА, КОЛИЧЕСТВО БАЙТ НА СТРОКУ, ДАННЫЕ
    13, 10, 2, YourSprite_lines
};

*/

const uint8_t ShipSprite_lines[] PROGMEM = {
    B01111111, B11000000,
    B11110000, B00000000,
    B11111100, B00000000,
    B11111111, B11110000,
    B11111111, B11110000,
    B11111100, B00000000,
    B11110000, B00000000,
    B01111111, B11000000,
};

const game_sprite ShipSprite PROGMEM = {
    12, 8, 2, ShipSprite_lines
};

const uint8_t BallSprite_lines[] PROGMEM = {
    B11100000,
    B11100000,
    B11100000,

};

const game_sprite BallSprite PROGMEM = {
    3, 3, 1, BallSprite_lines
};

const uint8_t GameoverSprite_lines[] PROGMEM = {
    B00001111, B10000000,B00000000,B00000000,B00000011,B11100000,B00000000,B00000000,
    B00011000, B10000000,B00000000,B00000000,B00000110,B00110000,B00000000,B00000000,
    B00010000, B00001110,B00011011,B00001100,B00000100,B00010100,B00100110,B00101100,
    B00010000, B00000001,B00100100,B10010010,B00000100,B00010110,B01101001,B00110000,
    B00010011, B10001111,B00100100,B10011110,B00000100,B00010010,B01001111,B00100000,
    B00010000, B10010001,B00100100,B10010000,B00000100,B00010010,B01001000,B00100000,
    B00011000, B10010001,B00100100,B10010010,B00000110,B00110011,B11001001,B00100000,
    B00001111, B10001111,B00100100,B10001100,B00000011,B11100001,B10000110,B00100000,
};

const game_sprite GameoverSprite PROGMEM = {
    64, 8, 8, GameoverSprite_lines
};


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

struct SpaceShipsData
{
    short int 
    ShipX,
    ShipY,
    ObjY_1,
    ObjX_1,
    ObjY_2,
    ObjX_2,
    lives,
    score,
    game_set;
    float 
    Obj_spX_1,
    Obj_spY_1,
    Obj_spX_2,
    Obj_spY_2;
    
    /* Объявляйте ваши переменные здесь */
    /* Чтобы потом обращаться к ним, пишите data->ПЕРЕМЕННАЯ */
};
static SpaceShipsData* data; /* Эта переменная - указатель на структуру, которая содержит ваши переменные */

static void SpaceShips_prepare()
{
  
  data->ShipX = 5;
  data->ShipY = 32;
  data->ObjY_1 = (data->ShipY - 5) + rand() % 18;
  data->ObjX_1 = 65;
  data->ObjY_2 = (data->ShipY - 5) + rand() % 18;
  data->ObjX_2 = 65;
  data->Obj_spX_1 = 0.01;
  data->Obj_spY_1 = 0.01;
  data->Obj_spX_2 = 0.1;
  data->Obj_spY_2 = 0.1;
  data->lives = 3;
  data->score = 0;
  data->game_set = 0;
    /* Здесь код, который будет исполнятся один раз */
    /* Здесь нужно инициализировать переменные */
}

static void SpaceShips_render()
{
    if (data->lives > 0)
    {
    game_draw_sprite(&ShipSprite,data->ShipX,data->ShipY,PURPLE);
    }
    else
    {
    game_draw_sprite(&GameoverSprite,0,28,WHITE);
    }
    
    if ((data->ObjX_1 > 0) && (data->lives > 0))
    {   
        game_draw_sprite(&BallSprite,data->ObjX_1,data->ObjY_1,WHITE);
    }
        game_draw_digits((uint16_t)data->lives,2, 1, 0, WHITE);

        game_draw_digits((uint16_t)data->score,4, 48, 0, WHITE);

    if ((data->lives > 0) && (data->game_set >= 1))
    {
       game_draw_sprite(&BallSprite,data->ObjX_2,data->ObjY_2,GREEN);
    }

    //Text

    if (data->game_set == 0)
    game_draw_text((uint8_t*)"EASY", 18, 0, CYAN);
    
    if (data->game_set == 1)
    game_draw_text((uint8_t*)"NORM.", 18, 0, BLUE);

    if (data->game_set == 2)
    game_draw_text((uint8_t*)"HARD", 18, 0, GREEN);

    if (data->game_set == 3)
    game_draw_text((uint8_t*)"EXTR.", 18, 0, YELLOW);

    if (data->game_set == 4)
    game_draw_text((uint8_t*)"EXT.H", 18, 0, RED);
    /* Здесь код, который будет вывзваться для отрисовки кадра */
    /* Он не должен менять состояние игры, для этого есть функция update */

    /* Здесь (и только здесь) нужно вызывать функции game_draw_??? */
}

// Spawns

void CoolSpawn_1()
{
if (data->lives > 0)
{
  
  if ((data->ObjX_1 > 0))
          {
           data->ObjX_1 = data->ObjX_1 - data->Obj_spX_1;
          }
if (data->ObjX_1 == 1)
      {
  
  if (data->ShipY <= 13)
          {
            data->ObjY_1 = 8 + rand() % 18;
            data->ObjX_1 = 65;
          ++data->score;
          }
          else
              if (data ->ShipY >= 48)
              {
                data->ObjY_1 = 48 + rand() % 13;
                data->ObjX_1 = 65;
              ++data->score;
              }
              else
            {
              data->ObjY_1 = (data->ShipY - 5) + rand() % 18;
              data->ObjX_1 = 65;
              ++data->score;
            }
      }
  if ((data->ObjY_1 >= (data->ShipY - 2)) && ((data->ObjY_1 + 3) <= (data->ShipY + 10)) && ((data->ObjX_1 >= data->ShipX - 2) && (data->ObjX_1 + 3) <= (data->ShipX + 14)))
      {
        if (data->ShipY <= 13)
          {
            data->ObjY_1 = 8 + rand() % 18;
            data->ObjX_1 = 65;
          ++data->score;
          }
          else
              if (data ->ShipY >= 48)
              {
                data->ObjY_1 = 48 + rand() % 13;
                data->ObjX_1 = 65;
              ++data->score;
              }
              else
            {
              data->ObjY_1 = (data->ShipY - 5) + rand() % 18;
              data->ObjX_1 = 65;
              ++data->score;
            }
        
        data->lives = data->lives - 1;
      }
}
}

     
void CoolSpawn_2()

      
{
if (data->lives > 0)
{
  if ((data->ObjX_2 > 0))
          {
           data->ObjX_2 = data->ObjX_2 - data->Obj_spX_2;
          }
if (data->ObjX_2 == 1)
      {
  
  if (data->ShipY <= 13)
          {
            data->ObjY_2 = 8 + rand() % 18;
            data->ObjX_2 = 65;
          ++data->score;
          }
          else
              if (data ->ShipY >= 48)
              {
                data->ObjY_2 = 48 + rand() % 13;
                data->ObjX_2 = 65;
              ++data->score;
              }
              else
            {
              data->ObjY_2 = (data->ShipY - 5) + rand() % 18;
              data->ObjX_2 = 65;
              ++data->score;
            }
      }
  if ((data->ObjY_2 >= (data->ShipY - 2)) && ((data->ObjY_2 + 3) <= (data->ShipY + 10)) && ((data->ObjX_2 >= data->ShipX - 2) && (data->ObjX_2 + 3) <= (data->ShipX + 14)))
      {
        if (data->ShipY <= 13)
          {
            data->ObjY_2 = 8 + rand() % 18;
            data->ObjX_2 = 65;
          ++data->score;
          }
          else
              if (data ->ShipY >= 48)
              {
                data->ObjY_2 = 48 + rand() % 13;
                data->ObjX_2 = 65;
              ++data->score;
              }
              else
            {
              data->ObjY_2 = (data->ShipY - 5) + rand() % 18;
              data->ObjX_2 = 65;
              ++data->score;
            }
        
        data->lives = data->lives - 1;
      }
            
}
}

static void SpaceShips_update(unsigned long delta)
{
  //GameOver

  if((data->lives <= 0) && (game_is_button_pressed(BUTTON_A)))
  {
    data->lives = 3;
    data->score = 0;
  }
  
  //Difficity

  switch(data->score)
  {
    case 0: //Easy
    {
      game_set_ups(25);
      data->game_set = 0;
      data->Obj_spX_1 = 0.01;
      data->Obj_spY_1 = 0.01;
      break;
    }
    case 25: //Normal
    {      
      game_set_ups(25);
      data->game_set = 1;
      data->Obj_spX_1 = 0.01;
      data->Obj_spY_1 = 0.01;
      data->Obj_spX_2 = 0.1;
      data->Obj_spY_2 = 0.1;
      break;
    }
    case 50: //Hard
    {
      game_set_ups(25);
      data->game_set = 2;
      data->Obj_spX_1 = 0.1;
      data->Obj_spY_1 = 0.1;
      data->Obj_spX_2 = 1;
      data->Obj_spY_2 = 1;
      break;
    }
    case 100: //Extremal
    {
      game_set_ups(50);
      data->game_set = 3;
      data->Obj_spX_1 = 1;
      data->Obj_spY_1 = 1;
      break;
    }
    case 200: //Extra Hard!!!
    {      
      game_set_ups(75);
      data->game_set = 4;
      data->Obj_spX_1 = 1;
      data->Obj_spY_1 = 1;
      break;
    }
  }
  
  // Ship Controls
  if (game_is_button_pressed(BUTTON_RIGHT) && data->ShipX < 49 )
      {
        ++data->ShipX;
      }
  if (game_is_button_pressed(BUTTON_LEFT) && data->ShipX > 0)
      {
          --data->ShipX;
      }
  if (game_is_button_pressed(BUTTON_UP) && data->ShipY > 8)
      {
          --data->ShipY;
      }
  if (game_is_button_pressed(BUTTON_DOWN) && data->ShipY < 56)
        {
          ++data->ShipY;
        }

      // Random Objects

      

      //Easy
  if (data->game_set == 0)
  {
        CoolSpawn_1();
  }

      //Normal
  if (data->game_set >= 1)
  {
        
        
        CoolSpawn_1();
        
        if ((data->ObjX_1 - 20) > data->ObjX_2 || (data->ObjX_2 - 20) > data->ObjX_1)
        {
        CoolSpawn_2();
        }
  }


      //Hard



      //Extremal



      //Extra Hard!!!


      
      // Score
      
    /* Здесь код, который будет выполняться в цикле */
    /* Переменная delta содержит количество миллисекунд с последнего вызова */

    /* Здесь можно работать с кнопками и обновлять переменные */
}

game_instance SpaceShips = {
    "SpaceShips",         /* Имя, отображаемое в меню */
    SpaceShips_prepare,
    SpaceShips_render,
    SpaceShips_update,
    sizeof(SpaceShipsData),
    (void**)(&data)
};


/* Не забудьте зарегистрировать игру в application.cpp */
