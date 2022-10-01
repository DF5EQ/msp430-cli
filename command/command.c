/* ===== file header ===== */

/* ===== includes ===== */
#include <msp430.h>
#include <stdio.h>
#include "command.h"

/* ===== private datatypes ===== */

/* ===== private symbols ===== */

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

int command_get_index (char* cmd)
{
    int cmd_idx;

    if (cmd[0] == 0)
    {
        return -1;
    }

    for (cmd_idx = 0; cmd_idx < command_number; cmd_idx++)
    {
        if (strcmp(cmd, command_table[cmd_idx].Command) == 0)
        {
            return cmd_idx;
        }
    }

    return -2;
}

void command_debug (void)
{
    int cmd_idx;

    cmd_idx = command_get_index("");      printf("%2d\r\n", cmd_idx);
    cmd_idx = command_get_index("help");  printf("%2d\r\n", cmd_idx);
    cmd_idx = command_get_index("info");  printf("%2d\r\n", cmd_idx);
    cmd_idx = command_get_index("hello"); printf("%2d\r\n", cmd_idx);
    cmd_idx = command_get_index("xxx");   printf("%2d\r\n", cmd_idx);
}
