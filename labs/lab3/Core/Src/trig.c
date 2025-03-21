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
uint16_t built_in_sine(float radians) {
	volatile float dump64 = 0;
	(void) dump64; // Shutup unused warning
	uint16_t start_time = timer_start();
	dump64 = sin(radians);
	return timer_stop(start_time);
}

/**
 * This does a benchmark with the built-in cosine function.
 */
uint16_t built_in_cosine(float radians) {
	volatile float dump64 = 0;
	(void) dump64; // Shutup unused warning
	uint16_t start_time = timer_start();
	dump64 = cos(radians);
	return timer_stop(start_time);
}

/**
 * Sine integer Taylor series implementation with 4 terms, as described in
 * Crenshaw, ch5, Listing 5.6, p118
 */
uint16_t function_sin(float radians) {
	// y is measured in pirads
	volatile int16_t y = (int16_t) ((radians) * (1 << 15) / M_PI);
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
uint16_t function_cos(float radians) {
	// y is measured in pirads
	volatile int16_t y = (int16_t) ((radians) * (1 << 15) / M_PI);
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
uint64_t err_sine(float radians) {
	int64_t fixed_sine = (int64_t) (sin(radians) * (1 << 15));
	// y is measured in pirads
	int16_t y = (int16_t) (radians * (1 << 15) / M_PI);
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
	if (fixed_sine == 0)
		return 0;
	int64_t ppm_error = (error * 1000000) / abs(fixed_sine);
	return abs(ppm_error);
}

/**
 * This does a benchmark to find the taylor series error.
 */
uint64_t err_cosine(float radians) {
	int64_t fixed_cosine = (int64_t) (cos(radians) * (1 << 15));
	// y is measured in pirads
	int16_t y = (int16_t) (radians * (1 << 15) / M_PI);
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
	if (fixed_cosine == 0)
		return 0;
	int64_t ppm_error = (error * 1000000) / abs(fixed_cosine);
	return abs(ppm_error);
}

/**
 * This will run all the tests 361 times and take the average.
 */
void run_trig_test() {
	// Initialize counters
	volatile uint32_t sum_sine = 0;
	volatile uint32_t min_sine = 0;
	volatile uint32_t max_sine = 0;
	volatile uint32_t sum_cosine = 0;
	volatile uint32_t min_cosine = 0;
	volatile uint32_t max_cosine = 0;
	volatile uint32_t sum_taylor_sine = 0;
	volatile uint32_t min_taylor_sine = 0;
	volatile uint32_t max_taylor_sine = 0;
	volatile uint32_t sum_taylor_cosine = 0;
	volatile uint32_t min_taylor_cosine = 0;
	volatile uint32_t max_taylor_cosine = 0;
	volatile uint32_t sum_err_sine = 0;
	volatile uint32_t min_err_sine = 0;
	volatile uint32_t max_err_sine = 0;
	volatile uint32_t sum_err_cosine = 0;
	volatile uint32_t min_err_cosine = 0;
	volatile uint32_t max_err_cosine = 0;
	static uint8_t TX_buffer[40] = ""; // init buffer for transmit
	const uint8_t UART_timeout = 200; // transmit timeout
	// Run 100 iterations for all the tests
	for (int i = 0; i <= 360; i++) {
		float radians = degToRad(i);
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

		volatile uint32_t current_err_sine = err_sine(radians);
		sum_err_sine += current_err_sine;
		if (min_err_sine > current_err_sine) {
			min_err_sine = current_err_sine;
		}
		if (current_err_sine > max_err_sine) {
			max_err_sine = current_err_sine;
		}

		// Transmit the really bad values test
//		if (current_err_sine > 1000000) {
//			sprintf((char*) TX_buffer, "bad sine %d = %d PPM \n\r", i, current_err_sine);
//			HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
//			for (int i = 0; i <= 2000000; i++) {;}
//		}

		volatile uint32_t current_err_cosine = err_cosine(radians);
		sum_err_cosine += current_err_cosine;
		if (min_err_cosine > current_err_cosine) {
			min_err_cosine = current_err_cosine;
		}
		if (current_err_cosine > max_err_cosine) {
			max_err_cosine = current_err_cosine;
		}

		// Transmit the really bad values test
//		if (current_err_cosine > 1000000) {
//			sprintf((char*) TX_buffer, "bad cosine %d = %d PPM \n\r", i, current_err_cosine);
//			HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
//			for (int i = 0; i <= 2000000; i++) {;}
//		}

	}
	// Divide the results by 361 to get average clocks
#define divisor 361
	uint32_t avg_sine = sum_sine / divisor;
	uint32_t avg_cosine = sum_cosine / divisor;
	uint32_t avg_taylor_sine = sum_taylor_sine / divisor;
	uint32_t avg_taylor_cosine = sum_taylor_cosine / divisor;
	uint32_t avg_err_sine = sum_err_sine / divisor;
	uint32_t avg_err_cosine = sum_err_cosine / divisor;
	// Transmit the results
	const uint8_t test[] =
			"The avg of 361 iterations (0 to 360 degrees) in clock cycles is: \n\n\r";
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "built-in sine   avg = %lu \n\r", avg_sine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "                min = %lu \n\r", min_sine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "                max = %lu \n\r", max_sine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "built-in cosine avg = %lu \n\r", avg_cosine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "                min = %lu \n\r", min_cosine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "                max = %lu \n\r", max_cosine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "taylor sine     avg = %lu \n\r",
			avg_taylor_sine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "                min = %lu \n\r",
			min_taylor_sine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "                max = %lu \n\r",
			max_taylor_sine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "taylor cosine   avg = %lu \n\r",
			avg_taylor_cosine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "                min = %lu \n\r",
			min_taylor_cosine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "                max = %lu \n\n\r",
			max_taylor_cosine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "cos error ppm   avg = %lu \n\r",
			avg_err_cosine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "                min = %lu    \n\r", min_err_cosine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "                max = %lu \n\r",
			max_err_cosine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "sine error ppm  avg = %lu \n\r", avg_err_sine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "                min = %lu    \n\r", min_err_sine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
	sprintf((char*) TX_buffer, "                max = %lu \n\r", max_err_sine);
	HAL_UART_Transmit(&huart2, TX_buffer, 40, UART_timeout);
}
//
//void test_err_sine() {
//#define degToRad(angleInDegrees) ((angleInDegrees) * M_PI / 180.0)
//	float radians = degToRad(89);
//	float fixed_sine = sin(radians);
//	myprintf("internal sine %.3f\r\n", fixed_sine);
//	myprintf("fixed sine %.3f\r\n", fixed_sine * (1 << 15));
//	// y is the angle measure in pirads
//	int16_t y = (int16_t)((radians ) / M_PI * (1 << 15));
//	static short s1 = 0x6488;
//	static short s3 = 0x2958;
//	static short s5 = 0x51a;
//	static short s7 = 0x4d;
//	long z, prod, sum;
//	z = ((long) y * y) >> 12;
//	prod = (z * s7) >> 16;
//	sum = s5 - prod;
//	prod = (z * sum) >> 16;
//	sum = s3 - prod;
//	prod = (z * sum) >> 16;
//	sum = s1 - prod;
//	// for better accuracy, round here
//	int16_t taylor_sine = (short) ((y * sum) >> 13);
//	myprintf("taylor sine %d\r\n", taylor_sine);
//	// Now get the error
////	int64_t error = abs(taylor_sine - fixed_sine);
////	int64_t ppm_error = (error * 1000000) / abs(fixed_sine);
////	return ppm_error;
//}
