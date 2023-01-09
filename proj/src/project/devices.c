#include "../lib/timer/timer.h"
#include "../lib/timer/i8254.h"
#include "../lib/kbc/keyboard.h"
#include "../lib/kbc/kbc.h"
#include "../lib/kbc/mouse.h"

#include "devices.h"

/** @defgroup Devices
 * @{
 */

// For simplicity we used the bit. no equal to irq
static uint8_t timer_bitNo = TIMER0_IRQ;
static uint8_t kbd_bitNo = KEYBOARD_IRQ;
static uint8_t mouse_bitNo = MOUSE_IRQ;
static uint8_t rtc_bitNo = RTC_IRQ;

// Variables to be used by anothers files
int timer_irq;
int kbd_irq;
int mouse_irq;
int rtc_irq;

/**
 * Subscribes all the devices
 * @return 1 on error 0 otherwise
 */
int subscribe_devices(){
    // Timer
    if(timer_subscribe_int(&timer_bitNo)) return 1;
    // Keyboard
    if(kbd_subscribe_int(&kbd_bitNo)) return 1;
    // Mouse
    if(kbc_mouse_command(MOUSE_FUNC_ENABLE_DATA_REPORTING)) return 1;
    uint8_t ack;
    if(mouse_read_acknowledgment(&ack)) return 1;
    if(mouse_subscribe_int(&mouse_bitNo)) return 1;
    // RTC
    if(rtc_subscribe_int(&rtc_bitNo)) return 1;

    // Init. extern variables
    timer_irq = BIT(timer_bitNo);
    kbd_irq = BIT(kbd_bitNo);
    mouse_irq = BIT(mouse_bitNo);
    rtc_irq = BIT(rtc_bitNo);

    return 0;
}

/**
 * Unsubscribes all the devices
 * @return 1 on error 0 otherwise
 */
int unsubscribe_devices(){
    // Timer
    timer_unsubscribe_int();
    // Keyboard
    kbd_unsubscribe_int();
    // Mouse
    if(mouse_unsubscribe_int()) return 1;
    if(kbc_mouse_command(MOUSE_FUNC_DISABLE_DATA_REPORTING)) return 1;
    uint8_t ack;
    if(mouse_read_acknowledgment(&ack)) return 1;
    // RTC
    if(rtc_unsubscribe_int()) return 1;

    return 0;
}

/**
 * Handles the application independents interrupt handlers
 * @param irq IRQ Line of interrupt
 */
void device_ih(uint32_t irq){
    switch (irq){
        case TIMER0_IRQ:
            timer_int_handler();
            break;
        case KEYBOARD_IRQ:
            kbc_ih();
            break;
        case MOUSE_IRQ:
            mouse_ih();
            break;
        case RTC_IRQ:
            rtc_ih();
            break;
        default:
            break;
    }
}

/**
 * Returns the interrupts vector got by driver receive
 * @param vector Location to store result
 * @return 1 on error 0 otherwise
 */
int get_interrupts_vector(uint32_t* vector){
    // Initialize vector to 0
    *vector = 0;

    // Driver receive
    int ipc_status;
    message msg;
    if(driver_receive(ANY, &msg, &ipc_status) != 0) {
        printf("driver_receive failed.");
        return 1;
    }

    // Check for notifications
    if (is_ipc_notify(ipc_status)) { /* received notification */
        switch (_ENDPOINT_P(msg.m_source)) {
            case HARDWARE: /* hardware interrupt notification */
                *vector = msg.m_notify.interrupts;
                return 0;
            default: break; /* no other notifications expected: do nothing */
        }
    }

    // Found nothing
    return 0;
}
