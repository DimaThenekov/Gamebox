#pragma GCC optimize ("-O3")

#include "libgame.h"
#include "graphics.h"
#include "binary.h"
#include "controls.h"


static void make_a_move();
static void proverka();

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
 */
    const uint8_t krestikup_lines[] PROGMEM = {
    B11100000, B00000000, B01110000,
    B11110000, B00000000, B11110000,
    B11111000, B00000001, B11110000,
    B01111100, B00000011, B11100000,
    B00111110, B00000111, B11000000,
    B00011111, B00001111, B10000000,
    B00001111, B10011111, B00000000,
    B00000111, B11111110, B00000000,
    B00000011, B11111100, B00000000,
    B00000001, B11111000, B00000000,
   
};

const game_sprite krestikup PROGMEM = {
    // ШИРИНА, ВЫСОТА, КОЛИЧЕСТВО БАЙТ НА СТРОКУ, ДАННЫЕ
    20, 10, krestikup_lines
};


/*
 * ------------------------------
xxx       xxx
 xxx     xxx
  xxx   xxx
    xxxxx
  xxx   xxx
 xxx     xxx
xxx       xxx


 --------------------------------
 */

    const uint8_t krestikdow_lines[] PROGMEM = {
    B00000001, B11111000, B00000000,
    B00000011, B11111100, B00000000,
    B00000111, B11111110, B00000000,
    B00001111, B10011111, B00000000,
    B00011111, B00001111, B10000000,
    B00111110, B00000111, B11000000,
    B01111100, B00000011, B11100000,
    B11111000, B00000001, B11110000,
    B11110000, B00000000, B11110000,
    B11100000, B00000000, B01110000,
   
};

const game_sprite krestikdow PROGMEM = {
    // ШИРИНА, ВЫСОТА, КОЛИЧЕСТВО БАЙТ НА СТРОКУ, ДАННЫЕ
    20, 10, krestikdow_lines
};

   const uint8_t nolikup_lines[] PROGMEM = {
    B00000001, B11111000, B00000000,
    B00000111, B11111110, B00000000,
    B00011111, B00001111, B10000000,
    B00111100, B00000011, B11000000,
    B00110000, B00000000, B11000000,
    B01100000, B00000000, B01100000,
    B01100000, B00000000, B01100000,
    B11000000, B00000000, B00110000,
    B11000000, B00000000, B00110000,
    B11000000, B00000000, B00110000,
   
};

const game_sprite nolikup PROGMEM = {
    // ШИРИНА, ВЫСОТА, КОЛИЧЕСТВО БАЙТ НА СТРОКУ, ДАННЫЕ
    20, 10, nolikup_lines
};
const uint8_t nolikdow_lines[] PROGMEM = {
    B11000000, B00000000, B00110000,
    B11000000, B00000000, B00110000,
    B11000000, B00000000, B00110000,
    B01100000, B00000000, B01100000,
    B01100000, B00000000, B01100000,
    B00110000, B00000000, B11000000,
    B00111100, B00000011, B11000000,
    B00011111, B00001111, B10000000,
    B00000011, B11111100, B00000000,
    B00000001, B11111000, B00000000

};

const game_sprite nolikdow PROGMEM = {
    // ШИРИНА, ВЫСОТА, КОЛИЧЕСТВО БАЙТ НА СТРОКУ, ДАННЫЕ
    20, 10, nolikdow_lines
};


/*const uint8_t kursor_lines[] PROGMEM = {
    B00000000, B00000000,
    B00000000, B00000000, 
    B00000000, B00000000,
    B00000000, B00000000, 
    B00000000, B00000000, 
    B00000000, B00000000, 
    B00000000, B00000000, 
    B00000000, B00000000, 
    B00000000, B00000000, 
    B00000000, B00000000

};


const game_sprite kursor PROGMEM = {
    // ШИРИНА, ВЫСОТА, КОЛИЧЕСТВО БАЙТ НА СТРОКУ, ДАННЫЕ
    20, 10, 3, kursor_lines
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

struct TemplateData
{
    /* Объявляйте ваши переменные здесь */
    /* Чтобы потом обращаться к ним, пишите data->ПЕРЕМЕННАЯ */

    int kn[5][2],a[3][3];
    int x, y, z,q,w,d,s,n;
};
static TemplateData* data; /* Эта переменная - указатель на структуру, которая содержит ваши переменные */

static void Template_prepare()
{
    game_set_ups(10);
    for(int i = 0; i < 9; i++)
      {
//  data->q[i] = false;
        data->a[i][i] = -1;
      }
    data->x = data->y = data->z = data->q = data->w = data->d = data->s = data->n = 0;
    
}

