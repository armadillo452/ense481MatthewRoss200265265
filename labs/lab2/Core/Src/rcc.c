/**
 * FILENAME: rcc.c
 *
 * DESCRIPTION:
 * This gets the RCC config and prints it to the UART console.
 *
 * AUTHOR: Matthew Ross 200265265
 */

#include "rcc.h"

/**
 * This prints the system clock source to UART.
 */
void sysclk_source() {
	const uint8_t UART_timeout = 100;
	const uint8_t label[] = "  System clock source: ";
	const uint8_t pll[] = "PLL\n\r";
	const uint8_t hse[] = "HSE\n\r";
	const uint8_t hsi[] = "HSI\n\r";
	for (int i = 0; i < sizeof(label); i++) { // label msg
		HAL_UART_Transmit(&huart2, label + i, 1, UART_timeout);
	}
	if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_PLLCLK) {
		for (int i = 0; i < sizeof(pll); i++) { // HSE msg
			HAL_UART_Transmit(&huart2, pll + i, 1, UART_timeout);
		}
	} else if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_HSE) {
		for (int i = 0; i < sizeof(hse); i++) { // HSI msg
			HAL_UART_Transmit(&huart2, hse + i, 1, UART_timeout);
		}
	} else if (__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_HSI) {
		for (int i = 0; i < sizeof(hsi); i++) { // HSI msg
			HAL_UART_Transmit(&huart2, hsi + i, 1, UART_timeout);
		}
	}
}

/**
 * This prints the RCC config to UART.
 */
void rcc_status() {
	sysclk_source();
}
