/**
 * FILENAME: trig.h
 *
 * DESCRIPTION:
 * This runs a benchmark to compare the built in sine and cosine function to
 * integer taylor series implementation with 4 terms.
 *
 * AUTHOR: Matthew Ross 200265265
 */

#include "timer.h"
#include "trig.h"
#include "cli.h"
#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0 / M_PI)
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim2;

/**
 * This does a benchmark with the built-in sine function.
 */
uint16_t built_in_sine(int64_t radians) {
	volatile int64_t dump64 = 0;
	(void) dump64; // Shutup unused warning
	uint16_t start_time = timer_start();
	dump64 = sin(radians);
	return timer_stop(start_time);
}

/**
 * This does a benchmark with the built-in cosine function.
 */
uint16_t built_in_cosine(int64_t radians) {
	volatile int64_t dump64 = 0;
	(void) dump64; // Shutup unused warning
	uint16_t start_time = timer_start();
	dump64 = cos(radians);
	return timer_stop(start_time);
}

/**
 * Sine integer Taylor series implementation with 4 terms, as described in
 * Crenshaw, ch5, Listing 5.6, p118
 */
uint16_t function_sin(int64_t radians) {
	volatile int16_t y = radians;
	volatile int16_t dump32;
	(void) dump32; // Shutup unused warning
	uint16_t start_time = timer_start();
	static short s1 = 0x6488;
	static short s3 = 0x2958;
	static short s5 = 0x51a;
	static short s7 = 0x4d;
	long z, prod, sum;
	z = ((long) y * y) >> 12;
	prod = (z * s7) >> 16;
	sum = s5 - prod;
	prod = (z * sum) >> 16;
	sum = s3 - prod;
	prod = (z * sum) >> 16;
	sum = s1 - prod;
	// for better accuracy, round here
	dump32 = (short) ((y * sum) >> 13);
	return timer_stop(start_time);
}

/**
 * Cosine integer Taylor series implementation with 4 terms, as described in
 * Crenshaw, ch5, Listing 5.6, p118
 */
uint16_t function_cos(int64_t radians) {
	volatile int16_t y = radians;
	volatile int16_t dump16;
	(void) dump16; // Shutup unused warning
	uint16_t start_time = timer_start();
	static short c0 = 0x7fff;
	static short c2 = 0x4ef5;
	static short c4 = 0x103e;
	static short c6 = 0x156;
	long z, prod, sum;
	z = ((long) y * y) >> 12;
	prod = (z * c6) >> 16;
	sum = c4 - prod;
	prod = (z * sum) >> 16;
	sum = c2 - prod;
	// for better accuracy, round here
	prod = (z * sum) >> 15; // note, not 16
	dump16 = (short) (c0 - prod);
	return timer_stop(start_time);
}

/**
 * This does a benchmark to find the taylor series error.
 */
uint16_t err_sine(int64_t radians) {
	int64_t fixed_sine = (int64_t) (sin(radians) * (1 << 13));
	int16_t y = radians * 4096;
	static short s1 = 0x6488;
	static short s3 = 0x2958;
	static short s5 = 0x51a;
	static short s7 = 0x4d;
	long z, prod, sum;
	z = ((long) y * y) >> 12;
	prod = (z * s7) >> 16;
	sum = s5 - prod;
	prod = (z * sum) >> 16;
	sum = s3 - prod;
	prod = (z * sum) >> 16;
	sum = s1 - prod;
	// for better accuracy, round here
	int16_t taylor_sine = (short) ((y * sum) >> 13);
	// Now get the error
	int64_t error = abs(taylor_sine - fixed_sine);
	int64_t ppm_error = (error * 1000000) / abs(fixed_sine);
	return ppm_error;
}

/**
 * This does a benchmark to find the taylor series error.
 */
uint16_t err_cosine(int64_t radians) {
	int64_t fixed_cosine = (int64_t) (cos(radians) * (1 << 13));
	int16_t y = radians * 4096;
	static short c0 = 0x7fff;
	static short c2 = 0x4ef5;
	static short c4 = 0x103e;
	static short c6 = 0x156;
	long z, prod, sum;
	z = ((long) y * y) >> 12;
	prod = (z * c6) >> 16;
	sum = c4 - prod;
	prod = (z * sum) >> 16;
	sum = c2 - prod;
	// for better accuracy, round here
	prod = (z * sum) >> 15; // note, not 16
	int16_t taylor_cosine = (short) (c0 - prod);
	// Now get the error
	int64_t error = abs(taylor_cosine - fixed_cosine);
	int64_t ppm_error = (error * 1000000) / abs(fixed_cosine);
	return ppm_error;
}

/**
 * This will run all the tests 361 times and take the average. The time to
 * generate random numbers in not included.
 */
