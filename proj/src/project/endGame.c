#include "endGame.h"
#include "devices.h"

#include "../lib/graphics/font.h"
#include "../lib/graphics/xpm.h"

/** @defgroup Endgame
 * @{
 */

extern int keyCode;
extern uint8_t mouse_filled_bytes;
extern int kbd_irq, mouse_irq;
extern int mouse_x, mouse_y;

static int XRes, YRes;
static xpm_image_t mouse, background;

static int _won, _gameTime, _rank;

/**
 * Initializes this screen
 */
static void init(){
  XRes = get_XRes();
  YRes = get_YRes();

  mouse = getXPM(MOUSE);
  background = getXPM(BACKGROUND);

  vg_draw_xpm(0, 0, background);

  if(_won){
    font_draw_phrase(450, 250, "You won!", WHITE);

    char phrase[64];

    snprintf(phrase, 64, "Score: %d seconds", _gameTime);
    font_draw_phrase(350, 310, phrase, WHITE);

    if(_rank == -1){
      font_draw_phrase(250, 370, "Not enough for top 10.", WHITE);
    } else{
      snprintf(phrase, 64, "You got rank: #%02d", _rank);
      font_draw_phrase(325, 370, phrase, WHITE);      
    }
  } else{
    font_draw_phrase(450, 300, "You lost!", WHITE);
  }

  font_draw_phrase(220, _won ? 430 : 370, "Press any key to continue", WHITE);
  vg_draw_mouse(mouse_x, mouse_y, mouse);
  vg_refresh();
}

/**
 * Application Dependent Mouse Handler
 * @param exit Pointer to exit variable
 */
static void mouse_handler(bool *exit) {
  if (mouse_filled_bytes == 3) {
    struct packet p;
    if (mouse_get_packet(&p))
      return;

    mouse_x += p.delta_x;
    mouse_y -= p.delta_y;

    if (mouse_x < 0)
      mouse_x = 0;
    if (mouse_y < 0)
      mouse_y = 0;
    if (mouse_x > XRes - 19)
      mouse_x = XRes - 6;
    if (mouse_y > YRes - 22)
      mouse_y = YRes - 8;

    vg_restore_behind_mouse();
    vg_draw_mouse(mouse_x, mouse_y, mouse);
    vg_refresh();
  }
}

/**
 * Application Dependent KBD Handler
 */
static bool kbd_handler(){
  return !KBD_IS_BREAK_CODE(keyCode);
}

/**
 * Handles the end game screen
 * @param won Player won
 * @param gameTime Game time
 * @param rank Player rank in case of win
 * @return 1 on error 0 otherwise
 */
int(end_game_main_loop)(int won, int gameTime, int rank) {
  _won = won;
  _gameTime = gameTime;
  _rank = rank;

  init();

  bool exit = false;
  uint32_t interrupts;

  while (keyCode != KBD_ESC_BC && !exit) {
      if (get_interrupts_vector(&interrupts))
          continue;

      // Handle Keyboard
      if (interrupts & BIT(KEYBOARD_IRQ)) {
          /* Application Independent */
          device_ih(KEYBOARD_IRQ);

          /* Application Dependent */
          if(kbd_handler())
              exit = true;
      }

      // Handle Mouse
      if (interrupts & BIT(MOUSE_IRQ)) {
          /* Application Independent */
          device_ih(MOUSE_IRQ);

          /* Application Dependent */
          mouse_handler(&exit);
      }
  }

  keyCode = 0; // For menu don't read esc too
  return 0;
}
