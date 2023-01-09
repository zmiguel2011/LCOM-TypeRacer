#ifndef _LCOM_VIDEO_GR_H_
#define _LCOM_VIDEO_GR_H_

#include <lcom/lcf.h>

/** @defgroup VideoCard
 * @{
 */

#define BLACK 0
#define WHITE 0xFFFFFF
#define GREEN 0x00FF00
#define RED 0xFF0000
#define GRAY 0xD2D2D2

/**
 * Returns the X resolution of the screen
 * @return X Resolution
 */
int get_XRes();
/**
 * Returns the Y resolution of the screen
 * @return Y Resolution
 */
int get_YRes();

/**
 * Initializes the graphics with the given mode
 * @param mode Graphics mode
 * @return Pointer to video mem buffer
 */
void* (vg_init)(uint16_t mode);

/**
 * Draws a pixel on the screen
 * @param x X position
 * @param y Y position
 * @param color Color of the pixel
 */
void (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
/**
 * Draws a xpm
 * @param x X position
 * @param y Y position
 * @param xpm XPM to draw
 * @return 1 on error 0 otherwise
 */
int (vg_draw_xpm)(uint16_t x, uint16_t y, xpm_image_t xpm);
/**
 * Draws a scaled xpm
 * @param x X position
 * @param y Y position
 * @param xpm XPM to draw
 * @param scale XPM scale
 * @return 1 on error 0 otherwise
 */
int (vg_draw_scaled_xpm)(uint16_t x, uint16_t y, xpm_image_t xpm, uint8_t scale);
/**
 * Draws a font xpm
 * @param x X position
 * @param y Y position
 * @param xpm XPM to draw
 * @param scale XPM scale
 * @param color Color of the letters
 * @return 1 on error 0 otherwise
 */
int vg_draw_font_xpm(uint16_t x, uint16_t y, xpm_image_t xpm, uint8_t scale, uint32_t color);

/**
 * FUNCTIONS USED FOR DOUBLE BUFFERING AND MOUSE DRAWING ONLY
 */

/**
 * Switches the video buffers
 */
void (vg_refresh)();
/**
 * Clears the next frame buffer
 */
void vg_clear_next_frame();
/**
 * Draws the mouse
 * The pixels behind it are stored
 * @param x X position
 * @param y Y position
 * @param xpm XPM to draw
 */
void vg_draw_mouse(uint16_t x, uint16_t y, xpm_image_t xpm);
/**
 * Restores what was behind the mouse
 * (Deletes the mouse)
 */
void vg_restore_behind_mouse();

/**@}*/

#endif
