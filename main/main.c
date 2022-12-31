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
#define CLEAR_SCREEN    "\e[2J"
#define PROMPT          "msp430> "

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
    printf("%s", CLEAR_SCREEN);
    puts  ("");
    puts  ("*----------------------------------------*");
    puts  ("*         MSP-EXP430FR5969 LaunchPad     *");
    puts  ("*         Command Line Interface         *");
    puts  ("*----------------------------------------*");
    puts  ("");
    puts  ("<< System Info >>");
    puts  ("  MCU:                     MSP430FR5969");
    puts  ("  FRAM:                    64kB");
    printf("  FRAM used:               %u\n", &__m_flash_size);
    puts  ("  SRAM:                    2kB");
    printf("  SRAM used:               %u\n", &__m_ram_size);
    puts  ("  Main clock (MCLK):       16MHz");
    puts  ("  Sub-Main clock (SMCLK):  1MHz");
    puts  ("  System console baudrate: 9600bps");
    puts  ("");
    printf("%s", PROMPT);
}

/* ----- command executing: help ----- */
static void cmd_help(int argc, char* argv[])
{
    uint8_t i;
    /* Print all commands and description for usage */
    printf( "\nPlease input command as follows:");

    for (i = 0; i < COMMAND_NUM; i++)
    {
        printf("\n  %-8s: %s", command_tbl[i].Command, command_tbl[i].Command_Desc);
    }
    puts("");
}

/*----- command executing: hello ----- */
static void cmd_hello(int argc, char* argv[])
{
    /* Say "Hello, World!"" */
    printf("\nHello, World!");
    if (argc == 1)
    {
        printf("\nI'm Peter. You'll find me on Earth.");
    }
    else
    {
        printf("\nI'm %s. You'll find me on Earth.", argv[1]);
    }
    puts("");
}

/* ----- command executing: info ----- */
static void cmd_info(int argc, char* argv[])
{
    switch(argc)
    {
        case 1:
            printf("\n  CPU            : MSP430FR5969");
            printf("\n  Architecture   : 16bit RISC Architecture");
            printf("\n  CPU clock      : 16MHz");
            printf("\n  FRAM           : 64kB");
            printf("\n  SRAM           : 2kB");
            printf("\n  ADC            : 12-bit, 16 ext, 2 int channels");
            printf("\n  Comparator     : 16 channels");
            printf("\n  Timer          : 2 Timer A, 7 Timer B");
            printf("\n  Comunication   : 2 I2C/SPI/UART");
            printf("\n  AES            : yes");
            printf("\n  BSL            : UART");
            printf("\n  Debug interface: JTAG + Spy-Bi-wire");
            break;

        case 2:
            if( strcmp(argv[1], "cpu") == 0 )
            {
                printf("\n  CPU      : MSP430FR5969");
                printf("\n  CPU clock: 16MHz");
            }
            else if( strcmp(argv[1], "arch") == 0 )
            {
                printf("\n  Architecture: 16bit RISC Architecture");
            }
            else if( strcmp(argv[1], "mem") == 0 )
            {
                printf("\n  FRAM: 64kB");
                printf("\n  SRAM:  2kB");
            }
            else if( strcmp(argv[1], "peri") == 0 )
            {
                printf("\n  ADC         : 12-bit, 16 ext, 2 int channels");
                printf("\n  Comparator  : 16 channels");
                printf("\n  Timer       : 2 Timer A, 7 Timer B");
                printf("\n  Comunication: 2 I2C/SPI/UART");
            }
            else if( strcmp(argv[1], "misc") == 0 )
            {
                printf("\n  AES            : yes");
                printf("\n  BSL            : UART");
                printf("\n  Debug interface: JTAG + Spy-Bi-wire");
             }
            else
            {
                printf("\n  Invalid argument!");           
            }
            break;

        default:
            printf("\n  Invalid number of arguments!");
            break;
    }
    puts("");
}

/* ----- command executing: showarg ----- */
static void cmd_showarg(int argc, char* argv[])
{
    int i;

    printf("\n  argc   : %d\n", argc);
    for (i=0; i<argc; i++)
    {
        printf("  argv[%d]: %s\n", i, argv[i]);
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
                    printf("\nInvalid command!\n");
                    break;
                case COMMAND_MISSING:
                    printf("\nMissing command!\n");
                    break;
                default:
                    command_get_function(cmd_idx)(main_argc, main_argv);
                    break;
            }

            printf("\n%s", PROMPT);

            led_off(LED_RED);
            led_on(LED_GREEN);
        }
    }
}

