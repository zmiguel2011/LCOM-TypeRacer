#ifndef _LCOM_MENU_H_
#define _LCOM_MENU_H_

/** @defgroup Menu
 * @{
 */

enum MenuOption{
    OPTION_SINGLEPLAYER, 
    OPTION_MULTIPLAYER, 
    OPTION_LEADERBOARD, 
    OPTION_EXIT, 
    OPTION_NONE
};

/**
 * Initializes menu
 */
void menu_init();
/**
 * Draws the menu
 */
void menu_draw();
/**
 * Handles the menu options
 */
enum MenuOption menu_handler();

#endif
