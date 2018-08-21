#pragma GCC optimize ("-O3")

#include "libgame.h"
#include "graphics.h"
#include "binary.h"
#include "controls.h"
#include "music.h"
#include "tunes.h"

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
    B11000000,
    B11100000,
    B01100000,

};

const game_sprite BallSprite PROGMEM = {
    3, 3, 1, BallSprite_lines
};

const uint8_t GameoverSprite_lines[] PROGMEM = {
    B00001111,B10000000,B00000000,B00000000,B00000011,B11100000,B00000000,B00000000,
    B00011000,B10000000,B00000000,B00000000,B00000110,B00110000,B00000000,B00000000,
    B00010000,B00001110,B00011011,B00001100,B00000100,B00010100,B00100110,B00101100,
    B00010000,B00000001,B00100100,B10010010,B00000100,B00010110,B01101001,B00110000,
    B00010011,B10001111,B00100100,B10011110,B00000100,B00010010,B01001111,B00100000,
    B00010000,B10010001,B00100100,B10010000,B00000100,B00010010,B01001000,B00100000,
    B00011000,B10010001,B00100100,B10010010,B00000110,B00110011,B11001001,B00100000,
    B00001111,B10001111,B00100100,B10001100,B00000011,B11100001,B10000110,B00100000,
};

const game_sprite GameoverSprite PROGMEM = {
    64, 8, 8, GameoverSprite_lines
};

const uint8_t YouwinSprite_lines[] PROGMEM = {
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
    B00000000,B00100010,B00000000,B00001000,B10001010,B00000001,B00100100,B00000000,
    B00000000,B00100010,B00000000,B00000100,B10010000,B00000001,B00100100,B00000000,
    B00000000,B00010100,B11100100,B10000101,B11010010,B01110001,B00100100,B00000000,
    B00000000,B00001001,B00010100,B10000101,B01010010,B01001001,B00100100,B00000000,
    B00000000,B00001001,B00010100,B10000011,B01100010,B01001001,B00100100,B00000000,
    B00000000,B00001001,B00010100,B10000010,B00100010,B01001000,B00000000,B00000000,
    B00000000,B00001000,B11100111,B10000010,B00100010,B01001001,B00100100,B00000000,
    B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,B00000000,
};

const game_sprite YouwinSprite PROGMEM = {
    64, 8, 8, YouwinSprite_lines
};

struct SpaceShipsData
{
    short int 
    ShipX,
    ShipY,
    ObjY_1,
    ObjX_1,
    ObjY_2,
    ObjX_2,
    ObjY_3,
    ObjX_3,
    ObjY_4,
    ObjX_4,
    lives,
    score,
    game_set,
    kek_1,
    kek_2,
    kek_3,
    Obj_spX_1,
    Obj_spY_1,
    Obj_spX_2,
    Obj_spY_2,
    Obj_spX_3,
    Obj_spY_3,
    Obj_spX_4,
    Obj_spY_4,
    hiscore;
};
static SpaceShipsData* data;

