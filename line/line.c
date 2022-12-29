/* ===== file header ===== */

/* ===== includes ===== */
#include "terminal.h"

/* ===== private datatypes ===== */

/* ===== private symbols ===== */

/* ===== private constants ===== */

/* ===== public constants ===== */

/* ===== private variables ===== */

/* ===== public variables ===== */

/* ===== private functions ===== */

/* ===== public functions ===== */

/*************************************************************************
Purpose: initialize terminal module
Input  : none
Return : none
**************************************************************************/
void terminal_init (void)
{
}

/*************************************************************************
Purpose: places one character in internal buffer
          evaluates character for special actions
Input  : c - character to process
Return : 0x0100..0x7fff -  256.. 32767 -
         0x0001..0x00ff -    1..   255 - provided character
                 0x0000 -    0
         0xffff..0xff00 -   -1..  -256 - index for ansi string to send to terminal
         0xfeff..0x8000 - -257..-32768 - error (buffer overflow and the like)
**************************************************************************/
int terminal_putc (char c)
{
    return 0;
}

/*************************************************************************
Purpose: reads string from internal buffer, if available
Input  : s - pointer to external buffer
Return : >0 - number of character transferred, length of string without trailing zero
          0 - no pointer to external buffer provided
         -1 - no string available
**************************************************************************/
int terminal_gets (char* s)
{
    return 0;
}

