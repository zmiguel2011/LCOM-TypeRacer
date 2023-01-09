#ifndef _LCOM_KBC_H_
#define _LCOM_KBC_H_

#include <stdint.h>

/** @defgroup KBC
 * @{
 */

// IRQ Lines
#define KBD_IRQ 1
#define MOUSE_IRQ 12

// KBC Ports
#define KBC_IN_PORT 0x60
#define KBC_OUT_PORT 0x60
#define KBC_STATUS_PORT 0x64
#define KBC_CMD_PORT 0x64

// KBC Status Bits
#define KBC_ST_ERR (BIT(7) | BIT(6))
#define KBC_ST_MOUSE BIT(5)
#define KBC_ST_IN_FULL BIT(1)
#define KBC_ST_OUT_FULL BIT(0)

// KBC Commands
#define KBC_READ_CMD 0x20
#define KBC_WRITE_CMD 0x60
#define KBC_MOUSE_CMD 0xD4

// KBC Command Bits
#define KBC_CMD_DIS2 BIT(5)
#define KBC_CMD_DIS BIT(4)
#define KBC_CMD_INT2 BIT(1)
#define KBC_CMD_INT BIT(0)

/**
 * Gets the status of the KBD
 * @param arg Location to store result
 * @return 1 on error 0 otherwise
 */ 
int(kbc_get_status)(uint8_t *arg);
/**
 * Performs the KBC read command
 * @param arg Location to store result
 * @return 1 on error 0 otherwise
 */
int(kbc_read_command)(uint8_t *arg);
/**
 * Performs the KBC write command
 * @param arg Byte to write
 * @return 1 on error 0 otherwise
 */
int(kbc_write_command)(uint8_t arg);
/**
 * Sends a command directly to mouse
 * @param arg Mouse command
 * @return 1 on error 0 otherwise
 */
int(kbc_mouse_command)(uint8_t arg);

/**@}*/

#endif
