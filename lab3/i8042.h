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

#define KBC_IRQ 1

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

#define KBC_CMD_DIS2 BIT(5)
#define KBC_CMD_DIS BIT(4)
#define KBC_CMD_INT2 BIT(1)
#define KBC_CMD_INT BIT(0)

/*
    Others
*/

#define KBD_IS_BREAK_CODE(n) (BIT(7) & n)
#define KBD_MULTIPLE_BYTES 0xE0

#endif
