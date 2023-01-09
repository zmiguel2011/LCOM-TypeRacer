#ifndef _LCOM_DRAGDROP_H_
#define _LCOM_DRAGDROP_H_

#include <lcom/lcf.h>

/** @defgroup Drag Drop
 * @{
 */

typedef struct Draggable{
    uint16_t original_x, original_y;
    uint16_t width, height;
    uint16_t x, y;
    bool beingDragged;
    bool fixed;
    char word[32];
} Draggable;

typedef struct DragTarget{
    uint16_t x, y;
    uint16_t width, height;
    bool filled;
} DragTarget;

/**
 * Initializes drag and drop
 */
void dragDrop_init();
/**
 * Reset draggable position to its original position
 * @param draggable Draggable in cause
 */
void resetDraggablePosition(Draggable* draggable);
/**
 * Handles the mouse drag
 * Changes the draggable position sync. with mouse
 * @param draggable Draggable in cause
 * @param mouse_delta_x Mouse x movement
 * @param mouse_delta_y Mouse y movement
 */
void handleDrag(Draggable* draggable, uint16_t mouse_delta_x, uint16_t mouse_delta_y);
/**
 * Check draggable bounds to start dragging
 * @param draggable Draggable in cause
 * @param mouse_x Mouse x position
 * @param mouse_y Mouse y position
 * @return True if drag started, false otherwise
 */
bool checkBounds(Draggable* dragabble, uint16_t mouse_x, uint16_t mouse_y);
/**
 * Draws the draggable on screen
 * @param draggable Draggable in cause
 */
void drawDraggable(Draggable* dragabble);
/**
 * Checks if draggable is on a target position
 * @param draggable Draggable in cause
 * @param targets Existing targets
 */
void checkTarget(Draggable* draggable, DragTarget* targets);

#endif
