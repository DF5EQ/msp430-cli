/* ===== file header ===== */

/* ===== includes ===== */
#include <stdint.h>
#include <string.h>
#include "line.h"

/* ===== private datatypes ===== */
typedef enum {
    LINE_RUNNING,
    LINE_ESC,         /* 0x1b (escape) reveived */
    LINE_ESC_SBO,     /* 0x1b (escape) and 0x5b (square bracket open) reveived */
    LINE_ESC_SBO_TILDE,
    LINE_FULL
} state_t;

/* ===== private symbols ===== */
#define LINE_LENGTH 80

#define NUL 0x00
#define BEL 0x07
#define BS  0x08
#define LF  0x0a
#define CR  0x0d
#define ESC 0x1b

#define CURSOR_RIGHT      "\x1b[C"
#define CURSOR_LEFT       "\x1b[D"
#define CURSOR_SAVE       "\x1b[s"
#define CURSOR_RESTORE    "\x1b[u"
#define DELETE_TO_LINEEND "\x1b[0K"

/* ===== private constants ===== */

/* ===== public constants ===== */

/* ===== private variables ===== */
static uint8_t line[LINE_LENGTH];
static uint8_t position;
static state_t state;

/* ===== public variables ===== */

/* ===== private functions ===== */

/* ===== public functions ===== */

/*************************************************************************
Purpose: initialize line module
Input  : none
Return : none
**************************************************************************/
void line_init (void)
{
    /* reset line buffer */
    memset(line, 0, LINE_LENGTH);
    position = 0;

    /* reset rx state machine */
    state = LINE_RUNNING;
}

/*************************************************************************
Purpose: places one character in internal buffer
         evaluates character for special actions
         updates terminal
Input  : c - character to process
Return : 0..255 - provided character TODO
             -1 - error              TODO
**************************************************************************/
int line_putc (char c)
{
    switch(state)
    {
        case LINE_RUNNING:
            switch(c)
            {
                case BS: /* delete to left */
                    if(position == 0)
                    {
                        putchar(BEL);
                    }
                    else
                    {
                        /* update terminal */
                        printf("%s", CURSOR_LEFT);
                        printf("%s", CURSOR_SAVE);
                        printf("%s", DELETE_TO_LINEEND);
                        printf("%s", &line[position]);
                        printf("%s", CURSOR_RESTORE);

                        /* update buffer */
                        position--;
                        strcpy(&line[position], &line[position+1]);
                    }
                    break;
                    case ESC: /* change state */
                        state = LINE_ESC;
                        break;
                    case CR:
                    case LF:
                        state = LINE_FULL;
                        break;
                    default:
                        if( c >= 0x20 && c <= 0x7f )
                        {
                            if( position == LINE_LENGTH-1 )
                            {
                                putchar(BEL);
                            }
                            else
                            {
                                /* store character */
                                line[position] = c;
                                position++;

                                /* echo character */
                                putchar(c);
                            }
                        }
                        break;
            }
            break;

        case LINE_ESC:
            switch(c)
            {
                case '[':
                    state = LINE_ESC_SBO;
                    break;
                default:
                    state = LINE_RUNNING;
                    break;
            }
            break;

        case LINE_ESC_SBO:
            switch(c)
            {
                case 'C': /* cursor right */
                    if(line[position] == 0)
                    {
                        putchar(BEL);
                    }
                    else
                    {
                        printf("%s", CURSOR_RIGHT);
                        position++;
                    }
                    state = LINE_RUNNING;
                    break;
                case 'D': /* cursor left */
                    if(position == 0)
                    {
                        putchar(BEL);
                    }
                    else
                    {
                        printf("%s", CURSOR_LEFT);
                        position--;
                    }
                    state = LINE_RUNNING;
                    break;
                case '3': /* delete */
                    if(line[position] == 0)
                    {
                        putchar(BEL);
                    }
                    else
                    {
                        /* update terminal */
                        printf("%s", CURSOR_SAVE);
                        printf("%s", DELETE_TO_LINEEND);
                        printf("%s", &line[position+1]);
                        printf("%s", CURSOR_RESTORE);

                        /* update line buffer */
                        strcpy(&line[position], &line[position+1]);

                        /* next state */
                        state = LINE_ESC_SBO_TILDE;
                    }
                    break;
                case '2': /* insert */
                case '5': /* page up */
                case '6': /* page down */
                    state = LINE_ESC_SBO_TILDE;
                    break;
                default:
                    state = LINE_RUNNING;
                    break;
            }
            break;

        case LINE_ESC_SBO_TILDE:
            state = LINE_RUNNING;
            break;

        case LINE_FULL:
            /* nothing to do here */
            /* exit after external read of buffer */
            break;
    }
    return 0; //TODO
}

/*************************************************************************
Purpose: reads string from internal buffer, if available
Input  : s - pointer to external buffer
Return : !=NULL - ponter to assembeled line
         ==NULL - line not ready
**************************************************************************/
char* line_gets (char* s)
{
    if(state == LINE_FULL)
    {
        strcpy(s, line);
        memset(line, 0, LINE_LENGTH);
        position = 0;
        state = LINE_RUNNING;
        return s;
    }

    return NULL;
}

