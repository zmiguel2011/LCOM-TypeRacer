#include <lcom/lcf.h>

#include "../lib/graphics/video_gr.h"
#include "../lib/graphics/xpm.h"

#include "menu.h"
#include "config.h"

/** @defgroup Menu
 * @{
 */

#define GAMETITLE_Y 170
#define MAINMENU_Y 320
#define SINGLEPLAYERTEXT_Y 417
#define MULTIPLAYERTEXT_Y 511
#define LEADERBOARTEXT_Y 605
#define EXITTEXT_Y 692

static int XRes;

static xpm_image_t gameTitle;
static xpm_image_t mainMenuText;
static xpm_image_t singlePlayerText;
static xpm_image_t multiPlayerText;
static xpm_image_t leaderboardText;
static xpm_image_t exitText;

int singleplayer_x;
int multiplayer_x;
int leaderboard_x;
int exit_x;

/**
 * Calculates XPM center position
 * @param width Width of xpm
 * @return X centered position
 */
int calculateCenterX(int width){
    return (XRes - width) / 2;
}

/**
 * Initializes menu
 */
void menu_init(){
    XRes = get_XRes();

    gameTitle = getXPM(GAME_TITLE);
    mainMenuText = getXPM(MAIN_MENU);
    singlePlayerText = getXPM(SINGLEPLAYER);
    multiPlayerText = getXPM(MULTIPLAYER);
    leaderboardText = getXPM(LEADERBOARD);
    exitText = getXPM(EXIT);

    singleplayer_x = calculateCenterX(singlePlayerText.width);
    multiplayer_x = calculateCenterX(multiPlayerText.width); 
    leaderboard_x = calculateCenterX(leaderboardText.width);
    exit_x = calculateCenterX(exitText.width);
}

/**
 * Draws the menu
 */
void menu_draw(){
    vg_draw_xpm(calculateCenterX(gameTitle.width), GAMETITLE_Y, gameTitle);
    vg_draw_xpm(calculateCenterX(mainMenuText.width), MAINMENU_Y, mainMenuText);
    vg_draw_xpm(singleplayer_x, SINGLEPLAYERTEXT_Y, singlePlayerText);
    vg_draw_xpm(multiplayer_x, MULTIPLAYERTEXT_Y, multiPlayerText);
    vg_draw_xpm(leaderboard_x, LEADERBOARTEXT_Y, leaderboardText);
    vg_draw_xpm(exit_x, EXITTEXT_Y, exitText);
}

/**
 * Handles the menu options
 */
enum MenuOption menu_handler(int mouse_x, int mouse_y){
    /* Singleplayer */
    if(mouse_y > SINGLEPLAYERTEXT_Y && mouse_y < SINGLEPLAYERTEXT_Y + singlePlayerText.height && mouse_x > singleplayer_x && mouse_x < singleplayer_x + singlePlayerText.width){
        /* Handles Click Event */
        return OPTION_SINGLEPLAYER;
    }

    /* Multiplayer */
    else if(mouse_y > MULTIPLAYERTEXT_Y && mouse_y < MULTIPLAYERTEXT_Y + multiPlayerText.height && mouse_x > multiplayer_x && mouse_x < multiplayer_x + multiPlayerText.width){
        /* Handles Click Event */
        return OPTION_MULTIPLAYER;
    }
    
    /* LeaderBoard */
    else if(mouse_y > LEADERBOARTEXT_Y && mouse_y < LEADERBOARTEXT_Y + leaderboardText.height && mouse_x > leaderboard_x && mouse_x < leaderboard_x + multiPlayerText.width){
        /* Handles Click Event */
        return OPTION_LEADERBOARD;
    }

    /* Exit */
    else if(mouse_y > EXITTEXT_Y && mouse_y < EXITTEXT_Y + exitText.height && mouse_x > exit_x && mouse_x < exit_x + exitText.width){
        /* Handles Click Event */
        return OPTION_EXIT;
    }
    
    /* None of the above */
    else {
        return OPTION_NONE;
    }
}
