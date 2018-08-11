#pragma GCC optimize ("-O3")

#ifndef EMULATED
#include "libgame.h"
#include "font.h"
#include "graphics.h"
#include "controls.h"
#include "music.h"
#include "menu.h"
#include "fxm_tunes.h"
#include "random.h"

struct PlayerData
{
    Menu *menu;
};
static PlayerData* data;

static const MenuItem tunes[] PROGMEM = {
  {"Alla Turca", alla_turca},
  {"Atomix", atomix},
  {"Axel F. The"/*me"*/, axel_f_theme},
  {"Aztec", aztec},
  {"Belegost", belegost},
  {"Chimera", chimera},
  {"Commando", commando},
  {"Crazy Comet"/*s and Penetrator"*/, crazy_comets_and_penetrator},

  {"E.T. Flying", e_t_flying},
  {"Eqinoxe V", eqinoxe_v},
  {"ExplodingAt"/*oms"*/, exploding_atoms},
  {"Falcon", falcon},
  {"Feud", feud},
  {"FIRE", fire},
  {"Fugue", fugue},
  {"Ghostbuster"/*s"*/, ghostbusters},

  {"Golden Tria"/*ngle Anthem"*/, golden_triangle_anthem},
  {"Indiana Jon"/*es 3"*/, indiana_jones_3},
  {"Jet Story", jet_story},
  {"Land Of Con"/*fusion"*/, land_of_confusion},
  {"Magnetic Fi"/*elds IV"*/, magnetic_fields_iv},
  {"Magnetic Fi"/*elds V"*/, magnetic_fields_v},
  {"Master of M"/*agic"*/, master_of_magic},
  {"Monty On Th"/*e Run"*/, monty_on_the_run},

  {"Piskworks-T"/*opgun"*/, piskworks_topgun},
  {"Red dawn", red_dawn},
  {"RendezVous "/*IV"*/, rendezvous_iv},
  {"SpaceCrusad"/*e"*/, space_crusade},
  {"StarDragon", stardragon},
  {"StarDragon "/*Hall of Fame"*/, stardragon_hall_of_fame},
  {"Terra Crest"/*a"*/, terra_cresta},
  {"TerraCresta", terra_cresta_},

  {"Tetris2", tetris2},
  {"The Last V8", the_last_v8},
  {"View to a K"/*ill"*/, view_to_a_kill},
  {"Where Time "/*Dropped Dead"*/, where_time_dropped_dead},
  {"ZUB", zub},
  {"", NULL}
};

void Player_prepare()
{
  game_set_ups(50);
  data->menu = menu_setup(tunes);
}

void Player_setup_melody(int i)
{
  fxm_disable();
  fxm_init(pgm_read_pointer(&tunes[i].opaque));
  fxm_enable();
}

void Player_setup_random_melody()
{
  Player_setup_melody(rand() % (sizeof(tunes) / sizeof(MenuItem) - 1));
}

static void Player_render()
{
  menu_render(data->menu);
}

static void Player_update(unsigned long delta)
{
  void *p = menu_update(data->menu, delta);
  if (p)
  {
    fxm_disable();
    fxm_init(p);
    fxm_enable();
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
