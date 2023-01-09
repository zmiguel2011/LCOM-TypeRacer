#include <lcom/lcf.h>

#include "config.h"
#include "devices.h"
#include "menu.h"
#include "singleplayer.h"
#include "dragDrop.h"
#include "leaderboard.h"

#include "../lib/graphics/font.h"
#include "../lib/graphics/xpm/xpm_loading.h"
#include "../lib/graphics/xpm/xpm_background.h"

/** @defgroup Main
 * @{
 */

/* Variables from anothers files */
extern int keyCode;
extern uint8_t mouse_filled_bytes;
extern int timer_irq, kbd_irq, mouse_irq;
extern Date date;

/* Global variables used by file */
static xpm_image_t mouse, background;
static int XRes, YRes;
extern int mouse_x, mouse_y;

static bool refreshFullScreen;
static bool refreshOnlyMouse;

int main(int argc, char *argv[]) {
  lcf_set_language("EN-US");

  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  if (lcf_start(argc, argv))
    return 1;

  return lcf_cleanup();
}

/**
 * Draws loading screen while the rest of game is loading
 */
void draw_loading_screen(){
  xpm_load((xpm_map_t) xpm_background, XPM_IMAGE_TYPE, &background);
  vg_draw_xpm(0, 0, background);

  xpm_image_t loading;
  xpm_load((xpm_map_t) xpm_loading, XPM_IMAGE_TYPE, &loading);

  vg_draw_xpm(250, 350, loading);
  vg_refresh();
}

/**
 * Initializes main screen
 */
static int init() {
  if (vg_init(GRAPHICS_MODE) == NULL)
    return 1;

  draw_loading_screen();

  xpm_init();
  menu_init();
  font_init();
  dragDrop_init();

  mouse = getXPM(MOUSE);
  XRes = get_XRes();
  YRes = get_YRes();
  mouse_x = get_XRes() / 2;
  mouse_y = get_YRes() / 2;
  refreshFullScreen = true;
  refreshOnlyMouse = false;

  return 0;
}

/**
 * Application Dependent Timer Handler
 */
static void timer_handler(){
  if(refreshFullScreen){
    vg_clear_next_frame();

    vg_draw_xpm(0, 0, background);
    vg_draw_scaled_xpm(462, 10, getXPM(MENU_CAR), 3);
    menu_draw();

    char datePhrase[32];
    snprintf(datePhrase, 32, "Date: %02d:%02d:%02d | %02d/%02d/20%02d", 
    date.hour, date.min, date.sec, date.day, date.month, date.year);
    font_draw_phrase(180, 785, datePhrase, WHITE);

    vg_draw_mouse(mouse_x, mouse_y, mouse);

    vg_refresh();
  }
  else if(refreshOnlyMouse){
    vg_restore_behind_mouse();
    vg_draw_mouse(mouse_x, mouse_y, mouse);
    vg_refresh();
  }

  refreshFullScreen = false;
  refreshOnlyMouse = false;
}

/**
 * Application Dependent Mouse Handler
 * @param exit Pointer to exit variable
 */
static void mouse_handler(bool* exit){
  if (mouse_filled_bytes == 3) {
    // Mouse moved
    refreshOnlyMouse = true;

    // Get packet
    struct packet p;
    if (mouse_get_packet(&p)) return;

    // Detect left click
    if (p.lb) {
      enum MenuOption option = menu_handler(mouse_x, mouse_y);
      switch (option) {
        case OPTION_SINGLEPLAYER:{
          rtc_toggle_int(false);
          singleplayer_main_loop();
          rtc_toggle_int(true);
          refreshFullScreen = true;
          break;
        }
        case OPTION_MULTIPLAYER:
          break;
        case OPTION_LEADERBOARD:{
          rtc_toggle_int(false);
          leaderboard_main_loop();
          rtc_toggle_int(true);
          refreshFullScreen = true;
          break;
        }
        case OPTION_EXIT:
          *exit = true;
          break;
        default:
          break;
      }
    }

    mouse_x += p.delta_x;
    mouse_y -= p.delta_y;

    if (mouse_x < 0) mouse_x = 0;
    if (mouse_y < 0) mouse_y = 0;
    if (mouse_x > XRes - 19) mouse_x = XRes - 6;
    if (mouse_y > YRes - 22) mouse_y = YRes - 8;
  }
}

/**
 * Project main loop
 * @param argc
 * @param argv
 */
int(proj_main_loop)(int argc, char *argv[]) {
  if (init()) return 1;
  if (subscribe_devices()) return 1;

  bool exit = false;
  uint32_t interrupts;

  while (!exit) {
    if (get_interrupts_vector(&interrupts))
      continue;

    // Handle Timer
    if (interrupts & timer_irq) {
      /* Application Independent */
      device_ih(TIMER0_IRQ);

      /* Application Dependent */
      timer_handler();
    }

    // Handle Keyboard
    if (interrupts & BIT(KEYBOARD_IRQ)) {
      /* Application Independent */
      device_ih(KEYBOARD_IRQ);

      /* Application Dependent */
      if(keyCode == KBD_ESC_SC)
        exit = true;
    }

    // Handle Mouse
    if (interrupts & BIT(MOUSE_IRQ)) {
      /* Application Independent */
      device_ih(MOUSE_IRQ);

      /* Application Dependent */
      mouse_handler(&exit);
    }

    // Handle RTC
    if (interrupts & BIT(RTC_IRQ)) {
      /* Application Independent */
      device_ih(RTC_IRQ);

      /* Application Dependent */
      refreshFullScreen = true;
    }
  }

  if (unsubscribe_devices()) return 1;
  return vg_exit();
}
