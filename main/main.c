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
#include "command.h"

/* ===== private datatypes ===== */

/* ===== private symbols ===== */
#define COMMAND_NUM     (sizeof(command_tbl)/sizeof(command_tbl[0]))
#define CLEAR_SCREEN    "\r\e[2J"
#define PROMPT          "msp430 >"

/* ===== private constants ===== */

/* needed forward declarations */
static void cmd_help(void);
static void cmd_info(void);
static void cmd_hello(void);

static const command_t command_tbl[] =
{
    /* Command, Description,                 Command_Func */
    { "help"  , "Show a list of commands",   cmd_help  },
    { "info"  , "Show all features of MCU",  cmd_info  },
    { "hello" , "Say \"Hello, World\"",      cmd_hello }
};

/* ===== public constants ===== */

extern volatile uint16_t __m_flash_size;
extern volatile uint16_t __m_ram_size;

/* ===== private variables ===== */

/* ===== public variables ===== */

/* ===== private functions ===== */

/* ----- start ----- */
static void startup(void)
{
    puts(CLEAR_SCREEN);
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
    printf("\r\n\r\n%s ", PROMPT);
}

/* ----- command executing: help ----- */
static void cmd_help(void)
{
    uint8_t i;
    /* Print all commands and description for usage */
    printf( "\r\nPlease input command as follows:");

    for (i = 0; i < COMMAND_NUM; i++)
    {
        printf("\r\n\t%s: %s", command_tbl[i].Command, command_tbl[i].Command_Desc);
    }
    printf("\r\n");
}

/*----- command executing: hello ----- */
static void cmd_hello(void)
{
    /* Say "Hello, World!"" */
    printf("\r\nHello, World!");
    printf("\r\nI'm Peter. You'll find me on Earth.");
    printf("\r\n");
}

/* ----- command executing: info ----- */
static void cmd_info(void)
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
    printf("\r\n");
}

/* ===== public functions ===== */

int main(void)
{
    unsigned char cmd[32];
    int cmd_idx;

    int i;

    int main_argc;
    char* main_argv[COMMAND_MAX_ARGC];

    /* initilise all modules */
    system_init();
    led_init();
    uart_init();
    command_init(command_tbl, COMMAND_NUM);

    /* show banner */
    startup();

    /* enable interrupt */
    __bis_SR_register(GIE);

    led_on(LED_GREEN);

    while (1)
    {
        /* uart_gets returns a non-NULL pointer when a string is available in uart module */
        if (uart_gets(cmd))
        {
            led_on(LED_RED);
            led_off(LED_GREEN);

            command_parse(cmd, &main_argc, main_argv);
            printf("\n\rargc: %d\n\r", main_argc);
            for (i=0; i<main_argc; i++)
            {
                printf("argv[%d]: %s\n\r", i, main_argv[i]);
            }

            switch (cmd_idx = command_get_index(cmd))
            {
                case COMMAND_INVALID:
                    printf("\r\nInvalid command!\r\n");
                    break;
                case COMMAND_MISSING:
                    printf("\r\nMissing command!\r\n");
                    break;
                default:
                    command_get_function(cmd_idx)();
                    break;
            }

            printf("\r\n%s ", PROMPT);

            led_off(LED_RED);
            led_on(LED_GREEN);
        }
    }
}