static void Template_render()
{
    game_draw_vline(20, 0, 63, WHITE); // vertical line
    game_draw_vline(21, 0, 63, WHITE); // vertical line
    game_draw_vline(43, 0, 63, WHITE); // vertical line
    game_draw_vline(42, 0, 63, WHITE); // vertical line
    game_draw_rect(0, 20, 64, 2, WHITE);
    game_draw_rect(0, 42, 64, 2, WHITE);

//while(flag != false){
  if(44>=data->x && data->x>=0 && 44>=data->y && data->y>=0){
    game_draw_vline(data->x, data->y, data->y+19, YELLOW); // vertical line
    game_draw_vline(data->x+19, data->y, data->y+19, YELLOW); // vertical line
    game_draw_rect(data->x, data->y, 20, 1, YELLOW);
    game_draw_rect(data->x, data->y+19, 20, 1, YELLOW);
  }
  for(int i = 0; i < 9; i++){
    for(int j = 0; j < 9; j++){
      if(data->a[i][j] == 1){
        game_draw_sprite(&krestikup, i*22, j*22, RED);         //КРЕСТИК
        game_draw_sprite(&krestikdow, i*22, (j*22)+10, RED);     // КРЕСТИК
      }
       if(data->a[i][j] == 2){
        game_draw_sprite(&nolikup, i*22, j*22, BLUE);         //КРЕСТИК
        game_draw_sprite(&nolikdow, i*22, (j*22)+10, BLUE);     // КРЕСТИК
      }
    }
  }
//}
    /* Здесь код, который будет вывзваться для отрисовки кадра */
    /* Он не должен менять состояние игры, для этого есть функция update */

    /* Здесь (и только здесь) нужно вызывать функции game_draw_??? */

/*  game_draw_sprite(&krestikup, 0, 0, RED);      КРЕСТИК
    game_draw_sprite(&krestikdow, 0, 10, RED);      КРЕСТИК
*/

/*
    game_draw_sprite(&nolikup, 22, 0, BLUE);         НОЛИК
    game_draw_sprite(&nolikdow, 22, 10, BLUE);       НОЛИК
*/



}

static void Template_update(unsigned long delta)
{
   if (game_is_button_pressed(BUTTON_LEFT))
        {
          if (data->x>0)
            {
              data->x-=22;
            }            
        }
        else if (game_is_button_pressed(BUTTON_RIGHT))
        {
          if (data->x<44)
            {
              data->x+=22;
            }            
        }
        else if (game_is_button_pressed(BUTTON_UP))
        {
            if (data->y>0)
            {
              data->y-=22;
            }            
        }
        else if (game_is_button_pressed(BUTTON_DOWN))
        {
            if (data->y<44)
            {
              data->y+=22;
            }             
        }
        else if (game_is_button_pressed(BUTTON_A))
        {
            
            
            
            if(data->x == 0)
              data->q = 0;
            if(data->x == 22)
              data->q = 1;
            if(data->x == 44)
              data->q = 2; 
              
            if(data->y == 0)
              data->w = 0;
            if(data->y == 22)
              data->w = 1;
            if(data->y == 44)
             data-> w = 2;  

           data->a[data->q][data->w] = 1;
           data->d++;
           data->s++;
        }
        if(data->d == 1)
        {
          make_a_move();
        }
       

        
        
 
    


    
}

const game_instance TicTacToe PROGMEM = {
    "TicTacToe",         /* Имя, отображаемое в меню */
    Template_prepare,
    Template_render,
    Template_update,
    sizeof(TemplateData),
    (void**)(&data)
};


static void make_a_move()
{
  
  proverka();

  
  if(data->s == 1)
  {
    if(data->a[1][1] == 1)
    {
      data->a[2][2] = 2;
      data->n = 1;
      data->s++;
      proverka();
    }
    else
    {
      data->a[1][1] = 2;
      data->n = 2;
      data->s++;
      proverka();
    }
    
  }
  else if(data->s > 1)
  {
    if(data->n == 1)
    {
      proverka();
    }
    if(data->n == 2)
    {
      proverka();
    }  
  }     
  data->d--;
}

