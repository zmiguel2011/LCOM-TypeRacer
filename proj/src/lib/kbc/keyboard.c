#include <lcom/lcf.h>

#include <ctype.h>

#include "keyboard.h"
#include "kbc.h"

uint8_t keyCode = 0;
static int hook_id = 0;

/**
 * Subscribes the KBD interrupts
 * @param bit_no Bit. no to be used on interrupts
 * @return 1 on eror 0 otherwise
 */
int(kbd_subscribe_int)(uint8_t *bit_no) {
  hook_id = *bit_no;
  return sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
}

/**
 * Unsubscribes the KBD interrupts
 */
int(kbd_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}

/**
 * Application independent KBD interrupt handler
 */
void(kbc_ih)() {
  uint8_t status, code;
  keyCode = 0;

  if(kbc_get_status(&status)) {
    printf("Couldn't get the KBC status\n");
    return;
  }

  if(!(status & KBC_ST_OUT_FULL)) {
    printf("KBC OUT buffer is empty\n");
    return;
  }

  if(util_sys_inb(KBC_OUT_PORT, &code)){
    printf("Couldn't get the KBC code\n");
    return;
  }

  if(status & KBC_ST_ERR) {
    printf("KBC Communication error\n");
    return;
  }

  keyCode = code;
}

/**
 * Parses the actual scan code to char
 * @return Char with the key pressed, 0 if none pressed
 */
char kbd_get_key(bool* shift){
  char c;
  switch (keyCode){
    case KBD_A: c = 'a'; break;
    case KBD_B: c = 'b'; break;
    case KBD_C: c = 'c'; break;
    case KBD_D: c = 'd'; break;
    case KBD_E: c = 'e'; break;
    case KBD_F: c = 'f'; break;
    case KBD_G: c = 'g'; break;
    case KBD_H: c = 'h'; break;
    case KBD_I: c = 'i'; break;
    case KBD_J: c = 'j'; break;
    case KBD_K: c = 'k'; break;
    case KBD_L: c = 'l'; break;
    case KBD_M: c = 'm'; break;
    case KBD_N: c = 'n'; break;
    case KBD_O: c = 'o'; break;
    case KBD_P: c = 'p'; break;
    case KBD_Q: c = 'q'; break;
    case KBD_R: c = 'r'; break;
    case KBD_S: c = 's'; break;
    case KBD_T: c = 't'; break;
    case KBD_U: c = 'u'; break;
    case KBD_V: c = 'v'; break;
    case KBD_W: c = 'w'; break;
    case KBD_X: c = 'x'; break;
    case KBD_Y: c = 'y'; break;
    case KBD_Z: c = 'z'; break;

    case KBD_SPACE: return ' ';
    case KBD_COMMA: return ','; 
    case KBD_DOT: return '.';
    
    case KBD_EXCLAMATION_MARK: return *shift ? '!' : 0;
    case KBD_QUESTION_MARK: return *shift ? '?' : '\'';

    case KBD_RIGHT_SHIFT:
    case KBD_LEFT_SHIFT: 
    case KBD_RIGHT_SHIFT_BREAK:
    case KBD_LEFT_SHIFT_BREAK: 
    case KBD_CAPS_LOCK:
      *shift = !*shift; return 0;
      
    default:
      return 0;
  }
  return *shift ? toupper((unsigned char)c) : c;
}
