#pragma GCC optimize ("-O3")

#ifndef EMULATED
#include "libgame.h"
#include "font.h"
#include "graphics.h"
#include "controls.h"
#include "music.h"
#include "menu.h"
#include "fxm_tunes.h"

struct PlayerData
{
};
static PlayerData* data;

static const MenuItem tunes[] PROGMEM = {
  {"Alla Turca", alla_turca},
  {"Atomix", atomix},
  {"Axel F. Theme", axel_f_theme},
  {"Aztec", aztec},
  {"Belegost", belegost},
  {"Chimera", chimera},
  {"Commando", commando},
  {"Crazy Comets and Penetrator", crazy_comets_and_penetrator},
  {"E.T. Flying", e_t_flying},
  {"Eqinoxe V", eqinoxe_v},
  {"ExplodingAtoms", exploding_atoms},
  {"Falcon", falcon},
  {"Feud", feud},
  {"FIRE", fire},
  {"Fugue", fugue},
  {"Ghostbusters", ghostbusters},
  {"Golden Triangle Anthem", golden_triangle_anthem},
  {"Indiana Jones 3", indiana_jones_3},
  {"Jet Story", jet_story},
  {"Land Of Confusion", land_of_confusion},
  {"Magnetic Fields IV", magnetic_fields_iv},
  {"Magnetic Fields V", magnetic_fields_v},
  {"Master of Magic", master_of_magic},
  {"Monty On The Run", monty_on_the_run},
  {"Piskworks-Topgun", piskworks_topgun},
  {"Red dawn", red_dawn},
  {"RendezVous IV", rendezvous_iv},
  {"SpaceCrusade", space_crusade},
  {"StarDragon", stardragon},
  {"StarDragon Hall of Fame", stardragon_hall_of_fame},
  {"Terra Cresta", terra_cresta},
  {"TerraCresta", terra_cresta_},
  {"Tetris2", tetris2},
  {"The Last V8", the_last_v8},
  {"View to a Kill", view_to_a_kill},
  {"Where Time Dropped Dead", where_time_dropped_dead},
  {"ZUB", zub},
  {NULL, NULL}
};

void Player_prepare()
{
  game_set_ups(50);
  menu_setup(tunes);
  fxm_enable_int();
}

void Player_setup_melody(int i)
{
  fxm_enable_int();
  fxm_init(tunes[i].opaque);
}

void Player_setup_random_melody()
{
  //TODO
  //Player_setup_melody(rand() % (sizeof(tunes) / sizeof(MenuItem) - 1));
}

static void Player_render()
{
  menu_render();
}

static void Player_update(unsigned long delta)
{
  void *p = menu_update(delta);
  if (p)
  {
    fxm_init(p);
  }
}

game_instance Player = {
    "Player",
    Player_prepare,
    Player_render,
    Player_update,
    sizeof(PlayerData),
    (void**)(&data)
};
#endif

