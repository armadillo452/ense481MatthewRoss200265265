/**
 * FILENAME: pwm_button_blinky.c
 *
 * DESCRIPTION:
 * This program allows a button to control the brightness of the LED. The
 * system should accept six button presses that cycle the brightness
 *  through these states:
 *  1. off
 *  2. 20% brightness
 *  3. 40% brightness
 *  4. 60% brightness
 *  5. 80% brightness
 *  6. full brightness
 *  A subsequent press should bring the state back to "off".
 *
 * AUTHOR: Matthew Ross 200265265
 */

#include "main.h"
#include "pwm_button_blinky.h"

uint8_t mode_global = 1; // Initialize LED duty cycle mode

void blinky() {
	uint8_t mode;
	while (1) {
		mode = mode_global; // Check the global mode
		if (mode == 1) // 0% duty
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		else if (mode == 2) { // 20% duty
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
			HAL_Delay(2);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
			HAL_Delay(8);
		} else if (mode == 3) { // 40% duty
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
			HAL_Delay(4);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
			HAL_Delay(6);
		} else if (mode == 4) { // 60% duty
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
			HAL_Delay(6);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
			HAL_Delay(4);
		} else if (mode == 5) { // 80% duty
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
			HAL_Delay(8);
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
			HAL_Delay(2);
		} else if (mode == 6)  // 100% duty
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		else
			// Invalid mode, turn off LED
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_13) { // Button press interrupt
		if (mode_global != 6) // If not max duty
			mode_global++; // Increase duty mode
		else
			mode_global = 1; // Else go back to mode 1
	}
}
