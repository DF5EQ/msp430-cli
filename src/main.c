/**
 * Copyright (C) 2018  nhivp
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

#include <msp430.h>
#include "msp_printf.h"
#include "msp_uart.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/**
 *  Typedef definition
 */
typedef void (*CLI_Command_Function_t)(void);

typedef struct
{
    char Command[32];
    char Command_Desc[64];
    CLI_Command_Function_t Command_Func;
} CLI_Command_t;

/**
 *  Macro definition
 */
#define COMMAND_LEN(x)            sizeof(x)/sizeof(*(&x[0]))
#define COMMAND_STRING_LEN        128

/**
 * Private function prototypes
 */
static void board_init(void);
static void startup_cli(void);
static void CLI_GetCommand(unsigned char* cmd);
static void CLI_Help(void);
static void CLI_Hello(void);
static void CLI_Info(void);

/**
 *  Private variables
 */
static CLI_Command_t command_tbl[] =
{
    /* Command, Description,                 Command_Func */
    { "help"  , "Show a list of commands",   CLI_Help  },
    { "info"  , "Show all features of MCU",  CLI_Info  },
    { "hello" , "Say \"Hello, World\"",      CLI_Hello }
};

/**
 *  Exported variables
 */
unsigned char parameterString[COMMAND_STRING_LEN];
uint8_t parameterLength;
volatile bool validCommandFlag;
extern volatile uint16_t __m_flash_size;
extern volatile uint16_t __m_ram_size;

/**
 * Main App
 */
int main(void)
{
    unsigned char cmd[32];
    uint8_t cmd_idx;

    board_init();
    uart_init();
    memset(parameterString, '\0', COMMAND_STRING_LEN);
    memset(cmd, '\0', 32);

    /* Show banner */
    startup_cli();
    /* Enable interrupt */
    __bis_SR_register(GIE);

    while (1)
    {
        /* The 'validCommandFlag' is true when the user enter an input command from console */
        while (validCommandFlag)
        {
            CLI_GetCommand(cmd);

            if (cmd[0] == '\0')
            {
                printf("\r\nMissing command!");
            }
            else
            {
                for (cmd_idx = 0; cmd_idx < COMMAND_LEN(command_tbl); cmd_idx++)
                {
                    if (!strcmp((char*)cmd, (char*)command_tbl[cmd_idx].Command))
                    {
                        break;
                    }
                }

                if (cmd_idx < COMMAND_LEN(command_tbl))
                {
                    /* Execute command */
                    command_tbl[cmd_idx].Command_Func();
                }
                else
                {
                    printf("\r\nInvalid command!");
                }
            }

            printf("\r\nMSP430@CLI > ");

            /* Reset receive buffer and flag*/
            memset(parameterString, '\0', parameterLength + 1);
            memset(cmd, '\0', 32);
            parameterLength = 0;
            validCommandFlag = false;
        }
    }
}

/**
 * Initialize the board MSP430FR5969 LaunchPad
 */
static void board_init(void)
{
    /* disable watchdog */
    WDTCTL = WDTPW + WDTHOLD;

    /* unlock ports */
    PM5CTL0 &= ~LOCKLPM5;

    /* initialise FRAM for 16MHz operation */
    FRCTL0   = FWPW;      /* unlock FR registers */
    FRCTL0_L = NACCESS_1; /* for MCLK = 16MHz we need one wait state for FRAM access */
    FRCTL0_H = 0;         /* lock FR registers */

    /* set MCLK = 16Mhz, SMCLK = 1MHz */
    CSCTL0   = CSKEY;                        /* unlock CS registers */
    CSCTL1   = DCOFSEL_4 | DCORSEL;          /* set DCO to 16MHz */
    CSCTL3   = DIVA__1 | DIVS__16 | DIVM__1; /* ACLK divider = 1, SMCLK divider = 16, MCLK divider = 1 */
    CSCTL0_H = 0;                            /* lock CS registers */
}

/**
 * Start CLI
 */
static void startup_cli(void)
{
    printf("\r\n");
    printf("*----------------------------------------*\r\n");
    printf("*         MSP-EXP430FR5969 LaunchPad     *\r\n");
    printf("*         Command Line Interface         *\r\n");
    printf("*----------------------------------------*\r\n");
    printf("\n<< System Info >>\r\n");
    printf("\tMCU:                     MSP430FR5969\r\n");
    printf("\tFRAM:                    64kB\r\n");
    printf("\tFRAM used:               %u\r\n", __m_flash_size);
    printf("\tSRAM:                    2kB\r\n");
    printf("\tSRAM used:               %u\r\n", __m_ram_size);
    printf("\tMain clock (MCLK):       16MHz\r\n");
    printf("\tSub-Main clock (SMCLK):  1MHz\r\n");
    printf("\tSystem console baudrate: 9600bps\r\n");
    printf("\r\n\r\nMSP430@CLI > ");
}

/**
 *  Get a command from the input string.
 */
static void CLI_GetCommand(unsigned char* cmd)
{
    uint8_t cmd_len;

    for (cmd_len = 0; cmd_len < parameterLength; cmd_len++)
    {
        if ((parameterString[cmd_len] == ' ')
            || (parameterString[cmd_len] == '\0')
            || (parameterString[cmd_len] == 0x0D))
        {
            strncpy((char*)cmd, (char*)parameterString, cmd_len);
            break;
        }
    }
}

/**
 * Command executing: help
 */
static void CLI_Help(void)
{
    uint8_t i;
    /* Print all commands and description for usage */
    printf( "\r\nPlease input command as follows:");

    for (i = 0; i < COMMAND_LEN(command_tbl); i++)
    {
        printf("\r\n\t%s: %s", command_tbl[i].Command, command_tbl[i].Command_Desc);
    }
    printf("\r\n");
}

/**
 * Command executing: hello
 */
static void CLI_Hello(void)
{
    /* Say "Hello, World!"" */
    printf("\r\nHello, World!");
    printf("\r\nI'm Peter. You'll find me on Earth.");
}

/**
 * Command executing: info
 */
static void CLI_Info(void)
{
    printf("\r\n<< Device Info >>");
    printf("\r\n\tCPU:             MSP430FR5969");
    printf("\r\n\tArchitecture:    16bit RISC Architecture");
    printf("\r\n\tCPU clock:       16MHz");
    printf("\r\n\tFRAM:            64kB");
    printf("\r\n\tSRAM:            2kB");
    printf("\r\n\tADC:             12-bit, 16 ext, 2 int channels");
    printf("\r\n\tComparator:      16 channels");
    printf("\r\n\tTimer:           2 Timer A, 7 Timer B");
    printf("\r\n\tComunication:    2 I2C/SPI/UART");
    printf("\r\n\tAES:             yes");
    printf("\r\n\tBSL:             UART");
    printf("\r\n\tDebug interface: JTAG + Spy-Bi-wire");
}

/**************************************************
 * Interrupt Handler
 **************************************************/
#pragma vector = USCI_A0_VECTOR
__interrupt void uart_interrupt (void)
{
    parameterString[parameterLength] = uart_getc();
    uart_putc(parameterString[parameterLength]); /* Echo */

    /* Also get the characters '\r\n' */
    if (parameterString[parameterLength++] == '\r')
    {
        validCommandFlag = true;
    }
}
