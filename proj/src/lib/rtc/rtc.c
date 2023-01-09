#include "rtc.h"

static int hook_id;
Date date;

/**
 * Initializes the RTC
 * @return 1 on error 0 otherwise
 */
int rtc_init(){
    if(rtc_write(RTC_REG_B, RTC_REG_B_24_12 | RTC_REG_B_UIE)) return 1;
    rtc_get_date();
    return 0;
}

/**
 * Application independent RTC interrupt handler
 */
void rtc_ih(){
    uint8_t regC;
    if(rtc_read(RTC_REG_C, &regC)) return;

    if (regC & RTC_REG_C_UF)
        rtc_get_date();
}

/**
 * Toggle UIE RTC interrupts
 */ 
void rtc_toggle_int(bool enable){
    uint8_t regB;
    if(rtc_read(RTC_REG_B, &regB)) return;

    if(enable) regB |= RTC_REG_B_UIE;
    else regB &= ~RTC_REG_B_UIE;

    rtc_write(RTC_REG_B, regB);
}

/**
 * Subscribes the RTC interrupts
 * @param bit_no Bit. no to be used on interrupts
 * @return 1 on eror 0 otherwise
 */
int(rtc_subscribe_int)(uint8_t *bit_no){
    rtc_init();

    hook_id = *bit_no;
    return sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id);
}

/**
 * Unsubscribes the RTC interrupts
 */
int(rtc_unsubscribe_int)(){
    return sys_irqrmpolicy(&hook_id);
}

/**
 * Reads a RTC register
 * @param register Register number
 * @param data Location to store result
 * @return 1 on error 0 otherwise
 */
int rtc_read(uint8_t reg, uint8_t* data){
    // Which register to read
    if(sys_outb(RTC_ADDR_REG, reg)) return 1;

    // Read register    
    if(util_sys_inb(RTC_DATA_REG, data)) return 1;

    // Success
    return 0;
}

/**
 * Writes a RTC register
 * @param register Register number
 * @param data Byte to write
 * @return 1 on error 0 otherwise
 */
int rtc_write(uint8_t reg, uint8_t data){
    // Which register to write
    if(sys_outb(RTC_ADDR_REG, reg)) return 1;

    // Write to register
    if(sys_outb(RTC_DATA_REG, data)) return 1;

    // Success
    return 0;
}

/**
 * Converts BCD number to decimal
 * @param bcd BCD number
 * @return Decimal representation
 */
uint8_t bcd_to_decimal(uint8_t bcd){
    return bcd - 6 * (bcd >> 4);
}

/**
 * Fills a Date global variable with the current date
 */ 
void rtc_get_date(){
    uint8_t regA;
    do {
        rtc_read(RTC_REG_A, &regA);
    } while (regA & RTC_REG_A_UIP);

    rtc_read(RTC_REG_MONTH_DAY, &date.day);
    rtc_read(RTC_REG_MONTH, &date.month);
    rtc_read(RTC_REG_YEAR, &date.year);
    rtc_read(RTC_REG_SEC, &date.sec);
    rtc_read(RTC_REG_MIN, &date.min);
    rtc_read(RTC_REG_HOUR, &date.hour);

    date.day = bcd_to_decimal(date.day);
    date.month = bcd_to_decimal(date.month);
    date.year = bcd_to_decimal(date.year);
    date.sec = bcd_to_decimal(date.sec);
    date.min = bcd_to_decimal(date.min);
    date.hour = bcd_to_decimal(date.hour);
}
