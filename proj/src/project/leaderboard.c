#include "leaderboard.h"
#include "devices.h"
#include "../lib/graphics/font.h"

/** @defgroup Leaderboard
 * @{
 */

#define MAX_ENTRIES 10

extern int keyCode;
extern uint8_t mouse_filled_bytes;
extern int mouse_x, mouse_y;

static int XRes, YRes;
static xpm_image_t mouse, background;

static LeaderboardEntry entries[MAX_ENTRIES];
static int numEntriesFilled = 0;

/**
 * Reads leaderboard file and fills entries array
 */
void leaderboard_fill_entries(){
    numEntriesFilled = 0;

    FILE* file = fopen("/home/lcom/labs/proj/src/data/leaderboard.txt", "r");
    if(file == NULL) return;

    char line[64];
    memset(line, '\0', 64);

    int i = 0;
    while(i < 10 && fgets(line, 64, file)){
        strncpy(entries[numEntriesFilled].content, line, 64);
        char* time = strtok(line, " ");
        entries[numEntriesFilled].time = atoi(time);

        numEntriesFilled++;
        i++;
    }

    fclose(file);
}

/**
 * Adds a new score to the leaderboard
 * @param score Time of the game
 * @param level Level played
 * @param date Current date
 * @return Rank the player got, -1 if out of top 10
 */
int leaderboard_add_new_score(int score, int level, Date date){
    leaderboard_fill_entries();

    FILE* file = fopen("/home/lcom/labs/proj/src/data/leaderboard.txt", "w");
    if(file == NULL) return -1;

    int limit = 10;
    int written = 0;
    bool addedNewScore = false;
    int i = 0;

    int newRank = -1;

    while(written < limit){
      if(i < numEntriesFilled){
          if(!addedNewScore && score < entries[i].time){
              fprintf(file, "%02d | %02d | %02d:%02d:%02d | %02d/%02d/20%02d\n",
              score, level, date.hour, date.min, date.sec, date.day, date.month, date.year);
              written++;
              addedNewScore = true;
              newRank = i + 1;
          }

          fprintf(file, "%s", entries[i++].content);
          written++;
      } else if(!addedNewScore){
          fprintf(file, "%02d | %02d | %02d:%02d:%02d | %02d/%02d/20%02d\n",
              score, level, date.hour, date.min, date.sec, date.day, date.month, date.year);
          newRank = i + 1;
          break;
      }
    }

    fclose(file);
    return newRank;
}

/**
 * Initializes screen
 */
static void init(){
    XRes = get_XRes();
    YRes = get_YRes();

    mouse = getXPM(MOUSE);
    background = getXPM(BACKGROUND);

    leaderboard_fill_entries();

    vg_draw_xpm(0, 0, background);
    font_draw_phrase(400, 10, "LEADERBOARD", WHITE);
    font_draw_phrase_scale(35, 90, "Rank|Time|Lvl | Hour     | Date", RED, 3);
    for(int i = 0; i < numEntriesFilled; i++){
        char phrase[64];
        snprintf(phrase, 64, "#%02d | %s", i + 1, entries[i].content);

        font_draw_phrase_scale(35, 160 + 70 * i, phrase, i % 2 == 0 ? WHITE : GRAY, 3);
    }
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
 * Handles the leaderboard screen
 * @return 1 on error 0 otherwise
 */
int (leaderboard_main_loop)(){
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
