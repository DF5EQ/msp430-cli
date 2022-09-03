#ifndef LED_H
#define LED_H

/* ===== file header ===== */

/* ===== includes ===== */
#include <msp430.h>
#include <stdint.h>

/* ===== public datatypes ===== */
typedef enum {
    LED_GREEN,
    LED_RED
} led_t;

/* ===== public symbols ===== */

/* ===== public constants ===== */

/* ===== public variables ===== */

/* ===== public functions ===== */
void led_init   (void);
void led_on     (led_t led);
void led_off    (led_t led);
void led_toggle (led_t led);

#endif

