#ifndef _TIMER_H_
#define _TIMER_H_

#include <lcom/lcf.h>

int(timer_set_frequency)(uint8_t timer, uint32_t freq);
int(timer_subscribe_int)(uint8_t *bit_no);
int(timer_unsubscribe_int)();
void(timer_int_handler)();
int(timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field);

#endif
