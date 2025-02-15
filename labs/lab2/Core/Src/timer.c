/**
 * FILENAME: timer.c
 *
 * DESCRIPTION:
 * This is a test suite to test a bunch of different operations and then get
 * the average time to execute.  The results are transmitted to the UART CLI.
 *
 * AUTHOR: Matthew Ross 200265265
 */

#include "timer.h"
#include "cli.h"
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim2;

typedef struct byte8 { // 8 byte struct for assignment copy
	uint8_t byte_array[8];
} byte8;

typedef struct byte128 { // 128 byte struct for assignment copy
	uint8_t byte_array[128];
} byte128;

typedef struct byte1024 { // 1024 byte struct for assignment copy
	uint8_t byte_array[1024];
} byte1024;

/**
 * timer_start() returns the current value of the timer as a 16-bit signed int.
 */
int16_t timer_start() {
	int16_t start_time = __HAL_TIM_GET_COUNTER(&htim2);
	return start_time;
}

/**
 * timer_stop() takes a int16_t (obtained from a previous call to timer_start)
 * and compares it to the current timer value. It returns the number of timer
 * ticks since the supplied start_time. Your function must operate correctly
 * even if the timer wrapped around from 0x0000 to 0xFFFF in that interval.
 * You do not need to handle more than 1 wraparound. So, your timer is
 * required to handle time intervals from 0 to 65535 clocks.
 */
int16_t timer_stop(int16_t start_time) {
	int16_t stop_time = start_time - __HAL_TIM_GET_COUNTER(&htim2);
	return stop_time;
}

/**
 * Generate a random 64-bit integer
 */
int64_t rand64() {
	uint64_t number = (((uint64_t) (rand()) << 32) | rand());
	return number;
}

/**
 * Generate a random 8-bit integer
 */
int64_t rand8() {
	uint8_t number = ((uint8_t) rand());
	return number;
}

/**
 * add two random 32-bit integers
 */
int16_t rand_add32() {
	int32_t rand1 = rand();
	int32_t rand2 = rand();
	volatile int32_t dump32;
	(void) dump32; // Shutup unused warning
	int16_t start_time = timer_start();
	dump32 = rand1 + rand2;
	return timer_stop(start_time);
}

/**
 * add two random 64-bit integers
 */
int16_t rand_add64() {
	int64_t rand1 = rand64();
	int64_t rand2 = rand64();
	volatile int64_t dump64;
	(void) dump64; // Shutup unused warning
	int16_t start_time = timer_start();
	dump64 = rand1 + rand2;
	return timer_stop(start_time);
}

/**
 * multiply two random 32-bit integers
 */
int16_t rand_mult32() {
	int32_t rand1 = rand();
	int32_t rand2 = rand();
	volatile int32_t dump32;
	(void) dump32; // Shutup unused warning
	int16_t start_time = timer_start();
	dump32 = rand1 * rand2;
	return timer_stop(start_time);
}

/**
 * multiply two random 64-bit integers
 */
int16_t rand_mult64() {
	int64_t rand1 = rand64();
	int64_t rand2 = rand64();
	volatile int64_t dump64;
	(void) dump64; // Shutup unused warning
	int16_t start_time = timer_start();
	dump64 = rand1 * rand2;
	return timer_stop(start_time);
}

/**
 * divide two random 32-bit integers
 */
int16_t rand_div32() {
	int32_t rand1 = rand();
	int32_t rand2 = rand();
	volatile int32_t dump32;
	(void) dump32; // Shutup unused warning
	int16_t start_time = timer_start();
	if (rand2 == 0)
		return timer_stop(start_time);
	dump32 = rand1 / rand2;
	return timer_stop(start_time);
}

/**
 * divide two random 64-bit integers
 */
int16_t rand_div64() {
	int64_t rand1 = rand64();
	int64_t rand2 = rand64();
	volatile int64_t dump64;
	(void) dump64; // Shutup unused warning
	int16_t start_time = timer_start();
	if (rand2 == 0)
		return timer_stop(start_time);
	dump64 = rand1 / rand2;
	return timer_stop(start_time);
}

