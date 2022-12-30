/* ===== file header ===== */
/*
 * Copyright (C) 2018  nhivp
 *               2022  Peter Bägel (DF5EQ)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* ===== includes ===== */
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "uart.h"
#include "line.h"

/* ===== private datatypes ===== */
typedef enum {
    RX_RUNNING,
    RX_ESC,         /* 0x1b (escape) reveived */
    RX_ESC_SBO,     /* 0x1b (escape) and 0x5b (square bracket open) reveived */
    RX_ESC_SBO_TILDE,
    RX_FULL
} rx_state_t;

/* ===== private symbols ===== */
#define NUL 0x00
#define BEL 0x07
#define BS  0x08
#define LF  0x0a
#define CR  0x0d
#define ESC 0x1b

#define CURSOR_RIGHT      "\x1b[C"
#define CURSOR_LEFT       "\x1b[D"
#define CURSOR_SAVE       "\x1b[s"
#define CURSOR_RESTORE    "\x1b[u"
#define DELETE_TO_LINEEND "\x1b[0K"

/* ===== private constants ===== */

/* ===== public constants ===== */

/* ===== private variables ===== */
static char rx_buffer[UART_RX_BUFFER_SIZE];
static char rx_buffer_index;
static rx_state_t rx_state;
static int last_char;

/* ===== public variables ===== */

/* ===== private functions ===== */

/* ===== interrupt functions ===== */
#pragma vector = USCI_A0_VECTOR
__interrupt void uart_interrupt (void)
{
    char c;

    switch(UCA0IV)
    {
        case 0x00:  // Vector 0: No interrupts
            break;
        case 0x02:  // Vector 2: UCRXIFG
            /* read byte from serial line */
            /* store in buffer            */
            /* send echo                  */
            c = UCA0RXBUF;
            last_char = c;
            break;
        case 0x04:  // Vector 4: UCTXIFG
            break;
        case 0x06:  // Vector 6: UCSTTIFG
            break;
        case 0x08:  // Vector 8: UCTXCPTIFG
            break;
        default:
            break;
    }
}

/* ===== public functions ===== */

/* ----- initialize UART ----- */
void uart_init(void)
{
    /* attach port pins to UART eUSCI_A0 */
    P2SEL1 |=   BIT0 | BIT1;
    P2SEL0 &= ~(BIT0 | BIT1);

    /* configure eUSCI_A0 for UART mode 9600 8N1 */
    UCA0CTLW0  = UCSWRST;                  /* put eUSCI_A0 in reset */
                                           /* sets  : UCTXIFG */
                                           /* clears: UCRXIE, UCTXIE, UCRXIFG */
    UCA0CTLW0 |= UCSSEL__SMCLK;            /* BRCLK = SMCLK */
    UCA0BR0    = 6;                        /* see SLAU367P table 30-5 for */
    UCA0BR1    = 0;                        /* BRCLK = 1MHz    */
    UCA0MCTLW  = UCBRS5 | UCBRF3 | UCOS16; /* Baudrate = 9600 */
    UCA0CTLW0 &= ~UCSWRST;                 /* release eUSCI_A0 from reset */

    /* enable receive interrupt */
    UCA0IE |= UCRXIE;

    /* clear rx buffer */
    memset(rx_buffer, 0, UART_RX_BUFFER_SIZE);

    /* reset buffer index */
    rx_buffer_index = 0;

    /* reset rx state machine */
    rx_state = RX_RUNNING;

    last_char = EOF;
}

/* ----- send a byte ----- */
int putchar (int byte)
{
    /* wait until TX buffer empty */
    while (!(UCA0IFG & UCTXIFG));

    /* send byte */
    UCA0TXBUF = byte;

    return 0;
}

/* ----- send a string ----- */
int uart_puts (char* s)
{
    while(*s != 0)
    {
        putchar(*s);
        s++;
    }
}

/* ----- read a string ----- */
char* uart_gets (char* s)
{
    if(rx_state == RX_FULL)
    {
        strcpy(s, rx_buffer);
        memset(rx_buffer, 0, UART_RX_BUFFER_SIZE);
        rx_buffer_index = 0;
        rx_state = RX_RUNNING;
        return s;
    }

    return NULL;
}

/* ----- read character ----- */
int getchar (void)
{
    int c;

    __bic_SR_register(GIE); // start of atomic region
    c = last_char;
    last_char = EOF;
    __bis_SR_register(GIE); // end of atomic region
    return c;
}
