#include <lcom/lcf.h>

#include <time.h>

#include "config.h"
#include "devices.h"
#include "dragDrop.h"
#include "endGame.h"
#include "leaderboard.h"

#include "../lib/graphics/font.h"
#include "level.h"

/** @defgroup Singleplayer
 * @{
 */

/* Variables from anothers files */
extern int keyCode;
extern uint8_t mouse_filled_bytes;
extern int timer_irq, kbd_irq, mouse_irq;
extern int mouse_x, mouse_y;

/* Global variables used by file */
static xpm_image_t road, lambo, mouse, ferrari, font_space, buffer, background;
static int XRes, YRes;

static bool refreshFullScreen;
static bool refreshOnlyMouse;

static char* paragraph;
static char** words;
static int numWords;
static char** answers;
static int answers_counter;
static char level_filepath[1024];
static char answers_filepath[1024];

#define D1_X 135
static struct Draggable *draggables;
static DragTarget *dragTargets;
static int dragActive;

static bool shiftOn;
#define BUFFER_SIZE 38
static char input[BUFFER_SIZE];
static int inputIndex;
static UserInput userInput;
static int nextChar;
static bool blocked;
static int nextTarget;

static bool gameDone;
static int gameStartCountdown;
#define SEMAPHORE_X 401
#define SEMAPHORE_Y 350

static int playerCar_x;
static int botCar_x;
static int delta_x;
static int botMoves;
static int timer_ticks;

extern Date date;
static int gameTime;
static int lvl;

#define NUM_LEVELS 4
#define BOT_MOVE_CHANGE 45

/**
 * Initializes the level
 * @param level Level number
 */
static void level_init(int level) {
  sprintf(level_filepath, "/home/lcom/labs/proj/src/project/levels/level%d.txt", level);
  sprintf(answers_filepath, "/home/lcom/labs/proj/src/project/levels/answers%d.txt", level);

  paragraph = read_file(level_filepath);
  char *answers_line = read_file(answers_filepath);
  answers = split_string(answers_line, " ,.", &answers_counter);
  words = split_string(paragraph, " [", &numWords);
  nextChar = 0;

  draggables = malloc(sizeof(Draggable) * answers_counter);

  for (int i = 0; i < answers_counter; i++) {
    draggables[i].original_x = D1_X + 323 * i;
    draggables[i].original_y = 315;
    draggables[i].width = 400;
    draggables[i].height = 50;
    draggables[i].x = D1_X + 323 * i;
    draggables[i].y = 310;
    draggables[i].beingDragged = false;
    draggables[i].fixed = false;
    strcpy(draggables[i].word, answers[i]);
  }

  dragActive = -1;
  dragTargets = malloc(sizeof(DragTarget) * answers_counter);

  for (int i = 0; i < answers_counter; i++) {
    dragTargets[i].x = 0;
    dragTargets[i].y = 0;
    dragTargets[i].width = 0;
    dragTargets[i].height = 0;
    dragTargets[i].filled = false;
  }
}

/**
 * Initializes screen
 */
static void init() {
  srand(time(NULL));

  XRes = get_XRes();
  YRes = get_YRes();

  road = getXPM(ROAD);
  lambo = getXPM(LAMBO);
  mouse = getXPM(MOUSE);
  ferrari = getXPM(FERRARI);
  font_space = getXPM(FONT_SPACE);
  buffer = getXPM(BUFFER);
  background = getXPM(BACKGROUND);

  refreshFullScreen = true;
  refreshOnlyMouse = false;
  shiftOn = false;
  inputIndex = 0;

  lvl = 1 + rand() % NUM_LEVELS;
  level_init(lvl);

  userInput.currentWordIndex = 0;
  userInput.rightCharacters = 0;
  userInput.wrongCharacters = 0;

  playerCar_x = 0;
  botCar_x = 0;
  delta_x = (road.width - ferrari.width) / numWords;
  timer_ticks = 0;
  botMoves = 0;

  gameDone = false;
  blocked = false;
  nextTarget = 0;

  gameStartCountdown = 4;
  gameTime = 0;

  rtc_toggle_int(true);
}

/**
 * Handler called every second
 */
static void every_second_handler(){
  gameTime++;

  int probability = rand() % 101;

  if(botMoves == numWords - 1){
    gameDone = true;
    end_game_main_loop(false, gameTime, -1);
  } else if(probability < BOT_MOVE_CHANGE){
    botCar_x += delta_x;
    botMoves++;
    refreshFullScreen = true;
  }
}

/**
 * Application Dependent Timer Handler
 */
