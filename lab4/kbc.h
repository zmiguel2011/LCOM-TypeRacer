#ifndef __KBD_H
#define __KBD_H

#include <stdint.h>


int(kbc_get_status)(uint8_t *st);
int(kbc_command)(uint8_t cmd);
int(kbc_mouse_command)(uint8_t arg);

#endif
