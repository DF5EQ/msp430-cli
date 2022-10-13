/* ===== file header ===== */

/* ===== includes ===== */
#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include "command.h"

/* ===== private datatypes ===== */

/* ===== private symbols ===== */

/* ===== private constants ===== */

/* ===== public constants ===== */

/* ===== private variables ===== */
static const command_t* command_table;
static unsigned int command_number;

/* FIXME TEST */
static unsigned int command_argc;
static char*        command_argv [COMMAND_MAX_ARGC];

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
    const char* delimiter = " \t\n\r";
    int index;
    int i;

    /* get first token */
    command_argc = 0;
    index = 0;
    command_argv[index] = strtok(cmd, delimiter);

    /* keep getting tokens while one of the delimiters present in cmd */
    while ( (command_argv[index] != NULL) && (index < COMMAND_MAX_ARGC-2) )
    {
        command_argc++;
        index++;
        command_argv[index] = strtok(NULL, delimiter);
    }

    /* get rest of tokens if too much tokens in cmd */
    if(index == COMMAND_MAX_ARGC-2)
    {
        command_argc++;
        index++;
        command_argv[index] = strtok(NULL, NULL);
        command_argc++;
        index++;
    }

    /* TODO replace command_arg* with arg* and delete the following */
    *argc = command_argc;
    for (i=0; i<index; i++)
    {
        argv[i] = command_argv[i];
    }

    return cmd;
}

