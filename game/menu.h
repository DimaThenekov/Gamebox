#ifndef MENU_H
#define MENU_H

struct MenuItem
{
  const char *name;
  void *opaque;
};

// Нужно вызвать для инициализации конкретного меню
void menu_setup(MenuItem *items);
// Возвращает указатель opaque выбранного пункта, если нажата кнопка выбора 
void *menu_update(unsigned long delta);
// Отображает меню на экране
void menu_render();

#endif

