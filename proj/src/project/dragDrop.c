#include "dragDrop.h"

#include "../lib/graphics/video_gr.h"
#include "../lib/graphics/xpm.h"
#include "../lib/graphics/font.h"

/** @defgroup DragDrop
 * @{
 */

static xpm_image_t font_space;
static xpm_image_t font_space_done;

#define TOLERANCE 100

/**
 * Initializes drag and drop
 */
void dragDrop_init(){
    font_space = getXPM(FONT_SPACE);
    font_space_done = getXPM(FONT_SPACE_DONE);
}

/**
 * Reset draggable position to its original position
 * @param draggable Draggable in cause
 */
void resetDraggablePosition(Draggable* draggable){
    draggable->x = draggable->original_x;
    draggable->y = draggable->original_y;
    draggable->beingDragged = false;
}

/**
 * Handles the mouse drag
 * Changes the draggable position sync. with mouse
 * @param draggable Draggable in cause
 * @param mouse_delta_x Mouse x movement
 * @param mouse_delta_y Mouse y movement
 */
void handleDrag(Draggable* draggable, uint16_t mouse_delta_x, uint16_t mouse_delta_y){
    draggable->x += mouse_delta_x;
    draggable->y -= mouse_delta_y;
}

/**
 * Check draggable bounds to start dragging
 * @param draggable Draggable in cause
 * @param mouse_x Mouse x position
 * @param mouse_y Mouse y position
 * @return True if drag started, false otherwise
 */
bool checkBounds(Draggable* dragabble, uint16_t mouse_x, uint16_t mouse_y){
    if(mouse_x > dragabble->original_x 
    && mouse_x < dragabble->original_x + dragabble->width
    && mouse_y > dragabble->original_y
    && mouse_y < dragabble->original_y + dragabble->height){
        dragabble->beingDragged = true;
    }
    return dragabble->beingDragged;
}

/**
 * Draws the draggable on screen
 * @param draggable Draggable in cause
 */
void drawDraggable(Draggable* dragabble){
    vg_draw_xpm(dragabble->x, dragabble->y, dragabble->fixed ? font_space_done : font_space);
    font_draw_phrase(dragabble->x + (font_space.width - 29 * strlen(dragabble->word)) / 2, dragabble->y, dragabble->word, WHITE);
}

/**
 * Checks if draggable is on a target position
 * @param draggable Draggable in cause
 * @param targets Existing targets
 */
void checkTarget(Draggable* draggable, DragTarget* target){
    if((target->x - TOLERANCE) < draggable->x 
    && (target->x + TOLERANCE) > draggable->x
    && (target->y - TOLERANCE) < draggable->y
    && (target->y + TOLERANCE) > draggable->y){
        draggable->fixed = true;
        draggable->beingDragged = false;
        target->filled = true;

        draggable->x = target->x;
        draggable->y = target->y;
    } else{
        resetDraggablePosition(draggable);
    }
}
