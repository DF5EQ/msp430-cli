/* ===== file header ===== */

#ifndef LINE_H
#define LINE_H

/* ===== includes ===== */
#include <msp430.h>
#include "uart.h" // TODO would be good to not need this

/* ===== public datatypes ===== */

/* ===== public symbols ===== */

/* ===== public constants ===== */

/* ===== public variables ===== */

/* ===== public functions ===== */

/* ----- consumed functions -----*/
extern int getchar (void);
extern int putchar (int c);

/* ----- provided functions ----- */
void  line_init (void);
int   line_putc (char c);
char* line_gets (char* s);

#endif

