/* ===== file header ===== */
/*************************************************************************
Title   : Interrupt UART module with receive/transmit circular buffers
Author  : Peter Bägel
Software: msp430-gcc
Hardware: MSP430, tested on EXP430FR5969
License : GNU General Public License
Usage   : see README.md

Based on original avr-uart library by
    Andy Gock
    Peter Fluery
    Tim Sharpe
    Nicholas Zambetti

https://github.com/andygock/avr-uart
http://beaststwo.org/avr-uart/index.shtml
http://www.peterfleury.epizy.com/avr-software.html

LICENSE:
    Copyright (C) 2022 Peter Bägel (DF5EQ)
	Copyright (C) 2012 Andy Gock
	Copyright (C) 2006 Peter Fleury

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
*************************************************************************/

/* ===== includes ===== */
#include "uart.h"
#include "atomic.h"

/* ===== private datatypes ===== */

/* ===== private symbols ===== */

/* see SLAU367P chapter 30.3.10 Setting a Baud Rate */
#define FACTOR_N (UART_BRCLK/UART_BAUDRATE)
#if FACTOR_N > 17
    #define OS16 1
    #define UCBRx FACTOR_N/16
    #define UCBRFx FACTOR_N%16
#else
    #define OS16 0
    #define UCBRx FACTOR_N
    #define UCBRFx 0
#endif
#define FRACTION (10000ULL * UART_BRCLK / UART_BAUDRATE - UART_BRCLK / UART_BAUDRATE * 10000ULL)
#if FRACTION < 529
    #define UCBRSx 0x00
#elif FRACTION < 715
    #define UCBRSx 0x01
#elif FRACTION < 835
    #define UCBRSx 0x02
#elif FRACTION < 1001
    #define UCBRSx 0x04
#elif FRACTION < 1252
    #define UCBRSx 0x08
#elif FRACTION < 1430
    #define UCBRSx 0x10
#elif FRACTION < 1670
    #define UCBRSx 0x20
#elif FRACTION < 2147
    #define UCBRSx 0x11
#elif FRACTION < 2224
    #define UCBRSx 0x21
#elif FRACTION < 2503
    #define UCBRSx 0x22
#elif FRACTION < 3000
    #define UCBRSx 0x44
#elif FRACTION < 3335
    #define UCBRSx 0x25
#elif FRACTION < 3575
    #define UCBRSx 0x49
#elif FRACTION < 3753
    #define UCBRSx 0x4a
#elif FRACTION < 4003
    #define UCBRSx 0x52
#elif FRACTION < 4286
    #define UCBRSx 0x92
#elif FRACTION < 4378
    #define UCBRSx 0x53
#elif FRACTION < 5002
    #define UCBRSx 0x55
#elif FRACTION < 5715
    #define UCBRSx 0xaa
#elif FRACTION < 6003
    #define UCBRSx 0x6b
#elif FRACTION < 6254
    #define UCBRSx 0xad
#elif FRACTION < 6432
    #define UCBRSx 0xb5
#elif FRACTION < 6667
    #define UCBRSx 0xb6
#elif FRACTION < 7001
    #define UCBRSx 0xd6
#elif FRACTION < 7147
    #define UCBRSx 0xb7
#elif FRACTION < 7503
    #define UCBRSx 0xbb
#elif FRACTION < 7861
    #define UCBRSx 0xdd
#elif FRACTION < 8004
    #define UCBRSx 0xed
#elif FRACTION < 8333
    #define UCBRSx 0xee
#elif FRACTION < 8464
    #define UCBRSx 0xbf
#elif FRACTION < 8572
    #define UCBRSx 0xdf
#elif FRACTION < 8751
    #define UCBRSx 0xef
#elif FRACTION < 9004
    #define UCBRSx 0xf7
#elif FRACTION < 9170
    #define UCBRSx 0xfb
#elif FRACTION < 9288
    #define UCBRSx 0xfd
#elif FRACTION < 10000
    #define UCBRSx 0xfe
#else
    #error no lookup value for UCBRSx
#endif

#define UCAxBRW     UCBRx
#define UCAxMCTLW   ( UCBRSx << 8 | UCBRFx << 4 | OS16 )

/* ===== private constants ===== */

/* ===== public constants ===== */

/* ===== private variables ===== */
static volatile uint8_t UART_TxBuf[UART_TX_BUFFER_SIZE];
static volatile uint8_t UART_RxBuf[UART_RX_BUFFER_SIZE];
static volatile uint8_t UART_TxHead;
static volatile uint8_t UART_TxTail;
static volatile uint8_t UART_RxHead;
static volatile uint8_t UART_RxTail;

/* ===== public variables ===== */

/* ===== private functions ===== */

