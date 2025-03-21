#ifndef TRIG_H
#define TRIG_H

/**
 * FILENAME: trig.h
 *
 * DESCRIPTION:
 * This runs a benchmark to compare the built in sine and cosine function to
 * integer taylor series implementation with 4 terms.
 *
 * AUTHOR: Matthew Ross 200265265
 */

#include "main.h"
#include "stdlib.h"
#include "stdio.h"
#include "math.h"
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim2;

/**
 * This does a benchmark with the built-in sine function.
 */
uint16_t built_in_sine(float radians);

/**
 * This does a benchmark with the built-in cosine function.
 */
uint16_t built_in_cosine(float radians);

/**
 * This does a benchmark with taylor series to 4 terms for sine.
 */
uint16_t function_sin(float radians);

/**
 * This does a benchmark with taylor series to 4 terms for cosine.
 */
uint16_t function_cos(float radians);

uint64_t err_sine(float radians);

uint64_t err_cosine(float radians);

/**
 * This will run all the tests 100 times and take the average. The time to
 * generate random numbers in not included.
 */
void run_trig_test(void);

#endif
