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

#ifndef PRINTF_H_
#define PRINTF_H_

/* ===== includes ===== */
#include "uart.h"

/* ===== public datatypes ===== */

/* ===== public symbols ===== */
#define printf     printformat

/* ===== public constants ===== */

/* ===== public variables ===== */

/* ===== public functions ===== */
void printformat(char *format, ...);

#endif /* PRINTF_H_ */
