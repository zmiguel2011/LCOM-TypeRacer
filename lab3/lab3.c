#include <lcom/lab3.h>
#include <lcom/lcf.h>

#include <stdbool.h>
#include <stdint.h>

#include "i8042.h"
#include "kbd.h"
#include "../lab2/timer.c"

extern uint8_t keyCode;
extern uint32_t inbCount;
extern int timer_counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  // Interrupts
  uint8_t bitNo = 0;
  uint8_t irq_set = BIT(bitNo);

  // Driver Receive
  int ipc_status;
  message msg;

  // Multiple Bytes Codes
  bool waitingByte = false;
  uint8_t bytes[2];
  bytes[0] = KBD_MULTIPLE_BYTES;

  kbc_subscribe_int(&bitNo);

  while (keyCode != KBD_ESC_BC) {
    // Get driver information
    if (driver_receive(ANY, &msg, &ipc_status)) {
      printf("driver_receive failed");
      continue;
    }

    // Handle information
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();

            if(keyCode == 0){
              if(waitingByte) waitingByte = false;
              continue;
            }

            if (keyCode == KBD_MULTIPLE_BYTES) {
              waitingByte = true;
              continue;
            }

            if(waitingByte){
              waitingByte = false;
              bytes[1] = keyCode;
              kbd_print_scancode(!KBD_IS_BREAK_CODE(keyCode), 2, bytes);
            } else{
              kbd_print_scancode(!KBD_IS_BREAK_CODE(keyCode), 1, &keyCode);
            }
          }
          break;
        default:
          break;
      }
    }
  }

  if(kbc_unsubscribe_int()) return 1;
  return kbd_print_no_sysinb(inbCount);
}

int(kbd_test_poll)() {
  uint8_t code = 0;
  uint8_t status;

  while (code != KBD_ESC_BC) {
    if (kbc_get_status(&status))
      continue;

    if (!(status & KBC_ST_OUT_FULL))
      continue;

    if (util_sys_inb(KBC_OUT_PORT, &code))
      continue;

    if (status & KBC_ST_ERR)
      continue;

    kbd_print_scancode(!KBD_IS_BREAK_CODE(code), 1, &code);
  }

  if(kbc_write_command(KBC_CMD_INT)) return 1;
  return kbd_print_no_sysinb(inbCount);
}

int(kbd_test_timed_scan)(uint8_t n) {
  int ipc_status;
  message msg;

  uint8_t k_bitNum = 1;
  kbc_subscribe_int(&k_bitNum);
  uint32_t k_irq_set = BIT(k_bitNum);

  uint8_t t_bitNum = 2;
  timer_subscribe_int(&t_bitNum);
  uint32_t t_irq_set = BIT(t_bitNum);

  bool waitingByte = false;
  uint8_t bytes[2];
  bytes[0] = KBD_MULTIPLE_BYTES;

  while (keyCode != KBD_ESC_BC && timer_counter < 60 * n) {
    if (driver_receive(ANY, &msg, &ipc_status) != 0) {
      printf("driver_receive failed");
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & t_irq_set)
            timer_int_handler();

          if (msg.m_notify.interrupts & k_irq_set) {
            timer_counter = 0;
            kbc_ih();

            if(keyCode == 0){
              if(waitingByte) waitingByte = false;
              continue;
            }

            if (keyCode == KBD_MULTIPLE_BYTES) {
              waitingByte = true;
              continue;
            }

            if(waitingByte){
              waitingByte = false;
              bytes[1] = keyCode;
              kbd_print_scancode(!KBD_IS_BREAK_CODE(keyCode), 2, bytes);
            } else{
              kbd_print_scancode(!KBD_IS_BREAK_CODE(keyCode), 1, &keyCode);
            }
          }
          break;

        default:
          break;
      }
    }
  }

  bool error = false;
  error |= kbc_unsubscribe_int();
  error |= timer_unsubscribe_int();
  error |= kbd_print_no_sysinb(inbCount);
  return error;
}
