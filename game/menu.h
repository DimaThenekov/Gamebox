#ifndef MENU_H
#define MENU_H

#include "graphics.h"

struct MenuItem
{
  const char *name;
  void *opaque;
};

struct Menu;

// Нужно вызвать для инициализации конкретного меню
Menu *menu_setup(MenuItem *items, uint8_t x = 0, uint8_t y = 0, uint8_t bg = OPAQUE);
// Возвращает указатель opaque выбранного пункта, если нажата кнопка выбора 
void *menu_update(Menu *m, unsigned long delta);
// Отображает меню на экране
void menu_render(Menu *m);
// Завершает работу меню
void menu_finish(Menu *m);

#endif

