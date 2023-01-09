#ifndef __KBD_H
#define __KBD_H

#include <stdint.h>

/**
 * @brief Subscribes and enables Keyboard interrupts
 *
 * @param bit_no address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbc_subscribe_int)(uint8_t *bit_no);

/**
 * @brief Unsubscribes Keyboard interrupts
 *
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbc_unsubscribe_int)();

/**
 * @brief Keyboard interrupt handler
 *
 */
void(kbc_ih)();

/**
 * @brief Reads the KBC status
 *
 * @param st    Address of memory position to be filled with the timer config
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbc_get_status)(uint8_t *st);

/**
 * @brief Executes a KBC Command
 *
 * @param st    Command to execute
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbc_command)(uint8_t cmd);

/**
 * @brief Executes a KBC Read command
 *
 * @param st    Variable to store the data read
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbc_read_command)(uint8_t *arg);

/**
 * @brief Executes a KBC Write command
 *
 * @param st    Value to write
 * @return Return 0 upon success and non-zero otherwise
 */
int(kbc_write_command)(uint8_t arg);

#endif
