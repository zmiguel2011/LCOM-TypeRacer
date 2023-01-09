#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

static int hook_id = 0;
int timer_counter = 0;

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

int(timer_subscribe_int)(uint8_t *bit_no) {
  hook_id = *bit_no;
  return sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id);
}

int(timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}

void(timer_int_handler)() {
  timer_counter++;
}

int(timer_get_conf)(uint8_t timer, uint8_t *st) {
  if(timer > 2) return 1;
  if(st == NULL) return 1;

  uint8_t word = TIMER_RB_CMD | TIMER_RB_SEL(timer) | TIMER_RB_COUNT_;

  if (sys_outb(TIMER_CTRL, word))
    return 1;
  return util_sys_inb(TIMER_0 + timer, st);
}

int(timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  union timer_status_field_val val;
  switch (field) {
    case tsf_all:
      val.byte = st;
      break;
    case tsf_base:
      val.bcd = TIMER_ST_BASE(st);
      break;
    case tsf_initial:
      val.in_mode = TIMER_ST_IN_MODE(st);
      break;
    case tsf_mode:{
      int countMode = TIMER_ST_COUNT_MODE(st);
      // Handle "don't care" bit of mode 2 and 3
      if(countMode > 5)
        val.count_mode = countMode - 4;
      else
        val.count_mode = countMode; 
      break;
    }
  }
  return timer_print_config(timer, field, val);
}
