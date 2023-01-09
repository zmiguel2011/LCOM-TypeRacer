#include <lcom/lcf.h>

#include "kbc.h"

#define NUM_TRIES 20
#define DELAY 20000

/**
 * Gets the status of the KBD
 * @param arg Location to store result
 * @return 1 on error 0 otherwise
 */ 
int(kbc_get_status)(uint8_t *st) {
  return util_sys_inb(KBC_STATUS_PORT, st);
}

/**
 * Performs a KBD Command
 * @param cmd Command to perform
 * @return 1 on error 0 otherwise
 */
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

/**
 * Performs the KBC read command
 * @param arg Location to store result
 * @return 1 on error 0 otherwise
 */
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

/**
 * Performs the KBC write command
 * @param arg Byte to write
 * @return 1 on error 0 otherwise
 */
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

/**
 * Sends a command directly to mouse
 * @param arg Mouse command
 * @return 1 on error 0 otherwise
 */
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
