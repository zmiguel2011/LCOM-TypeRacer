#include <lcom/lcf.h>

#include <stdint.h>

/** @defgroup Utils
 * @{
 */

/**
 * Gets the least significant byte of a 16 bits value
 * @param val 16 bits value
 * @param lsb Location to store result
 * @return 1 on error 0 otherwise
 */
int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if(lsb == NULL) return 1;
  *lsb = (uint8_t)val;
  return 0;
}

/**
 * Gets the most significant byte of a 16 bits value
 * @param val 16 bits value
 * @param msb Location to store result
 * @return 1 on error 0 otherwise
 */
int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if(msb == NULL) return 1;
  *msb = (uint8_t)(val >> 8);
  return 0;
}

/**
 * Reads a byte from a port
 * @param port Port to read
 * @param value Location to store result
 * @return 1 on error 0 otherwise
 */
int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t v;
  if(sys_inb(port, &v)) return 1;
  *value = (uint8_t)v;
  return 0;
}

/**@}*/
