#include "video_gr.h"
#include "vbe.h"

#include <lcom/lcf.h>

static int XRes;
static int YRes;
static int BitsPerPixel;
static int ColorDepth;

static int BufferSize;

static uint8_t* video_mem;
static uint8_t* second_buffer;

static uint32_t* behindMouse;
static int cached_mouse_x;
static int cached_mouse_y;

#define MOUSE_W 19
#define MOUSE_H 22

/**
 * Returns the X resolution of the screen
 * @return X Resolution
 */
int get_XRes(){
    return XRes;
}

/**
 * Returns the Y resolution of the screen
 * @return Y Resolution
 */
int get_YRes(){
    return YRes;
}

/**
 * Initializes the graphics with the given mode
 * @param mode Graphics mode
 * @return Pointer to video mem buffer
 */
void* (vg_init)(uint16_t mode){
    // Get mode info
    vbe_mode_info_t info;
    if(vbe_get_mode_info(mode, &info)){
        printf("error: vge_get_mode_info\n");
        return NULL;
    }

    XRes = info.XResolution;
    YRes = info.YResolution;
    BitsPerPixel = info.BitsPerPixel;
    ColorDepth = BitsPerPixel / 8 + (BitsPerPixel % 8 != 0);

    // Allocates memory
    struct minix_mem_range mr; /* physical memory range */
    BufferSize = XRes * YRes * ColorDepth;

    mr.mr_base = info.PhysBasePtr;
    mr.mr_limit = mr.mr_base + BufferSize;

    if(sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)){
        panic("error: sys_privctl\n");
        return NULL;
    }

    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, BufferSize);
    if(video_mem == MAP_FAILED){
        panic("error: memory map");
        return NULL;
    }

    second_buffer = (uint8_t*)malloc(BufferSize);
    behindMouse = (uint32_t*)malloc(MOUSE_H * MOUSE_W * ColorDepth);

    // Set graphics mode
    reg86_t reg;
    memset(&reg, 0, sizeof(reg));

    reg.ax = VBE_FUNC_SET_MODE;
    reg.bx = VBE_LINEAR_MODE | mode;
    reg.intno = VBE_INT;

    if(sys_int86(&reg)){
        printf("error: sys_int86\n");
        return NULL;
    }

    if(reg.al != VBE_FUNC_SUPPORTED){
        printf("error: func not supported\n");
        return NULL;
    }

    if(reg.ah != VBE_FUNC_SUCCESS){
        printf("error: func failed\n");
        return NULL;
    }

    return video_mem;
}

/**
 * Returns a pointer to second buffer byte position
 * @param x X position
 * @param y Y position
 * @return Pointer to video. mem position
 */
uint8_t* getPosition(int x, int y){
    return second_buffer + (XRes * y + x) * ColorDepth;
} 

/**
 * Returns the current pixel of second memory buffer
 * @param x X position
 * @param y Y position
 * @return Byte color
 */
uint32_t* getCurrentPixel(int x, int y){
    return (uint32_t*)(second_buffer + (XRes * y + x) * ColorDepth);
} 

/**
 * Draws a pixel on the screen
 * @param x X position
 * @param y Y position
 * @param color Color of the pixel
 */
void vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color){
    if(x >= XRes || y >= YRes) return;
    memcpy(getPosition(x, y), &color, ColorDepth);
}

/**
 * Draws a xpm
 * @param x X position
 * @param y Y position
 * @param xpm XPM to draw
 * @return 1 on error 0 otherwise
 */
int (vg_draw_xpm)(uint16_t x, uint16_t y, xpm_image_t xpm){
  uint32_t* map = (uint32_t*)xpm.bytes;
  for(int i = 0; i < xpm.width; i++)
    for(int j = 0; j < xpm.height; j++)
        if(map[j * xpm.width + i] != 0xFF000000)
            vg_draw_pixel(x + i, y + j, map[j * xpm.width + i]);
    return 0;
}

/**
 * Draws a scaled xpm
 * @param x X position
 * @param y Y position
 * @param xpm XPM to draw
 * @param scale XPM scale
 * @return 1 on error 0 otherwise
 */
int (vg_draw_scaled_xpm)(uint16_t x, uint16_t y, xpm_image_t xpm, uint8_t scale){
  uint32_t* map = (uint32_t*)xpm.bytes;
  for(int i = 0; i < xpm.width / scale; i++)
    for(int j = 0; j < xpm.height / scale; j++)
        if(map[scale * (j * xpm.width + i)] != 0xFF000000)
            vg_draw_pixel(x + i, y + j, map[scale * (j * xpm.width + i)]);
    return 0;
}

/**
 * Draws a font xpm
 * @param x X position
 * @param y Y position
 * @param xpm XPM to draw
 * @param scale XPM scale
 * @param color Color of the letters
 * @return 1 on error 0 otherwise
 */
int vg_draw_font_xpm(uint16_t x, uint16_t y, xpm_image_t xpm, uint8_t scale, uint32_t color){
  uint32_t* map = (uint32_t*)xpm.bytes;
  for(int i = 0; i < xpm.width / scale; i++)
    for(int j = 0; j < xpm.height / scale; j++)
        if(map[scale * (j * xpm.width + i)] != 0xFF000000)
            vg_draw_pixel(x + i, y + j, color);
    return 0;
}

/**
 * Draws the mouse
 * The pixels behind it are stored
 * @param x X position
 * @param y Y position
 * @param xpm XPM to draw
 */
void vg_draw_mouse(uint16_t x, uint16_t y, xpm_image_t xpm){
    cached_mouse_x = x;
    cached_mouse_y = y;

    uint32_t* map = (uint32_t*)xpm.bytes;
    for(int i = 0; i < xpm.width; i++){
        for(int j = 0; j < xpm.height; j++){
            int index = j * xpm.width + i;
            behindMouse[index] = *getCurrentPixel(x + i, y + j);

            if(map[index] != 0xFF000000){
                vg_draw_pixel(x + i, y + j, map[index]);
            }
        }
    }
}

/**
 * Restores what was behind the mouse
 * (Deletes the mouse)
 */
void vg_restore_behind_mouse(){
    for(int i = 0; i < MOUSE_W; i++){
        for(int j = 0; j < MOUSE_H; j++){
            int index = j * MOUSE_W + i;

            if(behindMouse[index] != 0xFF000000){
                vg_draw_pixel(cached_mouse_x + i, cached_mouse_y + j, behindMouse[index]);
            }
        }
    }
}

/**
 * Clears the next frame buffer
 */
void vg_clear_next_frame(){
    memset(second_buffer, 0, BufferSize);
}

/**
 * Switches the video buffers
 */
void (vg_refresh)(){
    memcpy(video_mem, second_buffer, BufferSize);
}
