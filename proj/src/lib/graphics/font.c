#include <lcom/lcf.h>

#include "../../project/config.h"
#include "string.h"
#include "font.h"
#include "xpm.h"
#include "video_gr.h"

#include <stdio.h>
#include <stdlib.h>

#define NUM_LETTERS 96
#define ORIGINAL_CHAR_WIDTH 59
#define ORIGINAL_CHAR_HEIGHT 147

static xpm_image_t letters[NUM_LETTERS];
static xpm_image_t font_space;

static int XRes;

#define FONT_SCALE 2
#define SCALED_CHAR_WIDTH ORIGINAL_CHAR_WIDTH / FONT_SCALE
#define SCALED_CHAR_HEIGHT ORIGINAL_CHAR_HEIGHT / FONT_SCALE

/**
 * Reads a xpm from a file (XPM2)
 * @param path Path for the file
 * @return XPM read
 */
char** read_content(const char *path){
    FILE *f = fopen(path, "r");
    if(f == NULL) return NULL;

    size_t len = 1024; 
    char *line_buf = malloc(1024*sizeof(char));
    ssize_t sz;
    char **ret = NULL;
    unsigned w, h, num_colors, chars_per_pixel;

    sz = getline(&line_buf, &len, f);{
        sscanf(line_buf, "%d %d %d %d", &w, &h, &num_colors, &chars_per_pixel);
        ret = malloc((1+num_colors+h)*sizeof(char*));
    }

    ret[0] = malloc(((unsigned)sz+1)*sizeof(char)); 
    if(ret[0] == NULL){ 
        free(ret); 
        return NULL; 
    }
    strcpy(ret[0], line_buf);

    for(size_t i = 1; i < 1+num_colors+h; ++i){
        sz = getline(&line_buf, &len, f);
        ret[i] = malloc(((unsigned)sz+1)*sizeof(char));
        if(ret[i] == NULL){
            for(size_t j = 0; j < i; ++j)
                free(ret[i]);
            free(ret);
            return NULL;
        }
        strcpy(ret[i], line_buf);
        if(ret[i][sz-2] == '\r') ret[i][sz-2] = '\0';
        ret[i][sz-1] = '\0';
    }

    fclose(f); 
    f = NULL;

    free(line_buf); 
    line_buf = NULL;
    return ret;
}

/**
 * Initializes the font
 */
void font_init(){
    font_space = getXPM(FONT_SPACE);

    // Loads all letters into a xmp //
    char filepath[2048];
    for(int i = 32; i <= 127; i++) {
        sprintf(filepath, "/home/lcom/labs/proj/src/lib/graphics/font/ascii%03d.xpm2", i);

        char** content = read_content(filepath);
        if(content == NULL) printf("Failed to load ascii: %03d\n", i);

        xpm_load((xpm_map_t)content, XPM_IMAGE_TYPE, &letters[i - 32]);
    }

    XRes = get_XRes();
}

/**
 * Converts a char into a XPM
 * @param c Char
 * @return XPM of the char received
 */
xpm_image_t font_get_char(char c){
    return letters[c - 32];
}

/**
 * Draws a phrase
 * @param x X position
 * @param y Y position
 * @param phrase Array containing the phrase
 * @param color Color of the letters
 */ 
void font_draw_phrase(int x, int y, char* phrase, uint32_t color){
    for(size_t i = 0; i < strlen(phrase); i++)
        vg_draw_font_xpm(x + SCALED_CHAR_WIDTH * i, y, font_get_char(phrase[i]), FONT_SCALE, color);
}

/**
 * Draws a phrase scaled
 * @param x X position
 * @param y Y position
 * @param phrase Array containing the phrase
 * @param color Color of the letters
 * @param scale Scale of letters (2 is half size)
 */ 
void font_draw_phrase_scale(int x, int y, char* phrase, uint32_t color, int scale){
    for(size_t i = 0; i < strlen(phrase); i++)
        vg_draw_font_xpm(x + SCALED_CHAR_WIDTH * i, y, font_get_char(phrase[i]), scale, color);
}

/**
 * Function used for main gameplay.
 * The color of the letters depends on the user input.
 * @param x X position
 * @param y Y position
 * @param paragraph Array containing the text
 * @param target Array with the draggables
 * @param userInput Struct with info. about user input
 */
