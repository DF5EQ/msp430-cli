/* ===== file header ===== */

/* ===== includes ===== */
#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include "command.h"

/* ===== private datatypes ===== */

/* ===== private symbols ===== */
#define COMMAND_DELIMITER " \t\n\r"

/* ===== private constants ===== */

/* ===== public constants ===== */

/* ===== private variables ===== */
static const command_t* command_table;
static unsigned int command_number;

/* ===== public variables ===== */

/* ===== private functions ===== */

/* ===== public functions ===== */
void command_init (const command_t cmd_tab[], unsigned int cmd_num)
{
    command_table  = cmd_tab;
    command_number = cmd_num;
}

int command_get_index (char* cmd)
{
    int cmd_idx;

    if (cmd[0] == 0)
    {
        return COMMAND_MISSING;
    }

    for (cmd_idx = 0; cmd_idx < command_number; cmd_idx++)
    {
        if (strcmp(cmd, command_table[cmd_idx].Command) == 0)
        {
            return cmd_idx;
        }
    }

    return COMMAND_INVALID;
}

const char* command_get_command (int cmd_idx)
{
    if (cmd_idx < 0 || cmd_idx >= command_number)
    {
        return NULL;
    }
    return command_table[cmd_idx].Command;
}

const char* command_get_description (int cmd_idx)
{
    if (cmd_idx < 0 || cmd_idx >= command_number)
    {
        return NULL;
    }
    return command_table[cmd_idx].Command_Desc;
}

const command_function_t command_get_function (int cmd_idx)
{
    if (cmd_idx < 0 || cmd_idx >= command_number)
    {
        return NULL;
    }
    return command_table[cmd_idx].Command_Func;
}

char* command_parse (char* cmd, int* argc, char* argv[])
{
    int index;

    /* get first token */
    *argc = 0;
    index = 0;
    argv[index] = strtok(cmd, COMMAND_DELIMITER);

    /* keep getting tokens while one of the delimiters present in cmd */
    while ( (argv[index] != NULL) && (index < COMMAND_MAX_ARGC-2) )
    {
        (*argc)++;
        index++;
        argv[index] = strtok(NULL, COMMAND_DELIMITER);
    }

    /* get rest of tokens if too much tokens in cmd */
    if(index == COMMAND_MAX_ARGC-2)
    {
        *argc = COMMAND_MAX_ARGC;
        argv[COMMAND_MAX_ARGC-1] = strtok(NULL, NULL);
    }

    return cmd;
}

