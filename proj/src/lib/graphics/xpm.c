#include "xpm.h"
#include "xpm/xpm_road.h"
#include "xpm/xpm_ferrari.h"
#include "xpm/xpm_lambo.h"
#include "xpm/xpm_menu.h"
#include "xpm/xpm_mouse.h"
#include "xpm/xpm_space.h"
#include "xpm/xpm_space_done.h"
#include "xpm/xpm_buffer.h"
#include "xpm/xpm_sem_green.h"
#include "xpm/xpm_sem_red.h"
#include "xpm/xpm_sem_yellow.h"
#include "xpm/xpm_sem_none.h"
#include "xpm/xpm_background.h"
#include "xpm/xpm_menu_car.h"

#include "../../project/config.h"

static xpm_image_t mouse;

static xpm_image_t road;
static xpm_image_t lambo;
static xpm_image_t ferrari;
static xpm_image_t font_space;
static xpm_image_t font_space_done;
static xpm_image_t buffer;

static xpm_image_t semaphore_green;
static xpm_image_t semaphore_red;
static xpm_image_t semaphore_yellow;
static xpm_image_t semaphore_none;

static xpm_image_t gametitle;
static xpm_image_t mainmenu;
static xpm_image_t singleplayer;
static xpm_image_t multiplayer;
static xpm_image_t leaderboard;
static xpm_image_t __exit;
static xpm_image_t background;
static xpm_image_t menu_car;

/**
 * Initializes all the xpms
 */
void xpm_init() {
  xpm_load((xpm_map_t) xpm_mouse, XPM_IMAGE_TYPE, &mouse);

  xpm_load((xpm_map_t) xpm_road, XPM_IMAGE_TYPE, &road);
  xpm_load((xpm_map_t) xpm_lambo, XPM_IMAGE_TYPE, &lambo);
  xpm_load((xpm_map_t) xpm_ferrari, XPM_IMAGE_TYPE, &ferrari);
  xpm_load((xpm_map_t) xpm_font_space, XPM_IMAGE_TYPE, &font_space);
  xpm_load((xpm_map_t) xpm_font_space_done, XPM_IMAGE_TYPE, &font_space_done);
  xpm_load((xpm_map_t) xpm_buffer, XPM_IMAGE_TYPE, &buffer);

  xpm_load((xpm_map_t) xpm_sem_green, XPM_IMAGE_TYPE, &semaphore_green);
  xpm_load((xpm_map_t) xpm_sem_red, XPM_IMAGE_TYPE, &semaphore_red);
  xpm_load((xpm_map_t) xpm_sem_yellow, XPM_IMAGE_TYPE, &semaphore_yellow);
  xpm_load((xpm_map_t) xpm_sem_none, XPM_IMAGE_TYPE, &semaphore_none);

  xpm_load((xpm_map_t) xpm_game_title, XPM_IMAGE_TYPE, &gametitle);
  xpm_load((xpm_map_t) xpm_main_menu, XPM_IMAGE_TYPE, &mainmenu);
  xpm_load((xpm_map_t) xpm_single_player, XPM_IMAGE_TYPE, &singleplayer);
  xpm_load((xpm_map_t) xpm_multiplayer, XPM_IMAGE_TYPE, &multiplayer);
  xpm_load((xpm_map_t) xpm_leaderboard, XPM_IMAGE_TYPE, &leaderboard);
  xpm_load((xpm_map_t) xpm_exit, XPM_IMAGE_TYPE, &__exit);
  xpm_load((xpm_map_t) xpm_background, XPM_IMAGE_TYPE, &background);
  xpm_load((xpm_map_t) xpm_menu_car, XPM_IMAGE_TYPE, &menu_car);
}

/**
 * Returns a certain XPM
 * @param xpm XPM name
 * @return Struct containing the XPM
 */
xpm_image_t getXPM(enum xpm_list xpm) {
  switch (xpm) {
    case MOUSE: return mouse;
    case ROAD: return road;
    case LAMBO: return lambo;
    case FERRARI: return ferrari;
    case FONT_SPACE: return font_space;
    case FONT_SPACE_DONE: return font_space_done;
    case BUFFER: return buffer;
    case GAME_TITLE: return gametitle;
    case MAIN_MENU: return mainmenu;
    case SINGLEPLAYER: return singleplayer;
    case MULTIPLAYER: return multiplayer;
    case LEADERBOARD: return leaderboard;
    case EXIT: return __exit;
    case SEMAPHORE_GREEN: return semaphore_green;
    case SEMAPHORE_YELLOW: return semaphore_yellow;
    case SEMAPHORE_RED: return semaphore_red;
    case SEMAPHORE_NONE: return semaphore_none;
    case BACKGROUND: return background;
    case MENU_CAR: return menu_car;
    default: return lambo;
  }
}