static void SpaceShips_prepare()
{
  
  data->ShipX = 5;
  data->ShipY = 32;
  data->ObjY_1 = (data->ShipY - 5) + rand() % 18;
  data->ObjX_1 = 65;
  data->ObjY_2 = (data->ShipY - 5) + rand() % 18;
  data->ObjX_2 = 85;
  data->ObjY_3 =  64 * (0 + rand() % 2);
  data->ObjX_3 = 64;
  data->ObjX_4 = (data->ShipX - 5) + rand() % 22;
  data->ObjY_4 = 64;
  data->Obj_spX_1 = 1;
  data->Obj_spY_1 = 1;
  data->Obj_spX_2 = 1;
  data->Obj_spY_2 = 1;
  data->Obj_spX_3 = 1;
  data->Obj_spY_3 = 1;
  data->Obj_spX_4 = 1;
  data->Obj_spY_4 = 1;
  data->lives = 3;
  data->score = 0;
  data->game_set = 0;
  data->hiscore = 0;
  game_load(&data->hiscore, sizeof(data->hiscore));

#ifndef EMULATED
    tune_init(commando);
    tune_enable();
#endif
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
    
    if ((data->ObjX_1 > 0) && (data->lives > 0) && (data->game_set < 5))
    {   
        game_draw_sprite(&BallSprite,data->ObjX_1,data->ObjY_1,WHITE);
    }
        game_draw_digits((uint16_t)data->lives,2, 1, 0, WHITE);

        game_draw_digits((uint16_t)data->score,3, 52, 0, WHITE);

        game_draw_digits((uint16_t)data->hiscore,3, 34, 0, PURPLE);

    if ((data->lives > 0) && (data->game_set >= 1) && (data->game_set < 5))
    {
       game_draw_sprite(&BallSprite,data->ObjX_2,data->ObjY_2,GREEN);
    }

    if ((data->lives > 0) && (data->game_set >= 2) && (data->game_set < 5))
    {
       game_draw_sprite(&BallSprite,data->ObjX_4,data->ObjY_4,CYAN);
    }

    
    if ((data->lives > 0) && (data->game_set >= 3) && (data->game_set < 5))
    {
       game_draw_sprite(&BallSprite,data->ObjX_3,data->ObjY_3,RED);
    }
    if (data->game_set == 5)
    {
       game_draw_sprite(&YouwinSprite,0,28,WHITE);
    }

    
    //Text

    if (data->game_set == 0)
    game_draw_text((uint8_t*)"EASY", 10, 0, CYAN);
    
    if (data->game_set == 1)
    game_draw_text((uint8_t*)"NORM", 10, 0, BLUE);

    if (data->game_set == 2)
    game_draw_text((uint8_t*)"HARD", 10, 0, GREEN);

    if (data->game_set == 3)
    game_draw_text((uint8_t*)"EXTR", 10, 0, YELLOW);

    if (data->game_set == 4)
    game_draw_text((uint8_t*)"HELL", 10, 0, RED);

    if (data->game_set == 5)
    game_draw_text((uint8_t*)"WIN", 10, 0, WHITE);
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

if (data->ObjX_1 <= 2)
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
if (data->ObjX_2 <= 2)
      {
  
   if (data->ShipY <= 17)
          {
            data->ObjY_2 = 16 + rand() % 22;
            data->ObjX_2 = 65;
          ++data->score;
          }
          else
              if (data ->ShipY >= 48)
              {
                data->ObjY_2 = 40 + rand() % 21;
                data->ObjX_2 = 65;
              ++data->score;
              }
              else
            {
              data->kek_3 = (0 + rand() % 2);
              if (data->kek_3 == 0)
              {
                data->ObjY_2 = ((data->ShipY - 10) + rand() % 5);
              }
              else
              if (data->kek_3 == 1)
              {
              data->ObjY_2 = (data->ShipY + 13) + rand() % 5;
              }
              data->ObjX_2 = 65;
              ++data->score;
            }
      }

  
  if ((data->ObjY_2 >= (data->ShipY - 2)) && ((data->ObjY_2 + 3) <= (data->ShipY + 10)) && ((data->ObjX_2 >= data->ShipX - 2) && (data->ObjX_2 + 3) <= (data->ShipX + 14)))
      {
        if (data->ShipY <= 17)
          {
            data->ObjY_2 = 12 + rand() % 18;
            data->ObjX_2 = 65;
          ++data->score;
          }
          else
              if (data ->ShipY >= 48)
              {
                data->ObjY_2 = 44 + rand() % 17;
                data->ObjX_2 = 65;
              ++data->score;
              }
              else
            {
              data->kek_3 = (0 + rand() % 2);
              if (data->kek_3 == 0)
              {
                data->ObjY_2 = ((data->ShipY - 10) + rand() % 5);
              }
              else
              if (data->kek_3 == 1)
              {
              data->ObjY_2 = (data->ShipY + 13) + rand() % 5;
              }
              data->ObjX_2 = 65;
              ++data->score;
            }
        
        data->lives = data->lives - 1;
      }
            
}
}

