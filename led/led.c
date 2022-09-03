/* ===== file header ===== */

/* ===== includes ===== */
#include <msp430.h>
#include "led.h"

/* ===== private datatypes ===== */

/* ===== private symbols ===== */
#define LED_RED_OUT   P4OUT
#define LED_GREEN_OUT P1OUT

#define LED_RED_DIR   P4DIR
#define LED_GREEN_DIR P1DIR

#define LED_RED_BIT   BIT6
#define LED_GREEN_BIT BIT0

/* ===== private constants ===== */

/* ===== public constants ===== */

/* ===== private variables ===== */

/* ===== public variables ===== */

/* ===== private functions ===== */

/* ===== interrupt functions ===== */

/* ===== public functions ===== */
void led_init (void)
{
    /* initialise the leds */
    LED_RED_OUT    &= ~LED_RED_BIT;
    LED_RED_DIR    |=  LED_RED_BIT;
    LED_GREEN_OUT  &= ~LED_GREEN_BIT;
    LED_GREEN_DIR  |=  LED_GREEN_BIT;
}

void led_on (led_t led)
{
    switch (led)
    {
        case LED_RED:
            LED_RED_OUT |= LED_RED_BIT;
            break;
        case LED_GREEN:
            LED_GREEN_OUT |= LED_GREEN_BIT;
            break;
    }
}

void led_off (led_t led)
{
    switch (led)
    {
        case LED_RED:
            LED_RED_OUT &= ~LED_RED_BIT;
            break;
        case LED_GREEN:
            LED_GREEN_OUT &= ~LED_GREEN_BIT;
            break;
    }
}

void led_toggle (led_t led)
{
    switch (led)
    {
        case LED_RED:
            LED_RED_OUT ^= LED_RED_BIT;
            break;
        case LED_GREEN:
            LED_GREEN_OUT ^= LED_GREEN_BIT;
            break;
    }
}


