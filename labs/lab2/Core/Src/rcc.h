#ifndef RCC_H
#define RCC_H

/**
 * FILENAME: rcc.h
 *
 * DESCRIPTION:
 * This gets the RCC config and prints it to the UART console.
 *
 * AUTHOR: Matthew Ross 200265265
 */

#include "main.h"
#include "stdlib.h"
#include "stdio.h"
extern UART_HandleTypeDef huart2;

/**
 * This prints the system clock source to UART.
 */
void clock_source(void);

/**
 * This prints the RCC config to UART.
 */
void rcc_status(void);

#endif
