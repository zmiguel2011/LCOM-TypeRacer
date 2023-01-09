#ifndef _MOUSE_H_
#define _MOUSE_H_

#include <stdint.h>

int(mouse_subscribe_int)(uint8_t *bit_no);
int(mouse_unsubscribe_int)();
void(mouse_ih)();
int(mouse_read_acknowledgment)(uint8_t* byte);
int (mouse_get_packet)(struct packet* p);

#endif
