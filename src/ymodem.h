#ifndef _FYMODEM_H_
#define _FYMODEM_H_

/**
 * Free YModem implementation.
 *
 * Fredrik Hederstierna 2014
 *
 * This file is in the public domain.
 * You can do whatever you want with it.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
 
// MB: the ymodem code in f/term is straight from this public domain source: 
// https://github.com/fredrikhederstierna/fymodem
//   only changes made were the glue to f/term (and I renamed from fymodem to ymodem)

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

/* max length of filename */
#define FYMODEM_FILE_NAME_MAX_LENGTH  (64)

/* receive file over ymodem */
int32_t fymodem_receive(uint8_t *rxdata,
                        size_t rxsize,
                        char filename[FYMODEM_FILE_NAME_MAX_LENGTH]);

/* send file over ymodem */
int32_t fymodem_send(uint8_t *txdata,
                     size_t txsize,
                     const char *filename);

#endif