static void timer_handler() {
  if(gameStartCountdown == 0){
    if(++timer_ticks == 60){
      timer_ticks = 0;
      every_second_handler();
    }

    if(blocked && dragTargets[nextTarget].filled){
      blocked = false;
      nextTarget++;
    }
  }

  if (refreshFullScreen) {
    vg_clear_next_frame();

    vg_draw_xpm(0, 0, background);

    font_draw_paragraph(25, 400, paragraph, dragTargets, &userInput);

    vg_draw_xpm(10, 750, buffer);
    font_draw_phrase(20, 750, input, BLACK);

    vg_draw_xpm(8, 20, road);
    vg_draw_xpm(playerCar_x, 50, lambo);
    vg_draw_xpm(botCar_x, 150, ferrari);

    for (int i = 0; i < answers_counter; i++)
      drawDraggable(&draggables[i]);

    if(gameStartCountdown > 0){
      switch (gameStartCountdown){      
      case 4:
        vg_draw_xpm(SEMAPHORE_X, SEMAPHORE_Y, getXPM(SEMAPHORE_NONE));
        break;
      case 3:
        vg_draw_xpm(SEMAPHORE_X, SEMAPHORE_Y, getXPM(SEMAPHORE_RED));
        break;
      case 2:
        vg_draw_xpm(SEMAPHORE_X, SEMAPHORE_Y, getXPM(SEMAPHORE_YELLOW));
        break;
      case 1:
        vg_draw_xpm(SEMAPHORE_X, SEMAPHORE_Y, getXPM(SEMAPHORE_GREEN));
        break;      
      default:
        break;
      }
    }

    vg_draw_mouse(mouse_x, mouse_y, mouse);

    vg_refresh();
  }
  else if (refreshOnlyMouse) {
    vg_restore_behind_mouse();
    vg_draw_mouse(mouse_x, mouse_y, mouse);
    vg_refresh();
  }

  refreshFullScreen = false;
  refreshOnlyMouse = false;
}

/**
 * Writing mechanic, advances empty spaces and draggables
 * if they are filled, otherwise block new input
 */
void advance_next_char(){
  do{
    if(paragraph[nextChar] == '['){
      if(!dragTargets[nextTarget].filled)
        blocked = true;
      else
        nextTarget++;
    }
    else if(paragraph[nextChar] != ' ')
      break;

    nextChar++;
  } while(true);
}

/**
 * Application Dependent KBD Handler
 */
static void kbd_handler(){
  if(blocked || gameStartCountdown > 0) return;

  if(keyCode == KBD_BACKSPACE){
    if(inputIndex > 0){
      if(userInput.wrongCharacters > 0)
        userInput.wrongCharacters--;
      else if(userInput.rightCharacters > 0){
        userInput.rightCharacters--;
        nextChar--;
      }

      input[--inputIndex] = '\0';
      refreshFullScreen = true;
    }
    return;
  }

  if(inputIndex == BUFFER_SIZE) return;

  char c = kbd_get_key(&shiftOn);
  if(c == 0) return;

  if(c == ' ' && !strcmp(input, words[userInput.currentWordIndex])){
    userInput.currentWordIndex++;
    userInput.rightCharacters = 0;
    playerCar_x += delta_x;
    userInput.wrongCharacters = 0;
    inputIndex = 0;
    advance_next_char();
    memset(input, '\0', BUFFER_SIZE);
    refreshFullScreen = true;

    if(paragraph[nextChar] == '\0'){
      gameDone = true;

      rtc_get_date();
      int rank = leaderboard_add_new_score(gameTime, lvl, date);

      end_game_main_loop(true, gameTime, rank);
    }

    return;
  }

  if(userInput.wrongCharacters == 0 && paragraph[nextChar] == c){
    userInput.rightCharacters++;
    nextChar++;
  }
  else
    userInput.wrongCharacters++;

  input[inputIndex++] = c;
  refreshFullScreen = true;
}

/**
 * Application Dependent Mouse Handler
 */
static void mouse_handler(bool *exit) {
  if (mouse_filled_bytes == 3) {
    // Mouse moved
    refreshOnlyMouse = true; // Can be overwrited by refreshFullScreen

    // Get packet
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

    if (p.lb && gameStartCountdown == 0) {
      if (dragActive == -1) {
        for (int i = 0; i < answers_counter; i++) {
          if (draggables[i].fixed)
            continue;

          bool startedDrag = checkBounds(&draggables[i], mouse_x, mouse_y);

          if (startedDrag) {
            dragActive = i;
            refreshFullScreen = true;
          }
        }
      }
      else {
        handleDrag(&draggables[dragActive], p.delta_x, p.delta_y);
        refreshFullScreen = true;
      }
    }
    else if (dragActive != -1) {
      checkTarget(&draggables[dragActive], &dragTargets[dragActive]);
      dragActive = -1;
      refreshFullScreen = true;
    }
  }
}

/**
 * Application Dependent RTC Handler
 */
void rtc_handler(){
  gameStartCountdown--;
  refreshFullScreen = true;

  if(gameStartCountdown == 0)
    rtc_toggle_int(false);
}

/**
 * Handles the singleplayer screen
 * @return 1 on error 0 otherwise
 */
int(singleplayer_main_loop)() {
  init();

  bool exit = false;
  uint32_t interrupts;

  while (keyCode != KBD_ESC_BC && !exit) {
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
      kbd_handler();

      if(gameDone)
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
      rtc_handler();
    }
  }

  memset(input, '\0', BUFFER_SIZE);
  if(draggables != NULL){
    free(draggables);
    draggables = NULL;
  }
  if (dragTargets != NULL) {
    free(dragTargets);
    dragTargets = NULL;
  }
  rtc_toggle_int(false);
  keyCode = 0; // For menu don't read esc too
  return 0;
}
