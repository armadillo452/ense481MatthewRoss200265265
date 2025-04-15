#include "MAX6675.h"
#include "main.h"          // extern hspi1
#include "stm32f1xx_hal.h" // port and pin defines
#include "stdint.h"        // uint_t types

extern SPI_HandleTypeDef hspi1;

#define MAX6675_CS_PIN GPIO_PIN_9
#define MAX6675_CS_PORT GPIOA

float getMAX6675() {
	uint8_t data[2];  // Receive buffer
	uint16_t temp;
	HAL_GPIO_WritePin(MAX6675_CS_PORT, MAX6675_CS_PIN, GPIO_PIN_RESET); // Select MAX6675 (CS Low)
	HAL_SPI_Receive(&hspi1, data, 2, HAL_MAX_DELAY); // Read 2 bytes from SPI
	temp = ((data[0] << 8) | data[1]) >> 3; // Shift to remove unused bits
	HAL_GPIO_WritePin(MAX6675_CS_PORT, MAX6675_CS_PIN, GPIO_PIN_SET); // Deselect MAX6675 (CS High)
	return ((float) temp * 0.25); // Temperature in Celsius
}
