#ifndef TIMER_H
#define TIMER_H

/**
 * FILENAME: timer.h
 *
 * DESCRIPTION:
 * This is a test suite to test a bunch of different operations and then get
 * the average time to execute.  The results are transmitted to the UART CLI.
 *
 * AUTHOR: Matthew Ross 200265265
 */

#include "main.h"
#include "stdlib.h"
#include "stdio.h"
extern TIM_HandleTypeDef htim2;

/**
 * timer_start() returns the current value of the timer as a 16-bit signed int.
 */
uint16_t timer_start(void);

/**
 * timer_stop() takes a int16_t (obtained from a previous call to timer_start)
 * and compares it to the current timer value. It returns the number of timer
 * ticks since the supplied start_time. Your function must operate correctly
 * even if the timer wrapped around from 0x0000 to 0xFFFF in that interval.
 * You do not need to handle more than 1 wraparound. So, your timer is
 * required to handle time intervals from 0 to 65535 clocks.
 */
uint16_t timer_stop(int16_t start_time);

/**
 * Generate a random 64-bit number
 */
int64_t rand64(void);

/**
 * Generate a random 16-bit number
 */
int16_t rand16(void);

/**
 * Generate a random 8-bit number
 */
int8_t rand8(void);

/**
 * add two random 32-bit integers
 */
uint16_t rand_add32(void);

/**
 * add two random 64-bit integers
 */
uint16_t rand_add32(void);

/**
 * multiply two random 32-bit integers
 */
uint16_t rand_mult32(void);

/**
 * multiply two random 64-bit integers
 */
uint16_t rand_mult64(void);

/**
 * divide two random 32-bit integers
 */
uint16_t rand_div32(void);

/**
 * divide two random 64-bit integers
 */
uint16_t rand_div64(void);

/**
* copy an 8-byte struct using the assignment operator
*/
uint16_t copy_byte8(void);

/**
* copy an 128-byte struct using the assignment operator
*/
uint16_t copy_byte128(void);

/**
* copy an 1024-byte struct using the assignment operator
*/
uint16_t copy_byte1024(void);

/**
 * This will run all the tests 100 times and take the average. The time to
 * generate random numbers in not included. Made sure to include divide by
 * zero check.
 */
void run_test(void);

#endif
