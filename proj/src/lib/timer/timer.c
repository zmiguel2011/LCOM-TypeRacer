#include <lcom/lcf.h>

#include <stdint.h>

#include "timer.h"
#include "i8254.h"

static int hook_id = 0;
int timer_counter = 0;

/**
 * Modifies the timer frequency
 * @param timer Timer number
 * @param freq New frequency value
 * @return 1 on eror 0 otherwise
 */
int(timer_set_frequency)(uint8_t timer, uint32_t freq) {
  // Check Parameters
  if(timer > 2) return 1;
  if(freq > TIMER_MAX_FREQ || freq < TIMER_MIN_FREQ) return 1;

  // Get current timer configuration
  uint8_t currConfig;
  if (timer_get_conf(timer, &currConfig))
    return 1;

  // Build new configuration
  uint8_t newConfig;
  // Mantain 4 least-significant bits
  newConfig = currConfig & TIMER_ST_4LSB;
  // Select timer
  if(timer == 1) newConfig |= TIMER_SEL1;
  else if(timer == 2) newConfig |= TIMER_SEL2;
  // Select Initialization Mode
  newConfig |= TIMER_LSB_MSB;

  // Write new configuration
  if(sys_outb(TIMER_CTRL, newConfig))
    return 1;

  // Build and write initial value
  uint16_t iValue = (uint16_t)(TIMER_FREQ / freq);
  uint8_t lsb, msb;
  if(util_get_LSB(iValue, &lsb) || util_get_MSB(iValue, &msb)) return 1;  
  return sys_outb(TIMER_0 + timer, lsb) || sys_outb(TIMER_0 + timer, msb);
}

/**
 * Subscribes the timer interrupts
 * @param bit_no Bit. no to be used on interrupts
 * @return 1 on eror 0 otherwise
 */
int(timer_subscribe_int)(uint8_t *bit_no) {
  hook_id = *bit_no;
  return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id);
}

/**
 * Unsubscribes the timer interrupts
 */
int(timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}

/**
 * Aplication Independent timer interurpt handler
 */
void(timer_int_handler)() {
  timer_counter++;
}

/**
 * Returns the timer status
 * @param timer Timer number
 * @param st Location to store result
 * @return 1 on eror 0 otherwise
 */
int(timer_get_conf)(uint8_t timer, uint8_t *st) {
  if(timer > 2) return 1;
  if(st == NULL) return 1;

  uint8_t word = TIMER_RB_CMD | TIMER_RB_SEL(timer) | TIMER_RB_COUNT_;

  if (sys_outb(TIMER_CTRL, word))
    return 1;
  return util_sys_inb(TIMER_0 + timer, st);
}
