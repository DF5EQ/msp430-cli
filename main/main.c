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
#include "line.h"
#include "uart.h"
#include "command.h"

/* ===== private datatypes ===== */

/* ===== private symbols ===== */
#define COMMAND_NUM     (sizeof(command_tbl)/sizeof(command_tbl[0]))
#define CLEAR_SCREEN    "\r\e[2J"
#define PROMPT          "msp430 >"

#define COMMAND_LEN(x)     sizeof(x)/sizeof(*(&x[0]))
#define COMMAND_STRING_LEN UART_RX_BUFFER_SIZE

/* ===== private constants ===== */

/* needed forward declarations */
static void cmd_help(int argc, char* argv[]);
static void cmd_info(int argc, char* argv[]);
static void cmd_hello(int argc, char* argv[]);
static void cmd_showarg(int argc, char* argv[]);

static const command_t command_tbl[] =
{
    /* Command,   Description,                                     Command_Func */
    { "help"   , "Show a list of commands",                        cmd_help    },
    { "info"   , "Show features of MCU <cpu|arch|mem|peri|misc>",  cmd_info    },
    { "hello"  , "Say \"Hello, World\"",                           cmd_hello   },
    { "showarg", "show arguments of command",                      cmd_showarg }
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
    printf("\n\r");
    printf("*----------------------------------------*\n\r");
    printf("*         MSP-EXP430FR5969 LaunchPad     *\n\r");
    printf("*         Command Line Interface         *\n\r");
    printf("*----------------------------------------*\n\r");
    printf("\n<< System Info >>\n\r");
    printf("\tMCU:                     MSP430FR5969\n\r");
    printf("\tFRAM:                    64kB\n\r");
    printf("\tFRAM used:               %u\n\r", &__m_flash_size);
    printf("\tSRAM:                    2kB\n\r");
    printf("\tSRAM used:               %u\n\r", &__m_ram_size);
    printf("\tMain clock (MCLK):       16MHz\n\r");
    printf("\tSub-Main clock (SMCLK):  1MHz\n\r");
    printf("\tSystem console baudrate: 9600bps\n\r");
    printf("\n\r%s ", PROMPT);
}

/* ----- command executing: help ----- */
static void cmd_help(int argc, char* argv[])
{
    uint8_t i;
    /* Print all commands and description for usage */
    printf( "\n\rPlease input command as follows:");

    for (i = 0; i < COMMAND_NUM; i++)
    {
        printf("\n\r\t%-8s: %s", command_tbl[i].Command, command_tbl[i].Command_Desc);
    }
    printf("\n\r");
}

/*----- command executing: hello ----- */
static void cmd_hello(int argc, char* argv[])
{
    /* Say "Hello, World!"" */
    printf("\n\rHello, World!");
    if (argc == 1)
    {
        printf("\n\rI'm Peter. You'll find me on Earth.");
    }
    else
    {
        printf("\n\rI'm %s. You'll find me on Earth.", argv[1]);
    }
    printf("\n\r");
}

/* ----- command executing: info ----- */
static void cmd_info(int argc, char* argv[])
{
    switch(argc)
    {
        case 1:
            printf("\n\rCPU            : MSP430FR5969");
            printf("\n\rArchitecture   : 16bit RISC Architecture");
            printf("\n\rCPU clock      : 16MHz");
            printf("\n\rFRAM           : 64kB");
            printf("\n\rSRAM           : 2kB");
            printf("\n\rADC            : 12-bit, 16 ext, 2 int channels");
            printf("\n\rComparator     : 16 channels");
            printf("\n\rTimer          : 2 Timer A, 7 Timer B");
            printf("\n\rComunication   : 2 I2C/SPI/UART");
            printf("\n\rAES            : yes");
            printf("\n\rBSL            : UART");
            printf("\n\rDebug interface: JTAG + Spy-Bi-wire");
            break;

        case 2:
            if( strcmp(argv[1], "cpu") == 0 )
            {
                printf("\n\rCPU      : MSP430FR5969");
                printf("\n\rCPU clock: 16MHz");
            }
            else if( strcmp(argv[1], "arch") == 0 )
            {
                printf("\n\rArchitecture: 16bit RISC Architecture");
            }
            else if( strcmp(argv[1], "mem") == 0 )
            {
                printf("\n\rFRAM: 64kB");
                printf("\n\rSRAM:  2kB");
            }
            else if( strcmp(argv[1], "peri") == 0 )
            {
                printf("\n\rADC         : 12-bit, 16 ext, 2 int channels");
                printf("\n\rComparator  : 16 channels");
                printf("\n\rTimer       : 2 Timer A, 7 Timer B");
                printf("\n\rComunication: 2 I2C/SPI/UART");
            }
            else if( strcmp(argv[1], "misc") == 0 )
            {
                printf("\n\rAES            : yes");
                printf("\n\rBSL            : UART");
                printf("\n\rDebug interface: JTAG + Spy-Bi-wire");
             }
            else
            {
                printf("\n\rInvalid argument!");           
            }
            break;

        default:
            printf("\n\rInvalid number of arguments!");
            break;
    }
    printf("\n\r");
}

/* ----- command executing: showarg ----- */
static void cmd_showarg(int argc, char* argv[])
{
    int i;

    printf("\n\rargc: %d\n\r", argc);
    for (i=0; i<argc; i++)
    {
        printf("argv[%d]: %s\n\r", i, argv[i]);
    }
}

/* ===== public functions ===== */

int main(void)
{
    unsigned char cmd[COMMAND_STRING_LEN];
    int cmd_idx;
    int c;

    int main_argc;
    char* main_argv[COMMAND_MAX_ARGC];

    /* initilise all modules */
    system_init();
    led_init();
    uart_init();
    line_init();
    command_init(command_tbl, COMMAND_NUM);

    /* enable interrupt */
    __bis_SR_register(GIE);

    /* show banner */
    startup();

    led_on(LED_GREEN);

    while (1)
    {
        /* wait for char input */
        while( (c=getchar()) == EOF);

        /* send character to line evaluation */
        /* update terminal accordingly */
        line_putc(c);

        /* line_gets returns a non-NULL pointer when a line is available */
        if (line_gets(cmd))
        {
            led_on(LED_RED);
            led_off(LED_GREEN);

            command_parse(cmd, &main_argc, main_argv);

            switch (cmd_idx = command_get_index(cmd))
            {
                case COMMAND_INVALID:
                    printf("\n\rInvalid command!\n\r");
                    break;
                case COMMAND_MISSING:
                    printf("\n\rMissing command!\n\r");
                    break;
                default:
                    command_get_function(cmd_idx)(main_argc, main_argv);
                    break;
            }

            printf("\n\r%s ", PROMPT);

            led_off(LED_RED);
            led_on(LED_GREEN);
        }
    }
}

