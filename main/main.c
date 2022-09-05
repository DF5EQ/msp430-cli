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
#include <msp430.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "system.h"
#include "led.h"
#include "uart.h"

/* ===== private datatypes ===== */
typedef void (*CLI_Command_Function_t)(void);

typedef struct
{
    char Command[32];
    char Command_Desc[64];
    CLI_Command_Function_t Command_Func;
} CLI_Command_t;

/* ===== private symbols ===== */
#define COMMAND_LEN(x)     sizeof(x)/sizeof(*(&x[0]))
#define COMMAND_STRING_LEN 128

/* ===== private constants ===== */

/* ===== public constants ===== */

/* ===== private variables ===== */

/* needed forward declarations */
static void CLI_Help(void);
static void CLI_Info(void);
static void CLI_Hello(void);

static CLI_Command_t command_tbl[] =
{
    /* Command, Description,                 Command_Func */
    { "help"  , "Show a list of commands",   CLI_Help  },
    { "info"  , "Show all features of MCU",  CLI_Info  },
    { "hello" , "Say \"Hello, World\"",      CLI_Hello }
};

/* ===== public variables ===== */

/* provided public variables */
unsigned char parameterString[COMMAND_STRING_LEN];
uint8_t       parameterLength;

/* consumed public variables */
extern volatile uint16_t __m_flash_size;
extern volatile uint16_t __m_ram_size;

/* ===== private functions ===== */

/* ----- start CLI ----- */
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
    printf("\tFRAM used:               %u\r\n", &__m_flash_size);
    printf("\tSRAM:                    2kB\r\n");
    printf("\tSRAM used:               %u\r\n", &__m_ram_size);
    printf("\tMain clock (MCLK):       16MHz\r\n");
    printf("\tSub-Main clock (SMCLK):  1MHz\r\n");
    printf("\tSystem console baudrate: 9600bps\r\n");
    printf("\r\n\r\nmsp430-cli > ");
}

/* ----- get a command from the input string ----- */
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

/* ----- command executing: help ----- */
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

/*----- command executing: hello ----- */
static void CLI_Hello(void)
{
    /* Say "Hello, World!"" */
    printf("\r\nHello, World!");
    printf("\r\nI'm Peter. You'll find me on Earth.");
}

/* ----- command executing: info ----- */
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

/* ===== public functions ===== */

int main(void)
{
    unsigned char cmd[32];
    uint8_t cmd_idx;

    system_init();
    led_init();
    uart_init();

    /* show banner */
    startup_cli();
    /* enable interrupt */
    __bis_SR_register(GIE);

    led_on(LED_GREEN);

    while (1)
    {
        /* uart_gets returns a non-NULL pointer when a string is available in uart module */
        while (uart_gets(parameterString, 42))
        {
            led_on(LED_RED);
            led_off(LED_GREEN);

            parameterLength = strlen(parameterString);

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
                    /* execute command */
                    command_tbl[cmd_idx].Command_Func();
                }
                else
                {
                    printf("\r\nInvalid command!");
                }
            }

            printf("\r\nmsp430-cli > ");

            led_off(LED_RED);
            led_on(LED_GREEN);
        }
    }
}