static void proverka()
{
  if(data->a[0][0] == 1 && data->a[0][1] == 1)
  data->a[0][2] = 2;
else if(data->a[0][1] == 1 && data->a[0][2] == 1)
  data->a[0][0] = 2;
else if(data->a[0][0] == 1 && data->a[0][2] == 1)
  data->a[0][1] = 2;

else if(data->a[1][0] == 1 && data->a[1][1] == 1)
  data->a[1][2] = 2;
else if(data->a[1][1] == 1 && data->a[1][2] == 1)
  data->a[1][0] = 2;
else if(data->a[1][0] == 1 && data->a[1][2] == 1)
  data->a[1][1] = 2;

else if(data->a[2][0] == 1 && data->a[2][1] == 1)
  data->a[2][2] = 2;
else if(data->a[2][1] == 1 && data->a[2][2] == 1)
  data->a[2][0] = 2;
else if(data->a[2][0] == 1 && data->a[2][2] == 1)
  data->a[2][1] = 2;


else if(data->a[0][0] == 1 && data->a[1][0] == 1)
  data->a[2][0] = 2;
else if(data->a[1][0] == 1 && data->a[2][0] == 1)
  data->a[0][0] = 2;
else if(data->a[0][0] == 1 && data->a[2][0] == 1)
  data->a[1][0] = 2;

else if(data->a[0][1] == 1 && data->a[1][1] == 1)
  data->a[2][1] = 2;
else if(data->a[1][1] == 1 && data->a[2][1] == 1)
  data->a[0][1] = 2;
else if(data->a[0][1] == 1 && data->a[2][1] == 1)
  data->a[1][1] = 2;

else if(data->a[0][2] == 1 && data->a[1][2] == 1)
  data->a[2][2] = 2;
else if(data->a[1][2] == 1 && data->a[2][2] == 1)
  data->a[0][2] = 2;
else if(data->a[0][2] == 1 && data->a[2][2] == 1)
  data->a[1][2] = 2;

  


else if(data->a[0][0] == 1 && data->a[1][1] == 1)
  data->a[2][2] = 2;
else if(data->a[0][0] == 1 && data->a[2][2] == 1)
  data->a[1][1] = 2;
else if(data->a[1][1] == 1 && data->a[2][2] == 1)
  data->a[0][0] = 2; 

else if(data->a[2][0] == 1 && data->a[1][1] == 1)
  data->a[0][2] = 2;
else if(data->a[2][0] == 1 && data->a[0][2] == 1)
  data->a[1][1] = 2;
else if(data->a[1][1] == 1 && data->a[0][2] == 1)
  data->a[2][0] = 2; 






if(data->a[0][0] == 2 && data->a[0][1] == 2)
  data->a[0][2] = 1;
else if(data->a[0][1] == 2 && data->a[0][2] == 2)
  data->a[0][0] = 1;
else if(data->a[0][0] == 2 && data->a[0][2] == 2)
  data->a[0][1] = 1;

else if(data->a[1][0] == 2 && data->a[1][1] == 2)
  data->a[1][2] = 1;
else if(data->a[1][1] == 2 && data->a[1][2] == 2)
  data->a[1][0] = 1;
else if(data->a[1][0] == 2 && data->a[1][2] == 2)
  data->a[1][1] = 1;

else if(data->a[2][0] == 2 && data->a[2][1] == 2)
  data->a[2][2] = 1;
else if(data->a[2][1] == 2 && data->a[2][2] == 2)
  data->a[2][0] = 1;
else if(data->a[2][0] == 2 && data->a[2][2] == 2)
  data->a[2][1] = 1;
  

else if(data->a[0][0] == 2 && data->a[1][0] == 2)
  data->a[2][0] = 1;
else if(data->a[1][0] == 2 && data->a[2][0] == 2)
  data->a[0][0] = 1;
else if(data->a[0][0] == 2 && data->a[2][0] == 2)
  data->a[1][0] = 1;

else if(data->a[0][1] == 2 && data->a[1][1] == 2)
  data->a[2][1] = 1;
else if(data->a[1][1] == 2 && data->a[2][1] == 2)
  data->a[0][1] = 1;
else if(data->a[0][1] == 2 && data->a[2][1] == 2)
  data->a[1][1] = 1;

else if(data->a[0][2] == 2 && data->a[1][2] == 2)
  data->a[2][2] = 1;
else if(data->a[1][2] == 2 && data->a[2][2] == 2)
  data->a[0][2] = 1;
else if(data->a[0][2] == 2 && data->a[2][2] == 2)
  data->a[1][2] = 1;

  


else if(data->a[0][0] == 2 && data->a[1][1] == 2)
  data->a[2][2] = 1;
else if(data->a[0][0] == 2 && data->a[2][2] == 2)
  data->a[1][1] = 1;
else if(data->a[1][1] == 2 && data->a[2][2] == 2)
  data->a[0][0] = 1; 

else if(data->a[2][0] == 2 && data->a[1][1] == 2)
  data->a[0][2] = 1;
else if(data->a[2][0] == 2 && data->a[0][2] == 2)
  data->a[1][1] = 1;
else if(data->a[1][1] == 2 && data->a[0][2] == 2)
  data->a[2][0] = 1; 


}

/* Не забудьте зарегистрировать игру в application.cpp */
