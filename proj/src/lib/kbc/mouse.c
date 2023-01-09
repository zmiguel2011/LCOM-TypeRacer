#include <lcom/lcf.h>

#include "mouse.h"
#include "kbc.h"

static int hook_id = 0;
uint8_t mouse_bytes[3];
uint8_t mouse_filled_bytes = 0;

/**
 * Subscribes the mouse interrupts
 * @param bit_no Bit. no to be used on interrupts
 * @return 1 on eror 0 otherwise
 */
int(mouse_subscribe_int)(uint8_t *bit_no) {
  hook_id = *bit_no;
  return sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
}

/**
 * Unsubscribes the mouse interrupts
 */
int(mouse_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}

/**
 * Application independent mouse interrupt handler
 */
void(mouse_ih)() {
  uint8_t status;
  uint8_t mouse_byte = 0;

  if (kbc_get_status(&status)) {
    printf("Couldn't get the KBC status\n");
    return;
  }

  if (!(status & KBC_ST_OUT_FULL)) {
    printf("KBC OUT buffer is empty\n");
    return;
  }

  if(!(status & KBC_ST_MOUSE)){
      printf("Byte is not from mouse\n");
      return;
  }

  if (util_sys_inb(KBC_OUT_PORT, &mouse_byte)) {
    printf("Couldn't get the KBC byte\n");
    return;
  }

  if (status & KBC_ST_ERR) {
    mouse_byte = 0;
    printf("KBC Communication error\n");
    return;
  }

  if(mouse_filled_bytes == 0){
    if(mouse_byte & MB_1_VALID)
      mouse_bytes[mouse_filled_bytes++] = mouse_byte;
    else
      printf("invalid first byte\n");

    return;
  }

  mouse_bytes[mouse_filled_bytes++] = mouse_byte;
}

/**
 * Reads the mouse acknowlegment after a command
 * @param byte Location to store result
 * @return 1 on error 0 otherwise
 */
int (mouse_read_acknowledgment)(uint8_t* byte){
  return util_sys_inb(KBC_OUT_PORT, byte);
}

/**
 * Fills a packet struct with the information of the 3 bytes received
 * @param p Struct with packet information
 * @return 1 on error 0 otherwise
 */
int (mouse_get_packet)(struct packet* p){
  if(mouse_filled_bytes < 3) return 1;

  p->bytes[0] = mouse_bytes[0];
  p->bytes[1] = mouse_bytes[1];
  p->bytes[2] = mouse_bytes[2];

  p->lb = mouse_bytes[0] & MB_1_LEFT;
  p->rb = mouse_bytes[0] & MB_1_RIGHT;
  p->mb = mouse_bytes[0] & MB_1_MIDDLE;
  p->x_ov = mouse_bytes[0] & MB_1_X_OVER;
  p->y_ov = mouse_bytes[0] & MB_1_Y_OVER;

  int x_sign = ((mouse_bytes[0] & MB_1_X_SIGN) > 0) ? 1 : 0;
  int y_sign = ((mouse_bytes[0] & MB_1_Y_SIGN) > 0) ? 1 : 0;

  p->delta_x = (-1 * (x_sign << 8)) + mouse_bytes[1];
  p->delta_y = (-1 * (y_sign << 8)) + mouse_bytes[2];

  mouse_filled_bytes = 0;
  return 0;
}
