#include <lcom/lcf.h>

#include "i8042.h"
#include "kbd.h"

#define NUM_TRIES 20
#define DELAY 20000

uint8_t keyCode = 0;
static int hook_id = 0;

int(kbc_subscribe_int)(uint8_t *bit_no) {
  hook_id = *bit_no;
  return sys_irqsetpolicy(KBC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
}

int(kbc_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}

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

int(kbc_read_command)(uint8_t *arg) {
  // Signals KBC we wan't to read
  if (kbc_command(KBC_READ_CMD))
    return 1;

  uint8_t status;
  uint8_t tries = NUM_TRIES;

  // Read Command Byte
  while (tries-- > 0) {
    if (!kbc_get_status(&status) && (status & KBC_ST_OUT_FULL)) {
      if (status & KBC_ST_ERR)
        return 1;
      if (util_sys_inb(KBC_OUT_PORT, arg))
        return 1;
      return 0;
    }

    tickdelay(micros_to_ticks(DELAY));
  }
  return 1;
}

int(kbc_write_command)(uint8_t arg) {
  // Read current configuration of KBC
  // => Mantain others bits
  uint8_t config;
  if (kbc_read_command(&config))
    return 1;

  // Adds new flags
  config |= arg;

  // Signals KBC we will write
  if (kbc_command(KBC_WRITE_CMD))
    return 1;

  uint8_t status;
  uint8_t tries = NUM_TRIES;

  // Write new Command Byte
  while (tries-- > 0) {
    bool isInputBufferBusy = status & KBC_ST_IN_FULL;
    if (!kbc_get_status(&status) && !isInputBufferBusy)
      return sys_outb(KBC_IN_PORT, config);

    tickdelay(micros_to_ticks(DELAY));
  }
  return 1;
}