/**
 * copy an 8-byte struct using the assignment operator
 */
uint16_t copy_byte8() {
	volatile byte8 thing1, thing2;
	(void) thing2;  // Shutup unused warning
	for (int i = 0; i < 8; i++) {
		thing1.byte_array[i] = rand8();
	}
	int16_t start_time = timer_start();
	thing2 = thing1;
	return timer_stop(start_time);
}

/**
 * copy an 128-byte struct using the assignment operator
 */
uint16_t copy_byte128() {
	volatile byte128 thing1, thing2;
	(void) thing2; // Shutup unused warning
	for (int i = 0; i < 128; i++) {
		thing1.byte_array[i] = rand8();
	}
	int16_t start_time = timer_start();
	thing2 = thing1;
	return timer_stop(start_time);
}

/**
 * copy an 1024-byte struct using the assignment operator
 */
uint16_t copy_byte1024() {
	volatile byte1024 thing1, thing2;
	(void) thing2; // Shutup unused warning
	for (int i = 0; i < 1024; i++) {
		thing1.byte_array[i] = rand8();
	}
	int16_t start_time = timer_start();
	thing2 = thing1;
	return timer_stop(start_time);
}

/**
 * This will run all the tests 100 times and take the average. The time to
 * generate random numbers in not included. Made sure to include divide by
 * zero check.
 */
void run_test() {
	// Initialize counters
	int add32 = 0;
	int add64 = 0;
	int mult32 = 0;
	int mult64 = 0;
	int div32 = 0;
	int div64 = 0;
	int byte8 = 0;
	int byte128 = 0;
	int byte1024 = 0;
	// Run 100 iterations for all the tests
	for (int i = 0; i < 100; i++) {
		add32 += rand_add32();
		add64 += rand_add64();
		mult32 += rand_mult32();
		mult64 += rand_mult64();
		div32 += rand_div32();
		div64 += rand_div64();
		byte8 += copy_byte8();
		byte128 += copy_byte128();
		byte1024 += copy_byte1024();
	}

	// Run 100 iterations for a specific test
	// Test with external logic analyzer on the pin
	// Divide the result by 100
	// Make sure to uncomment the next block for test

//	volatile int32_t rand1 = rand();
//	volatile int32_t rand2 = rand();
//	volatile int32_t dump32;
//	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // Toggle onboard green LED
//	for (int i = 0; i < 100; i++) {
//		dump32 = rand1 + rand2;
//	}
//	HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // Toggle onboard green LED
//	(void) dump32; // Shutup warning

	// Divide the results by 100 to get average
	// Divide by 64 on 64MHz clock to get microseconds
	// Multiply by 1000 to get nanoseconds
	// 64*100/1000=6.4
	add32 /= 6.4;
	add64 /= 6.4;
	mult32 /= 6.4;
	mult64 /= 6.4;
	div32 /= 6.4;
	div64 /= 6.4;
	byte8 /= 6.4;
	byte128 /= 6.4;
	byte1024 /= 6.4;
	// Transmit the results
	const uint8_t UART_timeout = 100;
	const uint8_t test[] =
			"The avg of 100 iterations at 64MHz in nanoseconds is: \n\r";
	for (int i = 0; i < sizeof(test); i++) { // test intro msg
		HAL_UART_Transmit(&huart2, test + i, 1, UART_timeout);
	}
	uint8_t TX_buffer[40] = ""; // init buffer
	sprintf((char*) TX_buffer, "add32 = %d \n\r", add32); // add32 test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "add64 = %d \n\r", add64); // add64 test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "mult32 = %d \n\r", mult32); // mult32 test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "mult64 = %d \n\r", mult64); // mult64 test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "div32 = %d \n\r", div32); // div32 test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "div64 = %d \n\r", div64); // div64 test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "byte8 = %d \n\r", byte8); // byte8 test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "byte128 = %d \n\r", byte128); // byte128 test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "byte1024 = %d \n\r", byte1024); // byte1024 test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
}

