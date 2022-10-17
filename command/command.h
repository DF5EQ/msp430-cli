#ifndef COMMAND_H
#define COMMAND_H

/* ===== file header ===== */

/* ===== includes ===== */

/* ===== public datatypes ===== */
typedef void (*command_function_t)(int argc, char* argv[]);

typedef struct
{
    char Command[32];
    char Command_Desc[64];
    command_function_t Command_Func;
} command_t;

/* ===== public symbols ===== */
#define COMMAND_MAX_ARGC 10
#define COMMAND_INVALID -2
#define COMMAND_MISSING -1

/* ===== public constants ===== */

/* ===== public variables ===== */

/* ===== public functions ===== */
void                     command_init (const command_t cmd_tab[], unsigned int cmd_num);
int                      command_get_index (char* cmd);
const char*              command_get_command (int cmd_idx);
const char*              command_get_description (int cmd_idx);
const command_function_t command_get_function (int cmd_idx);
char*                    command_parse (char* cmd, int* argc, char* argv[]);
#endif

