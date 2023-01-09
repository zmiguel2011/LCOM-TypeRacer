#ifndef _LCOM_VIDEO_GR_H_
#define _LCOM_VIDEO_GR_H_

#include <lcom/lcf.h>

#define MAX_VALUE_WITH_BITS(n) ((1 << n) - 1)

#define RBG_GET_RED(n, rs, bs, gs) ((n >> (gs + bs)) & MAX_VALUE_WITH_BITS(rs))
#define RBG_GET_GREEN(n, gs, bs) ((n >> (bs)) & MAX_VALUE_WITH_BITS(gs))
#define RBG_GET_BLUE(n, bs) (n & MAX_VALUE_WITH_BITS(bs))

#define RBG_SET_COLOR(r, g, b, rs, gs, bs) (r << (bs + gs) | g << bs | b)

int get_xres();
int get_yres();

void* (vg_init)(uint16_t mode);

int (vg_draw_pixel)(uint16_t x, uint16_t y, uint32_t color);
int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int (vg_draw_xpm)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t* map);
int (vg_draw_xpm_secondary)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t* map);
void (vg_refresh)();

#endif
