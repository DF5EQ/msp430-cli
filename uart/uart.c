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
typedef enum {
    RX_RUNNING,
    RX_ESC,         /* 0x1b (escape) reveived */
    RX_ESC_SBO,     /* 0x1b (escape) and 0x5b (square bracket open) reveived */
    RX_ESC_SBO_TILDE,
    RX_FULL
} rx_state_t;

/* ===== private symbols ===== */
#define RX_BUFFER_LENGTH 10

#define NUL 0x00
#define BEL 0x07
#define BS  0x08
#define LF  0x0a
#define CR  0x0d
#define ESC 0x1b

/* ===== private constants ===== */

/* ===== public constants ===== */

/* ===== private variables ===== */
static char rx_buffer[RX_BUFFER_LENGTH];
static char rx_buffer_index;
static rx_state_t rx_state;
//static char cli_buffer[100];

/* ===== public variables ===== */

/* ===== private functions ===== */
static unsigned char uart_rx (unsigned char c)
{
    switch(rx_state)
    {
        case RX_RUNNING:
            switch(c)
            {
                case BS: /* TODO delete to left */
                    putchar(BS);
                    break;
                case ESC: /* change state */
                    rx_state = RX_ESC;
                    break;
                case CR:
                case LF:
                    rx_buffer[rx_buffer_index] = 0;
                    rx_state = RX_FULL;
                    break;
                default:
                        if( c >= 0x20 && c <= 0x7f )
                        {
                            if( rx_buffer_index == RX_BUFFER_LENGTH-1 )
                            {
                                putchar(BEL);
                            }
                            else
                            {
                                /* store character */
                                rx_buffer[rx_buffer_index] = c;
                                rx_buffer_index++;

                                /* echo character */
                                putchar(c);
                            }
                        }
                    break;
            }
            break;

        case RX_ESC:
            switch(c)
            {
                case '[':
                    rx_state = RX_ESC_SBO;
                    break;
                default:
                    rx_state = RX_RUNNING;
                    break;
            }
            break;

        case RX_ESC_SBO:
            switch(c)
            {
                case 'C': /* cursor right */
                    if(rx_buffer_index == RX_BUFFER_LENGTH-1)
                    {
                        putchar(BEL);
                    }
                    else
                    {
                        rx_buffer_index++;
                    }
                    rx_state = RX_RUNNING;
                    break;
                case 'D': /* cursor left */
                    if(rx_buffer_index == 0)
                    {
                        putchar(BEL);
                    }
                    else
                    {
                        rx_buffer_index--;
                    }
                    rx_state = RX_RUNNING;
                    break;
                case '2':
                case '3':
                case '5':
                case '6':
                    rx_state = RX_ESC_SBO_TILDE;
                    break;
                default:
                    rx_state = RX_RUNNING;
                    break;
            }
            break;

        case RX_ESC_SBO_TILDE:
            rx_state = RX_RUNNING;
            break;

        case RX_FULL:
            /* nothing to do here */
            /* will be left after read out of buffer */
            break;
    }
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
            uart_rx(UCA0RXBUF);
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

    /* reset rx state machine */
    rx_state = RX_RUNNING;
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
    if(rx_state == RX_FULL)
    {
        strcpy(s, rx_buffer);
        rx_buffer_index = 0;
        rx_state = RX_RUNNING;
        return s;
    }

    return NULL;
}

