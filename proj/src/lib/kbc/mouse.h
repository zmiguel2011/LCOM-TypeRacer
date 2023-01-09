#ifndef _LCOM_MOUSE_H_
#define _LCOM_MOUSE_H_

#include <stdint.h>

/** @defgroup Mouse
 * @{
 */

#define MB_1_LEFT BIT(0)
#define MB_1_RIGHT BIT(1)
#define MB_1_MIDDLE BIT(2)
#define MB_1_VALID BIT(3)
#define MB_1_X_SIGN BIT(4)
#define MB_1_Y_SIGN BIT(5)
#define MB_1_X_OVER BIT(6)
#define MB_1_Y_OVER BIT(7)

#define MOUSE_FUNC_DISABLE_DATA_REPORTING 0xF5
#define MOUSE_FUNC_ENABLE_DATA_REPORTING 0xF4

/**
 * Subscribes the mouse interrupts
 * @param bit_no Bit. no to be used on interrupts
 * @return 1 on eror 0 otherwise
 */
int(mouse_subscribe_int)(uint8_t *bit_no);
/**
 * Unsubscribes the mouse interrupts
 */
int(mouse_unsubscribe_int)();
/**
 * Application independent mouse interrupt handler
 */
void(mouse_ih)();
/**
 * Reads the mouse acknowlegment after a command
 * @param byte Location to store result
 * @return 1 on error 0 otherwise
 */
int(mouse_read_acknowledgment)(uint8_t* byte);
/**
 * Fills a packet struct with the information of the 3 bytes received
 * @param p Struct with packet information
 * @return 1 on error 0 otherwise
 */
int(mouse_get_packet)(struct packet* p);

/**@}*/

#endif