void run_trig_test() {
	// Initialize counters
	uint32_t sum_sine = 0;
	uint32_t min_sine = 0;
	uint32_t max_sine = 0;
	uint32_t sum_cosine = 0;
	uint32_t min_cosine = 0;
	uint32_t max_cosine = 0;
	uint32_t sum_taylor_sine = 0;
	uint32_t min_taylor_sine = 0;
	uint32_t max_taylor_sine = 0;
	uint32_t sum_taylor_cosine = 0;
	uint32_t min_taylor_cosine = 0;
	uint32_t max_taylor_cosine = 0;
	uint32_t sum_err_sine = 0;
	uint32_t min_err_sine = 0;
	uint32_t max_err_sine = 0;
	uint32_t sum_err_cosine = 0;
	uint32_t min_err_cosine = 0;
	uint32_t max_err_cosine = 0;
	// Run 100 iterations for all the tests
	for (int i = 0; i <= 360; i++) {
		int64_t radians = degToRad(i);
		sum_sine += built_in_sine(radians);
		if (min_sine == 0) {
			min_sine = built_in_sine(radians);
		} else if (built_in_sine(radians) < min_sine) {
			min_sine = built_in_sine(radians);
		}
		if (built_in_sine(radians) > max_sine) {
			max_sine = built_in_sine(radians);
		}
		sum_cosine += built_in_cosine(radians);
		if (min_cosine == 0) {
			min_cosine = built_in_cosine(radians);
		} else if (built_in_cosine(radians) < min_cosine) {
			min_cosine = built_in_cosine(radians);
		}
		if (built_in_cosine(radians) > max_cosine) {
			max_cosine = built_in_cosine(radians);
		}
		sum_taylor_sine += function_sin(radians);
		if (min_taylor_sine == 0) {
			min_taylor_sine = function_sin(radians);
		} else if (function_sin(radians) < min_taylor_sine) {
			min_taylor_sine = function_sin(radians);
		}
		if (function_sin(radians) > max_taylor_sine) {
			max_taylor_sine = function_sin(radians);
		}
		sum_taylor_cosine += function_cos(radians);
		if (min_taylor_cosine == 0) {
			min_taylor_cosine = function_cos(radians);
		} else if (function_cos(radians) < min_taylor_cosine) {
			min_taylor_cosine = function_cos(radians);
		}
		if (function_cos(radians) > max_taylor_cosine) {
			max_taylor_cosine = function_cos(radians);
		}
		sum_err_sine += err_sine(radians);
		if (min_err_sine == 0) {
			min_err_sine = err_sine(radians);
		} else if (err_sine(radians) < min_err_sine) {
			min_err_sine = err_sine(radians);
		}
		if (err_sine(radians) > max_err_sine) {
			max_err_sine = err_sine(radians);
		}
		sum_err_cosine += err_cosine(radians);
		if (min_err_cosine == 0) {
			min_err_cosine = err_cosine(radians);
		} else if (err_cosine(radians) < min_err_cosine) {
			min_err_cosine = err_cosine(radians);
		}
		if (err_cosine(radians) > max_err_cosine) {
			max_err_cosine = err_cosine(radians);
		}

	}
	// Divide the results by 361 to get average clocks
	uint32_t avg_sine = sum_sine / 361;
	uint32_t avg_cosine = sum_cosine / 361;
	uint32_t avg_taylor_sine = sum_taylor_sine / 361;
	uint32_t avg_taylor_cosine = sum_taylor_cosine / 361;
	uint32_t avg_err_sine = sum_err_sine / 361;
	uint32_t avg_err_cosine = sum_err_cosine / 361;
	// Transmit the results
	const uint8_t UART_timeout = 100;
	const uint8_t test[] =
			"The avg of 361 iterations (0 to 360 degrees) in clock cycles is: \n\n\r";
	for (int i = 0; i < sizeof(test); i++) { // test intro msg
		HAL_UART_Transmit(&huart2, test + i, 1, UART_timeout);
	}
	static uint8_t TX_buffer[40] = ""; // init buffer
	sprintf((char*) TX_buffer, "built-in sine   avg = %lu \n\r", avg_sine); // built in sine test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "                min = %lu \n\r", min_sine); // built in sine test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "                max = %lu \n\r", max_sine); // built in sine test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "built-in cosine avg = %lu \n\r", avg_cosine); // built in cosine test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "                min = %lu \n\r", min_cosine); // built in sine test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "                max = %lu \n\r", max_cosine); // built in sine test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "taylor sine     avg = %lu \n\r",
			avg_taylor_sine); // avg taylor sine test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "                min = %lu \n\r",
			min_taylor_sine); // min taylor sine test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "                max = %lu \n\r",
			max_taylor_sine); // max taylor sine test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "taylor cosine   avg = %lu \n\r",
			avg_taylor_cosine); // avg taylor cosine test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "                min = %lu \n\r",
			min_taylor_cosine); // min taylor cosine test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "                max = %lu \n\n\r",
			max_taylor_cosine); // max taylor cosine test
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "cos error ppm   avg = %lu \n\r",
			avg_err_cosine); // avg error of sine ppm
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "                min = %lu \n\r",
			min_err_cosine); // min error of sine ppm
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "                max = %lu \n\r",
			max_err_cosine); // max error of sine ppm
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "sine error ppm  avg = %lu \n\r", avg_err_sine); // avg error of sine ppm
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "                min = %lu \n\r", min_err_sine); // min error of sine ppm
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
	sprintf((char*) TX_buffer, "                max = %lu \n\r", max_err_sine); // max error of sine ppm
	for (int i = 0; i < sizeof(TX_buffer); i++) {
		HAL_UART_Transmit(&huart2, TX_buffer + i, 1, UART_timeout);
	}
}
