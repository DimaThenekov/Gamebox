#ifndef MENU_H
#define MENU_H

#include "graphics.h"

struct MenuItem
{
  const char name[12];
  /* Можно хранить любой указатель.
     const написано для хранения указателей на
     данные в программной памяти без необходимости
     приведения их к (void*). */
  const void *opaque;
};

struct Menu;

// Нужно вызвать для инициализации конкретного меню
Menu *menu_setup(const MenuItem *items, uint8_t x = 0, uint8_t y = 0, uint8_t bg = TRANSPARENT);
// Возвращает указатель opaque выбранного пункта, если нажата кнопка выбора 
const void *menu_update(Menu *m, unsigned long delta);
// Отображает меню на экране
void menu_render(Menu *m);
// Завершает работу меню
void menu_finish(Menu *m);

#endif

