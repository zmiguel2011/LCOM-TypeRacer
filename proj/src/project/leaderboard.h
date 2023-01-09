#ifndef _LCOM_LEADERBOARD_H_
#define _LCOM_LEADERBOARD_H_

#include "../lib/rtc/rtc.h"

/** @defgroup Leaderboard
 * @{
 */

typedef struct LeaderboardEntry{
    int time;
    char content[64];
} LeaderboardEntry;

/**
 * Adds a new score to the leaderboard
 * @param score Time of the game
 * @param level Level played
 * @param date Current date
 * @return Rank the player got, -1 if out of top 10
 */
int leaderboard_add_new_score(int score, int level, Date date);

/**
 * Handles the leaderboard screen
 * @return 1 on error 0 otherwise
 */
int (leaderboard_main_loop)();

#endif
