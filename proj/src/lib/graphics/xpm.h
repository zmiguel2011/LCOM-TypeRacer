#ifndef _LCOM_XPM_H_
#define _LCOM_XPM_H_

#include <lcom/lcf.h>

/** @defgroup XPM
 * @{
 */

enum xpm_list{
    /* General */
    MOUSE,

    /* Game */
    ROAD,
    LAMBO,
    FERRARI,
    FONT_SPACE,
    FONT_SPACE_DONE,
    BUFFER,
    SEMAPHORE_GREEN,
    SEMAPHORE_YELLOW,
    SEMAPHORE_RED,
    SEMAPHORE_NONE,

    /* Menu */
    GAME_TITLE,
    MAIN_MENU,
    SINGLEPLAYER,
    MULTIPLAYER,
    LEADERBOARD,
    EXIT,
    BACKGROUND,
    MENU_CAR
};

/**
 * Initializes all the xpms
 */
void xpm_init();
/**
 * Returns a certain XPM
 * @param xpm XPM name
 * @return Struct containing the XPM
 */
xpm_image_t getXPM(enum xpm_list xpm);

/**@}*/

#endif
