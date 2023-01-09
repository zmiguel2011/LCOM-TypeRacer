#ifndef _LCOM_RTC_H_
#define _LCOM_RTC_H_

#include <lcom/lcf.h>

/** @defgroup RTC
 * @{
 */

#define RTC_IRQ 8

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71

#define RTC_REG_SEC 0
#define RTC_REG_SEC_ALARM 1
#define RTC_REG_MIN 2
#define RTC_REG_MIN_ALARM 3
#define RTC_REG_HOUR 4
#define RTC_REG_HOUR_ALARM 5
#define RTC_REG_WEEK_DAY 6
#define RTC_REG_MONTH_DAY 7
#define RTC_REG_MONTH 8
#define RTC_REG_YEAR 9
#define RTC_REG_A 10
#define RTC_REG_B 11
#define RTC_REG_C 12
#define RTC_REG_D 13

#define RTC_REG_A_UIP BIT(7)
#define RTC_REG_A_RS3 BIT(3)
#define RTC_REG_A_RS2 BIT(2)
#define RTC_REG_A_RS1 BIT(1)
#define RTC_REG_A_RS0 BIT(0)

#define RTC_REG_B_SET BIT(7)
#define RTC_REG_B_PIE BIT(6)
#define RTC_REG_B_AIE BIT(5)
#define RTC_REG_B_UIE BIT(4)
#define RTC_REG_B_SQWE BIT(3)
#define RTC_REG_B_DM BIT(2)
#define RTC_REG_B_24_12 BIT(1)
#define RTC_REG_B_DSE BIT(0)

#define RTC_REG_C_IRQF BIT(7)
#define RTC_REG_C_PF BIT(6)
#define RTC_REG_C_AF BIT(5)
#define RTC_REG_C_UF BIT(4)

#define RTC_REG_D_VRT BIT(7)

/**
 * Struct representing a Date
 */
typedef struct Date{
    uint8_t day, month, year;
    uint8_t sec, min, hour;
} Date;

/**
 * Subscribes the RTC interrupts
 * @param bit_no Bit. no to be used on interrupts
 * @return 1 on eror 0 otherwise
 */
int(rtc_subscribe_int)(uint8_t *bit_no);
/**
 * Unsubscribes the RTC interrupts
 */
int(rtc_unsubscribe_int)();
/**
 * Application independent RTC interrupt handler
 */
void rtc_ih();
/**
 * Toggle UIE RTC interrupts
 */ 
void rtc_toggle_int(bool enable);

/**
 * Initializes the RTC
 * @return 1 on error 0 otherwise
 */
int rtc_init();

/**
 * Reads a RTC register
 * @param register Register number
 * @param data Location to store result
 * @return 1 on error 0 otherwise
 */
int rtc_read(uint8_t register, uint8_t* data);
/**
 * Writes a RTC register
 * @param register Register number
 * @param data Byte to write
 * @return 1 on error 0 otherwise
 */
int rtc_write(uint8_t register, uint8_t data);

/**
 * Fills a Date global variable with the current date
 */ 
void rtc_get_date();

/**@}*/

#endif
