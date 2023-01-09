#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>

/*
    Ports
*/

#define KBC_IN_PORT 0x60
#define KBC_OUT_PORT 0x60

#define KBC_STATUS_PORT 0x64
#define KBC_CMD_PORT 0x64

/*
    Interrupt Line
*/

#define KBD_IRQ 1
#define MOUSE_IRQ 12

/*
    Status Register
*/

#define KBC_ST_ERR (BIT(7) | BIT(6))
#define KBC_ST_MOUSE BIT(5)
#define KBC_ST_IN_FULL BIT(1)
#define KBC_ST_OUT_FULL BIT(0)

/*
    Keys Codes
*/

#define KBD_ESC_SC 0x01
#define KBD_ESC_BC 0x81

/*
    KBC Commands
*/

#define KBC_READ_CMD 0x20
#define KBC_WRITE_CMD 0x60
#define KBC_MOUSE_CMD 0xD4

#define KBC_CMD_DIS2 BIT(5)
#define KBC_CMD_DIS BIT(4)
#define KBC_CMD_INT2 BIT(1)
#define KBC_CMD_INT BIT(0)

/*
    Others
*/

#define KBD_IS_BREAK_CODE(n) (BIT(7) & n)
#define KBD_MULTIPLE_BYTES 0xE0

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

#endif