void font_draw_paragraph(int x, int y, char* paragraph, DragTarget* targets, UserInput* userInput){
    bool fillTargets = targets[0].width == 0;
    uint8_t targetIndex = 0;

    int nextX = x;
    int nextY = y;
    int sizeOfNextWord = 0;

    char nextWord[32];
    memset(&nextWord, '\0', 32);
    int charIndex = 0;

    int wordIndex = 0;
    int rightChars = userInput->rightCharacters;
    int wrongChars = userInput->wrongCharacters;

    for(size_t i = 0; i < strlen(paragraph); i++) {
        if(paragraph[i] == ' '){
            sizeOfNextWord = charIndex * SCALED_CHAR_WIDTH;

            if(nextX + sizeOfNextWord > XRes){
                nextY += SCALED_CHAR_HEIGHT + 5;
                nextX = x;
            }

            for(int j = 0; j < charIndex; j++){
                uint32_t color;

                // If the word is already done
                if(wordIndex < userInput->currentWordIndex){
                    color = GREEN;
                }
                // We have correct chars
                else if(rightChars > 0){
                    rightChars--;
                    color = GREEN;
                }
                else if(wrongChars > 0){
                    wrongChars--;
                    color = RED;
                }
                else
                    color = WHITE;

                vg_draw_font_xpm(nextX, nextY, font_get_char(nextWord[j]), FONT_SCALE, color);
                nextX += SCALED_CHAR_WIDTH;
            }

            nextX += SCALED_CHAR_WIDTH;

            memset(&nextWord, '\0', 32);
            charIndex = 0;
            wordIndex++;
        }
        else if (paragraph[i] == '[') {
            if(nextX + font_space.width > XRes){
                nextY += SCALED_CHAR_HEIGHT + 5;
                nextX = x;
            }

            if(fillTargets){
                targets[targetIndex].x = nextX;
                targets[targetIndex].y = nextY;
                targets[targetIndex].width = font_space.width;
                targets[targetIndex++].height = font_space.height;
            }

            if(!targets[targetIndex].filled)
                vg_draw_xpm(nextX, nextY, font_space);

            if(!fillTargets)
                targetIndex++;

            nextX += font_space.width;
            wordIndex--;
        }
        else {
            nextWord[charIndex++] = paragraph[i];
        }
    }

    sizeOfNextWord = charIndex * SCALED_CHAR_WIDTH;
    if(nextX + sizeOfNextWord > XRes){
        nextY += SCALED_CHAR_HEIGHT + 5;
        nextX = x;
    }

    for(int j = 0; j < charIndex; j++){
        uint32_t color;

        // If the word is already done
        if(wordIndex < userInput->currentWordIndex){
            color = GREEN;
        }
        // We have correct chars
        else if(rightChars > 0){
            rightChars--;
            color = GREEN;
        }
        else if(wrongChars > 0){
            wrongChars--;
            color = RED;
        }
        else
            color = WHITE;

        vg_draw_font_xpm(nextX, nextY, font_get_char(nextWord[j]), FONT_SCALE, color);
        nextX += SCALED_CHAR_WIDTH;
    }
}

/**
 * Function to draw the date
 * @param x X position
 * @param y Y position
 * @param date Actual date
 * @param color Color of the letters
 */
void font_draw_date(int x, int y, Date date, uint32_t color){
    vg_draw_font_xpm(x + SCALED_CHAR_WIDTH, y, font_get_char(date.day), FONT_SCALE, color);
    vg_draw_font_xpm(x + SCALED_CHAR_WIDTH * 1, y, font_get_char(date.month), FONT_SCALE, color);
    vg_draw_font_xpm(x + SCALED_CHAR_WIDTH * 2, y, font_get_char(date.year), FONT_SCALE, color);
    vg_draw_font_xpm(x + SCALED_CHAR_WIDTH * 3, y, font_get_char(date.sec), FONT_SCALE, color);
    vg_draw_font_xpm(x + SCALED_CHAR_WIDTH * 4, y, font_get_char(date.min), FONT_SCALE, color);
    vg_draw_font_xpm(x + SCALED_CHAR_WIDTH * 5, y, font_get_char(date.hour), FONT_SCALE, color);
}
