/* ===== file header ===== */

#ifndef LINE_H
#define LINE_H

/* ===== includes ===== */
#include <msp430.h>

/* ===== public datatypes ===== */

/* ===== public symbols ===== */
#define NUL 0x00
#define BEL 0x07
#define BS  0x08
#define LF  0x0a
#define CR  0x0d
#define ESC 0x1b

//#define CURSOR_RIGHT      "\x1b[C"
//#define CURSOR_LEFT       "\x1b[D"
//#define CURSOR_SAVE       "\x1b[s"
//#define CURSOR_RESTORE    "\x1b[u"
//#define DELETE_TO_LINEEND "\x1b[0K"

/* ===== public constants ===== */
enum line_strings_index
{
    CURSOR_RIGHT,
    CURSOR_LEFT,
    CURSOR_SAVE,
    CURSOR_RESTORE,
    DELETE_TO_LINEEND
};

const char* line_strings[] =
{
    "\x1b[C",    /* CURSOR_RIGHT      */
    "\x1b[D",    /* CURSOR_LEFT       */
    "\x1b[s",    /* CURSOR_SAVE       */
    "\x1b[u",    /* CURSOR_RESTORE    */
    "\x1b[0K"    /* DELETE_TO_LINEEND */ 
};

/* ===== public variables ===== */

/* ===== public functions ===== */
void line_init (void);
int  line_putc (char c);
int  line_gets (char* s);

#endif

