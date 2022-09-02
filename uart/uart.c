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
#include "uart.h"

/* ===== private datatypes ===== */

/* ===== private symbols ===== */

/* ===== private constants ===== */

/* ===== public constants ===== */

/* ===== private variables ===== */

/* ===== public variables ===== */

/* ===== private functions ===== */

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
}

/* ----- receive a single byte out through UART ----- */
unsigned char uart_getc(void)
{
    return UCA0RXBUF;
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