/*************************************************************************
Function: UART Receive Complete interrupt
Purpose : called when the UART has received a character
*************************************************************************/
static void uart_rx (void)
{
    uint16_t tmphead;

    /* calculate buffer index */
    tmphead = UART_RxHead + 1;
    if (tmphead >= UART_RX_BUFFER_SIZE) tmphead = 0;

    if (tmphead != UART_RxTail)
    {
        /* store new index */
        UART_RxHead = tmphead;

        /* store received character in receive buffer */
        UART_RxBuf[tmphead] = UCA0RXBUF;
    }
}

/*************************************************************************
Function: UART Data Register Empty interrupt
Purpose : called when the UART is ready to transmit the next byte
**************************************************************************/
static void uart_tx (void)
{
    uint16_t tmptail;

    if (UART_TxHead != UART_TxTail)
    {
        /* calculate and store new buffer index */
        tmptail = (UART_TxTail + 1);
        if (tmptail >= UART_TX_BUFFER_SIZE) tmptail = 0;
        UART_TxTail = tmptail;

        /* get one byte from buffer and write it to UART */
        UCA0TXBUF = UART_TxBuf[tmptail];  /* start transmission */
    }
    else
    {
        /* tx buffer empty, disable interrupt */
        UCA0IE &= ~UCTXIE;
    }
}

/* ===== interrupt functions ===== */

#pragma vector = USCI_A0_VECTOR
__interrupt void uart_interrupt (void)
{
    switch(UCA0IV)
    {
        case 0x00:  /* Vector 0: No interrupts */
            break;
        case 0x02:  /* Vector 2: UCRXIFG */
            uart_rx();
            break;
        case 0x04:  /* Vector 4: UCTXIFG */
            uart_tx();
            break;
        case 0x06:  /* Vector 6: UCSTTIFG */
            break;
        case 0x08:  /* Vector 8: UCTXCPTIFG */
            break;
        default:
            break;
    }
}

/* ===== public functions ===== */

/*************************************************************************
Purpose : initialize UART and set baudrate
Input   : baudrate using defines
          UART_BRCLK    - baudrate generator clock in hz
          UART_BAUDRATE - baudrate in bit per second
Returns : none
**************************************************************************/
void uart_init(void)
{
	/* set heads and tails to initial positions */
    UART_TxHead = 0;
    UART_TxTail = 0;
    UART_RxHead = 0;
    UART_RxTail = 0;

    /* attach port pins to UART eUSCI_A0 */
    P2SEL1 |=   BIT0 | BIT1;
    P2SEL0 &= ~(BIT0 | BIT1);

    /* configure eUSCI_A0 for UART mode 8N1 */
    UCA0CTLW0  = UCSWRST;       /* put eUSCI in reset */
    UCA0CTLW0 |= UCSSEL__SMCLK; /* BRCLK = SMCLK */
    UCA0BRW    = UCAxBRW;       /* see SLAU367P table 30-5 */
    UCA0MCTLW  = UCAxMCTLW;     /* see SLAU367P table 30-5 */
    UCA0CTLW0 &= ~UCSWRST;      /* release eUSCI from reset */

    /* enable receive interrupt */
    UCA0IE |= UCRXIE;
}

/*************************************************************************
Purpose : return next character from receive buffer
Input   : none
Returns : next character from reveive buffer converted to int 
          EOF if no new character available
**************************************************************************/
int16_t uart_getc(void)
{
    ATOMIC_BLOCK_RESTORESTATE
    (
        if (UART_RxHead == UART_RxTail)
        {
            /* no data available */
            return EOF;
        }
    )

	/* advance buffer index */
	UART_RxTail = UART_RxTail + 1;
    if (UART_RxTail >= UART_RX_BUFFER_SIZE) UART_RxTail = 0;

	/* return data from receive buffer */
	return UART_RxBuf[UART_RxTail];;
}

/*************************************************************************
Purpose : write character to transmitt buffer
Input   : character to write
Returns : written character
**************************************************************************/
int16_t uart_putc(int16_t c)
{
	uint16_t tmphead;

	/* calculate buffer index */
	tmphead = (UART_TxHead + 1);
    if (tmphead >= UART_TX_BUFFER_SIZE) tmphead = 0;

    /* wait for free space in buffer */
    while (tmphead == UART_TxTail);

	/* put character to transmit buffer */
	UART_TxBuf[tmphead] = c;
	UART_TxHead = tmphead;

    /* if transmit interrupt not enabled */
    if( (UCA0IE & UCTXIE) == 0 )
    {
        /* enable interrupt */
        UCA0IFG |= UCTXIFG; /* set transmit interrupt flag */
        UCA0IE  |= UCTXIE;  /* enable transmit interrupt */
    }

    /* convert LF into LF/CR */
    if(c=='\n')
    {
        uart_putc('\r');
    }

    return c;
}

/* ===== alias for public functions ===== */

int putchar(int c) __attribute__((alias("uart_putc")));
int getchar(void)  __attribute__((alias("uart_getc")));

