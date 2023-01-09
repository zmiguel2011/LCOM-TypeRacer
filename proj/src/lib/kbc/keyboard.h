#ifndef _LCOM_KBD_H_
#define _LCOM_KBD_H_

#include <stdint.h>

/** @defgroup KBD
 * @{
 */

// Key Codes
#define KBD_ESC_SC 0x01
#define KBD_ESC_BC 0x81

#define KBD_A 30
#define KBD_B 48
#define KBD_C 46
#define KBD_D 32
#define KBD_E 18
#define KBD_F 33
#define KBD_G 34
#define KBD_H 35
#define KBD_I 23
#define KBD_J 36
#define KBD_K 37
#define KBD_L 38
#define KBD_M 50
#define KBD_N 49
#define KBD_O 24
#define KBD_P 25
#define KBD_Q 16
#define KBD_R 19
#define KBD_S 31
#define KBD_T 20
#define KBD_U 22
#define KBD_V 47
#define KBD_W 17
#define KBD_X 45
#define KBD_Y 21
#define KBD_Z 44
#define KBD_LEFT_SHIFT 0x36
#define KBD_RIGHT_SHIFT 0x2A
#define KBD_LEFT_SHIFT_BREAK BREAK_CODE(KBD_LEFT_SHIFT)
#define KBD_RIGHT_SHIFT_BREAK BREAK_CODE(KBD_RIGHT_SHIFT)
#define KBD_BACKSPACE 0x0E
#define KBD_CAPS_LOCK 0x3A
#define KBD_DOT 0x34
#define KBD_COMMA 0x33
#define KBD_QUESTION_MARK 0x0C
#define KBD_EXCLAMATION_MARK 0x02
#define KBD_SPACE 0x39

#define BREAK_CODE(n) (n | BIT(7))

// Helpers
#define KBD_IS_BREAK_CODE(n) (BIT(7) & n)
#define KBD_MULTIPLE_BYTES 0xE0

/**
 * Subscribes the KBD interrupts
 * @param bit_no Bit. no to be used on interrupts
 * @return 1 on eror 0 otherwise
 */
int(kbd_subscribe_int)(uint8_t *bit_no);
/**
 * Unsubscribes the KBD interrupts
 */
int(kbd_unsubscribe_int)();
/**
 * Application independent KBD interrupt handler
 */
void(kbc_ih)();

/**
 * Parses the actual scan code to char
 * @return Char with the key pressed, 0 if none pressed
 */
char kbd_get_key();

/**@}*/

#endif
