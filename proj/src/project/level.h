#ifndef _LEVEL_H_
#define _LEVEL_H_

#include <lcom/lcf.h>

/** @defgroup Level
 * @{
 */

/**
 * @brief Reads and stores the whole contents of the file with filename into a char*
 * @param filename file path
 * @return Returns a pointer to the char array wtih the file contents (or NULL if there was an error reading the file contents)
 */
char *read_file(char *filename);
/**
 * @brief Reads and stores the level answers into a char**
 * @param filename file path
 * @return Returns a pointer to the char* array wtih the level answers (or NULL if there was an error reading the file contents)
 */
char **split_string(char *string, char *seperators, int *count);

#endif