void CoolSpawn_3()
  {
if (data->ObjY_3 == 64)
         {
          data->kek_1 = 1;
         }
         else
         if (data->ObjY_3 == 0)
         {
          data->kek_1 = 0;
         }
    
if (data->lives > 0)
{
  if (data->ObjX_3 > 0 && data->kek_1 == 0)
      {
        data->ObjY_3 = data->ObjY_3 + data->Obj_spY_3;
        data->ObjX_3 = data->ObjX_3 - data->Obj_spX_3;
      }
      else
  if (data->ObjX_3 > 0 && data->kek_1 == 1)
  {
     data->ObjY_3 = data->ObjY_3 - data->Obj_spY_3;
     data->ObjX_3 = data->ObjX_3 - data->Obj_spX_3;
  }
  
if (data->ObjX_3 <= 3)
      {
         data->ObjY_3 =  64 * (0 + rand() % 2);
         data->ObjX_3 = 64;
         ++data->score;
      }
  if ((data->ObjY_3 >= (data->ShipY - 2)) && ((data->ObjY_3 + 3) <= (data->ShipY + 10)) && ((data->ObjX_3 >= data->ShipX - 2) && (data->ObjX_3 + 3) <= (data->ShipX + 14)))
      {
           data->ObjY_3 =  64 * (0 + rand() % 2);
           data->ObjX_3 = 64;
           --data->lives;
      }
            
}
}

void CoolSpawn_4()
{

if (data->lives > 0)
{
  
  if (data->ObjY_4 >= -2)
          {
           data->ObjY_4 = data->ObjY_4 - data->Obj_spY_4;
          }
  
  if (data->ObjY_4 <= -3)
      {
         if (data->ShipX <= 17)
          {
          data->ObjY_4 = 64;
            data->ObjX_4 = 0 + rand() % 17;
              ++data->score;
          }
          else
              if (data ->ShipY >= 48)
              {
              data->ObjY_4 = 64;
                data->ObjX_4 = (data->ShipX - 5) + rand() % 17;
                  ++data->score;
              }
              else
            {
            data->ObjY_4 = 64;
              data->ObjX_4 = (data->ShipX - 5) + rand() % 22;
                ++data->score;
            }
      }
  if ((data->ObjY_4 >= (data->ShipY - 2)) && ((data->ObjY_4 + 3) <= (data->ShipY + 10)) && ((data->ObjX_4 >= data->ShipX - 2) && (data->ObjX_4 + 3) <= (data->ShipX + 14)))
      {
        if (data->ShipX <= 17)
          {
          data->ObjY_4 = 64;
            data->ObjX_4 = 0 + rand() % 17;
          }
          else
              if (data ->ShipY >= 48)
              {
              data->ObjY_4 = 64;
                data->ObjX_4 = (data->ShipX - 5) + rand() % 17;
              }
              else
            {
            data->ObjY_4 = 64;
              data->ObjX_4 = (data->ShipX - 5) + rand() % 22;
            }
            --data->lives;
      }
}
}

