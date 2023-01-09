#ifndef _LCOM_DEVICES_H_
#define _LCOM_DEVICES_H_

#include <lcom/lcf.h>

#include "../lib/graphics/video_gr.h"
#include "../lib/graphics/xpm.h"
#include "../lib/kbc/keyboard.h"
#include "../lib/kbc/kbc.h"
#include "../lib/kbc/mouse.h"
#include "../lib/timer/timer.h"
#include "../lib/timer/i8254.h"
#include "../lib/rtc/rtc.h"

/** @defgroup Devices
 * @{
 */

/**
 * Subscribes all the devices
 * @return 1 on error 0 otherwise
 */
int subscribe_devices();
/**
 * Unsubscribes all the devices
 * @return 1 on error 0 otherwise
 */
int unsubscribe_devices();

/**
 * Handles the application independents interrupt handlers
 * @param irq IRQ Line of interrupt
 */
void device_ih(uint32_t irq);

/**
 * Returns the interrupts vector got by driver receive
 * @param vector Location to store result
 * @return 1 on error 0 otherwise
 */
int get_interrupts_vector(uint32_t* vector);

#endif
