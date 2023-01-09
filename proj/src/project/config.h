#ifndef _LCOM_PROJ_CONFIG_H_
#define _LCOM_PROJ_CONFIG_H_

#include <lcom/lcf.h>

// Graphics Settings
#define GRAPHICS_MODE 0x14C
#define XPM_IMAGE_TYPE XPM_8_8_8_8

// Game Settings
typedef struct UserInput{
    int currentWordIndex;
    int rightCharacters;
    int wrongCharacters;
} UserInput;

#endif
