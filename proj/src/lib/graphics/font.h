#ifndef _LCOM_FONT_H_
#define _LCOM_FONT_H_

#include "../../project/dragDrop.h"
#include "../../project/config.h"
#include "../rtc/rtc.h"

/** @defgroup Font
 * @{
 */

/**
 * Initializes the font
 */
void font_init();
/**
 * Draws a phrase
 * @param x X position
 * @param y Y position
 * @param phrase Array containing the phrase
 * @param color Color of the letters
 */ 
void font_draw_phrase(int x, int y, char* phrase, uint32_t color);
/**
 * Draws a phrase scaled
 * @param x X position
 * @param y Y position
 * @param phrase Array containing the phrase
 * @param color Color of the letters
 * @param scale Scale of letters (2 is half size)
 */ 
void font_draw_phrase_scale(int x, int y, char* phrase, uint32_t color, int scale);
/**
 * Function used for main gameplay.
 * The color of the letters depends on the user input.
 * @param x X position
 * @param y Y position
 * @param paragraph Array containing the text
 * @param target Array with the draggables
 * @param userInput Struct with info. about user input
 */
void font_draw_paragraph(int x, int y, char* paragraph, DragTarget* targets, UserInput* userInput);
/**
 * Function to draw the date
 * @param x X position
 * @param y Y position
 * @param date Actual date
 * @param color Color of the letters
 */
void font_draw_date(int x, int y, Date date, uint32_t color);

/**@}*/

#endif
