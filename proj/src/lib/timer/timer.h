#ifndef _LCOM_TIMER_H_
#define _LCOM_TIMER_H_

#include <lcom/lcf.h>

/** @defgroup Timer
 * @{
 */

/**
 * Modifies the timer frequency
 * @param timer Timer number
 * @param freq New frequency value
 * @return 1 on eror 0 otherwise
 */
int(timer_set_frequency)(uint8_t timer, uint32_t freq);
/**
 * Subscribes the timer interrupts
 * @param bit_no Bit. no to be used on interrupts
 * @return 1 on eror 0 otherwise
 */
int(timer_subscribe_int)(uint8_t *bit_no);
/**
 * Unsubscribes the timer interrupts
 */
int(timer_unsubscribe_int)();
/**
 * Aplication Independent timer interurpt handler
 */
void(timer_int_handler)();
/**
 * Returns the timer status
 * @param timer Timer number
 * @param st Location to store result
 * @return 1 on eror 0 otherwise
 */
int(timer_get_conf)(uint8_t timer, uint8_t *st);

/** @} */

#endif
