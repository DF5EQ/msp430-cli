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
#include "uart.h"

/* ===== private datatypes ===== */

/* ===== private symbols ===== */
#define RX_BUFFER_LENGTH 10

#define NUL 0x00
#define BEL 0x07
#define BS  0x08
#define LF  0x0a
#define CR  0x0d

/* ===== private constants ===== */

/* ===== public constants ===== */

/* ===== private variables ===== */
static char rx_buffer[RX_BUFFER_LENGTH];
static char rx_buffer_index;
static bool cr_received = false;

/* ===== public variables ===== */

/* ===== private functions ===== */
static unsigned char uart_rx (unsigned char c)
{
    if(c == CR || c == LF)
    {
        rx_buffer[rx_buffer_index] = NUL;
        cr_received = true;
        return NUL;
    }

    if(c == BS)
    {
        if(rx_buffer_index == 0)
        {
            return NUL;
        }
        rx_buffer_index--;
        return BS;
    }

    if(rx_buffer_index >= RX_BUFFER_LENGTH-1)
    {
        return BEL;
    }

    rx_buffer[rx_buffer_index] = c;
    rx_buffer_index++;
    return c;
}

/* ===== interrupt functions ===== */
#pragma vector = USCI_A0_VECTOR
__interrupt void uart_interrupt (void)
{
    switch(UCA0IV)
    {
        case 0x00:  // Vector 0: No interrupts
            break;
        case 0x02:  // Vector 2: UCRXIFG
            /* read byte from serial line */
            /* store in buffer            */
            /* send echo                  */
            putchar(uart_rx(UCA0RXBUF));
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

    /* reset buffer index */
    rx_buffer_index = 0;
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

/* ----- read a string ----- */
char* uart_gets (char* s, const unsigned int n)
{
    if(cr_received == true)
    {
        strcpy(s, rx_buffer);
        rx_buffer_index = 0;
        cr_received = false;
        return s;
    }

    return NULL;
}
