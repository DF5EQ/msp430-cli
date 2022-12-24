/* ===== file header ===== */

#ifndef TERMINAL_H
#define TERMINAL_H

/* ===== includes ===== */
#include <msp430.h>

/* ===== public datatypes ===== */

/* ===== public symbols ===== */

/* ===== public constants ===== */

/* ===== public variables ===== */

/* ===== public functions ===== */
void terminal_init (void);
int  terminal_putc (char c);
int  terminal_gets (char* s);

#endif

