/**
 * @file MAX6675.h
 * @brief Interface for the MAX6675 thermocouple-to-digital converter.
 *
 * This module provides a function to retrieve temperature measurements from the
 * MAX6675 thermocouple-to-digital converter using SPI communication.
 *
 * @note
 * Hardware connections:
 * - **VCC:** 3.3V or 5V
 * - **GND:** Ground
 * - **SCK:** SPI1_SCK (PA5)
 * - **CS:**  GPIO (PA9)
 * - **SO:**  SPI1_MISO (PA6)
 *
 * @author Matthew Ross 200265265
 *
 * @version 1.0
 * @date    17apr2025
 */

#ifndef MAX6675_H
#define MAX6675_H

#include "stdint.h" // uint_t types

/**
 * @brief Reads the temperature from the MAX6675 thermocouple interface.
 *
 * This function initiates SPI communication with the MAX6675 by pulling the CS line low,
 * receives two bytes of data from the device, processes the data by shifting out unused bits,
 * and then converts the raw value to a temperature in degrees Celsius using the scaling factor
 * (0.25°C per count).
 *
 * @return Temperature in degrees Celsius.
 */
float getMAX6675(void);

#endif
