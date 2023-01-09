#ifndef _LCOM_PROJ_ENDGAME_H
#define _LCOM_PROJ_ENDGAME_H

/** @defgroup EndGame
 * @{
 */

/**
 * Handles the end game screen
 * @param won Player won
 * @param gameTime Game time
 * @param rank Player rank in case of win
 * @return 1 on error 0 otherwise
 */
int (end_game_main_loop)(int won, int gameTime, int rank);

#endif
