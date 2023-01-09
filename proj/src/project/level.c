#include <lcom/lcf.h>

#include "level.h"

/** @defgroup Level
 * @{
 */

/**
 * @brief Reads and stores the whole contents of the file with filename into a char*
 * @param filename file path
 * @return Returns a pointer to the char array wtih the file contents (or NULL if there was an error reading the file contents)
 */
char *read_file(char *filename)
{
  FILE *file;
  file = fopen(filename, "r");
  
  if (file == NULL) return NULL;
  
  fseek(file, 0, SEEK_END); 
  int length = ftell(file);
  fseek(file, 0, SEEK_SET);
  
  char *string = malloc(sizeof(char) * (length+1));
  
  char c;
  int i = 0;

  while ( (c = fgetc(file)) != EOF)
  {
    string[i] = c;
    i++;
  }

  string[i] = '\0';
  fclose(file);
  
  return string;
}

/**
 * @brief Reads and stores the level answers into a char**
 * @param filename file path
 * @return Returns a pointer to the char* array wtih the level answers (or NULL if there was an error reading the file contents)
 */
char **split_string(char *string, char *seperators, int *count)
{
  int len = strlen(string);
  *count = 0;
  
  int i = 0;
  while (i < len)
  {
    while (i < len)
    {
      if (strchr(seperators, string[i]) == NULL)
        break;
      i++;
    }
    
    int old_i = i;
    while (i < len)
    {
      if (strchr(seperators, string[i]) != NULL)
        break;
      i++;
    }

    if (i > old_i) *count = *count + 1;
  }
  
  char **strings = malloc(sizeof(char *) * *count);
  
  i = 0;
  char buffer[16384];
  int string_index = 0;
  while (i < len)
  {
    while (i < len)
    {
      if (strchr(seperators, string[i]) == NULL)
        break;
      i++;
    }
    
    int j = 0;
    while (i < len)
    {
      if (strchr(seperators, string[i]) != NULL)
        break;
      
      buffer[j] = string[i];
      i++;
      j++;
    }
    
    if (j > 0)
    {
      buffer[j] = '\0';
      int to_allocate = sizeof(char) * (strlen(buffer) + 1);

      strings[string_index] = malloc(to_allocate);
      strcpy(strings[string_index], buffer);
      string_index++;
    }
  }

  return strings;
}
