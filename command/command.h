#ifndef COMMAND_H
#define COMMAND_H

/* ===== file header ===== */

/* ===== includes ===== */

/* ===== public datatypes ===== */
typedef void (*CLI_Command_Function_t)(void);

typedef struct
{
    char Command[32];
    char Command_Desc[64];
    CLI_Command_Function_t Command_Func;
} CLI_Command_t;

/* ===== public symbols ===== */

/* ===== public constants ===== */

/* ===== public variables ===== */

/* ===== public functions ===== */
void  command_init (const CLI_Command_t cmd_tab[], unsigned int cmd_num);
int   command_get_index (char* cmd);
char* command_get_command (int cmd_idx);
void  command_debug (void);
#endif

