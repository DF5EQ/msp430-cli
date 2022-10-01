/* ===== file header ===== */

/* ===== includes ===== */
#include <msp430.h>
#include <stdio.h>
#include "command.h"

/* ===== private datatypes ===== */

/* ===== private symbols ===== */
#define CLEAR_SCREEN "\r\e[2J"

/* ===== private constants ===== */

/* ===== public constants ===== */

/* ===== private variables ===== */
static const CLI_Command_t* command_table;
static unsigned int command_number;

/* ===== public variables ===== */

/* ===== private functions ===== */

/* ===== public functions ===== */
void command_init (const CLI_Command_t cmd_tab[], unsigned int cmd_num)
{
    command_table  = cmd_tab;
    command_number = cmd_num;
}

void command_debug (void)
{
    int i;

    puts(CLEAR_SCREEN);
    for(i=0; i<command_number; i++)
    {
        printf("%2d: %-8s %-32s\r\n", i, command_table[i].Command, command_table[i].Command_Desc);
    } 
}
