// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "mouse.h"
#include "kbc.h"
#include "i8042.h"

extern uint8_t mouse_filled_bytes;
extern int timer_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (mouse_test_packet)(uint32_t cnt) {
  int ipc_status;
  message msg;
  
  uint8_t bitNo = 6;
  uint32_t irq_set = BIT(bitNo);

  if(kbc_mouse_command(MOUSE_FUNC_ENABLE_DATA_REPORTING)){
    printf("reporting error\n");
    return 1;
  }

  uint8_t ack;
  if(mouse_read_acknowledgment(&ack)){
    printf("ack error\n");
    return 1;
  }
  printf("ACK: %x\n", ack);

  if(mouse_subscribe_int(&bitNo)){
    printf("subscribe int error\n");
    return 1;
  }

  uint16_t count = 0;
  while (count < cnt) {
    if (driver_receive(ANY, &msg, &ipc_status)) {
      printf("driver_receive failed");
      continue;
    }

    if (is_ipc_notify(ipc_status) && _ENDPOINT_P(msg.m_source) == HARDWARE) {
      if (msg.m_notify.interrupts & irq_set){
        mouse_ih();
        
        if(mouse_filled_bytes == 3){
          struct packet p;
          if(mouse_get_packet(&p)){
            printf("mouse_get_packet error\n");
            continue;
          }

          mouse_print_packet(&p);
          count++;
        }
      }
    }
  }

  if(mouse_unsubscribe_int()){
    printf("mouse_unsubscribe_int error\n");
    return 1;
  }

  if(kbc_mouse_command(MOUSE_FUNC_DISABLE_DATA_REPORTING)){
    printf("kbc_mouse_command error\n");
    return 1;
  }

  if(mouse_read_acknowledgment(&ack)){
    printf("ack error\n");
    return 1;
  }
  printf("ACK: %x\n", ack);

  return 0;
}

int (mouse_test_async)(uint8_t idle_time) {
  int ipc_status;
  message msg;
  
  uint8_t mouse_bitNo = 6;
  uint32_t mouse_irq_set = BIT(mouse_bitNo);

  uint8_t timer_bitNo = 3;
  uint32_t timer_irq_set = BIT(timer_bitNo);

  if(kbc_mouse_command(MOUSE_FUNC_ENABLE_DATA_REPORTING)){
    printf("reporting error\n");
    return 1;
  }

  uint8_t ack;
  if(mouse_read_acknowledgment(&ack)){
    printf("ack error\n");
    return 1;
  }
  printf("ACK: %x\n", ack);

  if(mouse_subscribe_int(&mouse_bitNo)){
    printf("subscribe int error\n");
    return 1;
  }

  if(timer_subscribe_int(&timer_bitNo)){
    printf("timer_subscribe_int error\n");
    return 1;
  }

  int freq = sys_hz();
  while (timer_counter < idle_time * freq) {
    if (driver_receive(ANY, &msg, &ipc_status)) {
      printf("driver_receive failed");
      continue;
    }

    if (is_ipc_notify(ipc_status) && _ENDPOINT_P(msg.m_source) == HARDWARE) {
      if (msg.m_notify.interrupts & timer_irq_set)
            timer_int_handler();

      if (msg.m_notify.interrupts & mouse_irq_set){
        timer_counter = 0;
        mouse_ih();
        
        if(mouse_filled_bytes == 3){
          struct packet p;
          if(mouse_get_packet(&p)){
            printf("mouse_get_packet error\n");
            continue;
          }

          mouse_print_packet(&p);
        }
      }
    }
  }

  if(timer_unsubscribe_int()){
    printf("timer_unsubscribe_int error\n");
    return 1;
  }

  if(mouse_unsubscribe_int()){
    printf("mouse_unsubscribe_int error\n");
    return 1;
  }

  if(kbc_mouse_command(MOUSE_FUNC_DISABLE_DATA_REPORTING)){
    printf("kbc_mouse_command error\n");
    return 1;
  }

  if(mouse_read_acknowledgment(&ack)){
    printf("ack error\n");
    return 1;
  }
  printf("ACK: %x\n", ack);

  return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
