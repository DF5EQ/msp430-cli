/* ===== file header ===== */
/*
 * Copyright (C) 2018  nhivp
 *               2022  Peter Bägel (DF5EQ)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef UART_H_
#define UART_H_

/* ===== includes ===== */
#include <msp430.h>

/* ===== public datatypes ===== */

/* ===== public symbols ===== */

/* ===== public constants ===== */

/* ===== public variables ===== */

/* ===== public functions ===== */
void  uart_init(void);
int   putchar (int byte);
int   uart_puts (char* s);
char* uart_gets (char* s);

#endif /* UART_H_ */
