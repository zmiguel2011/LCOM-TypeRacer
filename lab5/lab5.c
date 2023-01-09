// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#include "i8042.h"
#include "kbd.h"
#include "timer.h"
#include "video_gr.h"

extern int keyCode;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (vg_init(mode) == NULL)
    return 1;

  sleep(delay);

  return vg_exit();
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color) {
  if (vg_init(mode) == NULL)
    return 1;

  vg_draw_rectangle(x, y, width, height, color);

  uint8_t bitNo = 0;
  uint8_t irq_set = BIT(bitNo);
  kbc_subscribe_int(&bitNo);

  int ipc_status;
  message msg;

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
          if (msg.m_notify.interrupts & irq_set)
            kbc_ih();
          break;
        default:
          break;
      }
    }
  }

  kbc_unsubscribe_int();
  return vg_exit();
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  if (vg_init(mode) == NULL)
    return 1;

  vbe_mode_info_t info;
  if (vbe_get_mode_info(mode, &info)) {
    printf("error: vge_get_mode_info\n");
    return 1;
  }

  uint32_t recWidth = info.XResolution / no_rectangles;
  uint32_t recHeight = info.YResolution / no_rectangles;

  uint32_t color;
  for (int x = 0; x < no_rectangles; x++) {
    for (int y = 0; y < no_rectangles; y++) {
      if (info.BitsPerPixel != 8) {
        color = RBG_SET_COLOR(
          (RBG_GET_RED(first, info.RedMaskSize, info.BlueMaskSize, info.GreenMaskSize) + x * step) % (1 << info.RedMaskSize),
          (RBG_GET_GREEN(first, info.GreenMaskSize, info.BlueMaskSize) + y * step) % (1 << info.GreenMaskSize),
          (RBG_GET_BLUE(first, info.BlueMaskSize) + (y + x) * step) % (1 << info.BlueMaskSize),
          info.RedMaskSize,
          info.GreenMaskSize,
          info.BlueMaskSize);
      }
      else
        color = (first + (y * no_rectangles + x) * step) % (1 << info.BitsPerPixel);

      if (vg_draw_rectangle(x * recWidth, y * recHeight, recWidth, recHeight, color))
        break;
    }
  }

  uint8_t bitNo = 0;
  uint8_t irq_set = BIT(bitNo);
  kbc_subscribe_int(&bitNo);

  int ipc_status;
  message msg;

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
          if (msg.m_notify.interrupts & irq_set)
            kbc_ih();
          break;
        default:
          break;
      }
    }
  }

  kbc_unsubscribe_int();
  return vg_exit();
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  if (vg_init(0x105) == NULL) {
    printf("vg_init error\n");
    return 1;
  }

  xpm_image_t image_info;
  uint8_t *pixmap = xpm_load(xpm, XPM_INDEXED, &image_info);
  if (pixmap == NULL) {
    printf("pixmap error\n");
    return 1;
  }

  for (int i = 0; i < image_info.width; i++) {
    for (int j = 0; j < image_info.height; j++) {
      vg_draw_pixel(x + i, y + j, pixmap[j * image_info.width + i]);
    }
  }

  uint8_t bitNo = 0;
  uint8_t irq_set = BIT(bitNo);
  kbc_subscribe_int(&bitNo);

  int ipc_status;
  message msg;

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
          if (msg.m_notify.interrupts & irq_set)
            kbc_ih();
          break;
        default:
          break;
      }
    }
  }

  kbc_unsubscribe_int();
  return vg_exit();
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate) {
  if (vg_init(0x105) == NULL) {
    printf("vg_init error\n");
    return 1;
  }

  xpm_image_t image_info;
  uint8_t *pixmap = xpm_load(xpm, XPM_INDEXED, &image_info);
  if (pixmap == NULL) {
    printf("pixmap error\n");
    return 1;
  }

  if(xf + image_info.width >= get_xres() || yf + image_info.height >= get_yres()){
    printf("overflow\n");
    return 1;
  }

  vg_draw_xpm(xi, yi, image_info.width, image_info.height, pixmap);

  int horizontal = xi == xf ? 0 : 1;
  int delay = speed < 0 ? speed : 0;

  if (timer_set_frequency(0, fr_rate))
    return 1;

  uint8_t k_bitNum = 1;
  kbc_subscribe_int(&k_bitNum);
  uint32_t k_irq_set = BIT(k_bitNum);

  uint8_t t_bitNum = 2;
  timer_subscribe_int(&t_bitNum);
  uint32_t t_irq_set = BIT(t_bitNum);

  int ipc_status;
  message msg;

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
          if (msg.m_notify.interrupts & k_irq_set)
            kbc_ih();
          if (msg.m_notify.interrupts & t_irq_set) {
            if (horizontal) {
              if (xi == xf)
                break;

              if (speed < 0) {
                if (++delay == 0) {
                  xi++;
                  delay = speed;
                }
              }
              else
                xi += speed;

              if (xi > xf)
                xi = xf;
            }
            else {
              if (yi == yf)
                break;

              if (speed < 0) {
                if (++delay == 0) {
                  yi++;
                  delay = speed;
                }
              }
              else
                yi += speed;

              if (yi > yf)
                yi = yf;
            }

            if(vg_draw_xpm_secondary(xi, yi, image_info.width, image_info.height, pixmap))
              printf("ERRRROR\n");
            vg_refresh();
          }
          break;
        default:
          break;
      }
    }
  }

  kbc_unsubscribe_int();
  timer_unsubscribe_int();
  return vg_exit();
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