static void SpaceShips_update(unsigned long delta)
{
  //GameOver

  if((data->lives <= 0) && (game_is_button_pressed(BUTTON_A)))
  {
    data->ShipX = 5;
    data->ShipY = 32;
    data->lives = 3;
    data->score = 0;
    data->ObjY_1 = (data->ShipY - 5) + rand() % 18;
    data->ObjX_1 = 65;
    data->ObjY_2 = (data->ShipY - 5) + rand() % 18;
    data->ObjX_2 = 85;
    data->ObjY_3 =  64 * (0 + rand() % 1);
    data->ObjX_3 = 64;
    data->ObjX_4 = (data->ShipX - 5) + rand() % 22;
    data->ObjY_4 = 64;
  }

if (data->score > data->hiscore)
            {
                data->hiscore = data->score;
                if (data->lives <= 0)
                    game_save(&data->hiscore, sizeof(data->hiscore));
            }
  
  //Difficity

    if (data->score <= 5)  //Easy
    {
      game_set_ups(25);
      data->game_set = 0;
      data->Obj_spX_1 = 1;
      data->Obj_spY_1 = 1;
    }
    
    if (data->score <= 29 && data->score >= 6) //Normal
    {      
      game_set_ups(25);
      data->game_set = 1;
      data->Obj_spX_1 = 1;
      data->Obj_spY_1 = 1;
      data->Obj_spX_2 = 1;
      data->Obj_spY_2 = 1;
    }
    
    if (data->score <= 79 && data->score >=30) //Hard
    {
      game_set_ups(25);
      data->game_set = 2;
      data->Obj_spX_1 = 1;
      data->Obj_spY_1 = 1;
      data->Obj_spX_2 = 1;
      data->Obj_spY_2 = 1;
      data->Obj_spX_3 = 1;
      data->Obj_spY_3 = 1;
      data->Obj_spX_4 = 1;
      data->Obj_spY_4 = 1;
    }
    
    if (data->score <= 665 && data ->score >= 80) //Extremal
    {
      game_set_ups(25);
      data->game_set = 3;
      data->Obj_spX_1 = 2;
      data->Obj_spY_1 = 2;
      data->Obj_spX_2 = 1;
      data->Obj_spY_2 = 1;
      data->Obj_spX_3 = 1;
      data->Obj_spY_3 = 1;
      data->Obj_spX_4 = 2;
      data->Obj_spY_4 = 2;
    }
    
    if (data->score >= 666 && data->score <= 998) //Extra Hard!!!
    {      
      game_set_ups(50);
      data->game_set = 4;
      data->Obj_spX_1 = 3;
      data->Obj_spY_1 = 3;
      data->Obj_spX_2 = 2;
      data->Obj_spY_2 = 2;
      data->Obj_spX_3 = 2;
      data->Obj_spY_3 = 2;
      data->Obj_spX_4 = 3;
      data->Obj_spY_4 = 3;
    }
    if (data->score >= 999) //You Win!!!
    {      
      if (data->game_set != 5)
          game_save(&data->hiscore, sizeof(data->hiscore));
      game_set_ups(50);
      data->game_set = 5;
      data->Obj_spX_1 = 0;
      data->Obj_spY_1 = 0;
      data->Obj_spX_2 = 0;
      data->Obj_spY_2 = 0;
      data->Obj_spX_3 = 0;
      data->Obj_spY_3 = 0;
      data->Obj_spX_4 = 0;
      data->Obj_spY_4 = 0;
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

switch (data->game_set)
{
      //Easy
  case 0:
  {
        CoolSpawn_1();
        break;
  }

      //Normal
  case 1:
  {
        CoolSpawn_1();
        
        CoolSpawn_2();
        break;
  }
      //Hard
  case 2:
  {
        CoolSpawn_1();
        
        CoolSpawn_2();

        CoolSpawn_4();
        break;
  }


      //Extremal
  case 3:
  {
        CoolSpawn_1();
        
        CoolSpawn_2();

        CoolSpawn_3();

        CoolSpawn_4();
        break;
  }


      //Extra Hard!!!
  case 4:
  {
        CoolSpawn_1();
        
        CoolSpawn_2();

        CoolSpawn_3();

        CoolSpawn_4();
        break;
  }
  }
}

game_instance SpaceShips = {
    "SpaceShips",
    SpaceShips_prepare,
    SpaceShips_render,
    SpaceShips_update,
    sizeof(SpaceShipsData),
    (void**)(&data)
};
