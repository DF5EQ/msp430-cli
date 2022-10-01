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

char* command_get_command (int cmd_idx)
{
    if (cmd_idx < 0 || cmd_idx >= command_number)
    {
        return NULL;
    }
    return command_table[cmd_idx].Command;
}

void command_debug (void)
{
    int cmd_idx;

    for (cmd_idx=-3; cmd_idx<=5; cmd_idx++)
    {
        printf("%2d %s\r\n", cmd_idx, command_get_command(cmd_idx));
    }
}
