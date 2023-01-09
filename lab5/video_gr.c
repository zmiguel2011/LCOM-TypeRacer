#include "video_gr.h"
#include "vbe.h"

#include <math.h>

static int XRes;
static int YRes;
static int BitsPerPixel;
static int ColorDepth;

static int BufferSize;

static uint8_t* video_mem;
static uint8_t* second_buffer;

int get_xres(){
    return XRes;
}

int get_yres(){
    return YRes;
}

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

uint8_t* getPosition(int x, int y){
    if(x > XRes || y > YRes){
        printf("error: invalid position\n");
        return NULL;
    }
    
    return video_mem + (XRes * y + x) * ColorDepth;
}

uint8_t* getPositionSecondary(int x, int y){
    if(x > XRes || y > YRes){
        printf("error: invalid position\n");
        return NULL;
    }
    
    return second_buffer + (XRes * y + x) * ColorDepth;
}

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color){
    if(x >= XRes || y >= YRes || memcpy(getPosition(x, y), &color, ColorDepth) == NULL)
        return 1;
    return 0;
}

int vg_draw_pixel_secondary(uint16_t x, uint16_t y, uint32_t color){
    if(x >= XRes || y >= YRes || memcpy(getPositionSecondary(x, y), &color, ColorDepth) == NULL)
        return 1;
    return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
    if(x + len > XRes){
        printf("error: invalid args\n");
        return 1;
    }

    for(int i = x; i < x + len; i++)
        if(vg_draw_pixel(i, y, color))
            return 1;

    return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
    uint32_t maxColor = pow(2, BitsPerPixel) - 1;
    if(color > maxColor){
        printf("error: invalid color (%x)\n", color);
        return 1;
    }

    for(int i = 0; i < height; i++)
        if(vg_draw_hline(x, y + i, width, color))
            printf("something went wrong\n");

    return 0;
}

int (vg_draw_xpm)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t* map){
  for(int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){
      if(vg_draw_pixel(x + i, y + j, map[j * width + i]))
        return 1;
    }
  }
  return 0;
}

int (vg_draw_xpm_secondary)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t* map){
  for(int i = 0; i < width; i++){
    for(int j = 0; j < height; j++){
      if(vg_draw_pixel_secondary(x + i, y + j, map[j * width + i]))
        return 1;
    }
  }
  return 0;
}

void (vg_refresh)(){
    memcpy(video_mem, second_buffer, BufferSize);
    memset(second_buffer, 0, BufferSize);
}
