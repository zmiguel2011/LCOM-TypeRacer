#include <lcom/lcf.h>

#include "i8042.h"
#include "kbc.h"

#define NUM_TRIES 20
#define DELAY 20000

int(kbc_get_status)(uint8_t *st) {
  return util_sys_inb(KBC_STATUS_PORT, st);
}

int(kbc_command)(uint8_t cmd) {
  uint8_t status;
  uint8_t tries = NUM_TRIES;

  while (tries-- > 0) {
    if(!kbc_get_status(&status) && !(status & KBC_ST_IN_FULL))
        return sys_outb(KBC_CMD_PORT, cmd); 
    else
        tickdelay(micros_to_ticks(20000));
  }
  return 1;
}

int(kbc_mouse_command)(uint8_t arg){
  if (kbc_command(KBC_MOUSE_CMD))
    return 1;

  uint8_t status;
  uint8_t tries = NUM_TRIES;

  // Write new Command Byte
  while (tries-- > 0) {
    bool isInputBufferBusy = status & KBC_ST_IN_FULL;
    if (!kbc_get_status(&status) && !isInputBufferBusy)
      return sys_outb(KBC_IN_PORT, arg);

    tickdelay(micros_to_ticks(DELAY));
  }
  return 1;
}
