/**
 * @file adc.h
 * @brief ADC conversion and configuration functions.
 *
 * This header file declares functions for configuring the ADC,
 * sampling channels, and converting ADC readings to temperature
 * and voltage values.
 *
 * @author Matthew Ross 200265265
 *
 * @version 1.0
 * @date    17apr2025
 */

#ifndef ADC_H
#define ADC_H

#include "stdint.h" // uint_t types

/**
 * @brief Retrieves the temperature measurement.
 *
 * This function reads the built-in temperature sensor by taking 100 consecutive ADC samples.
 * For each sample, it configures the ADC for temperature measurement, reads the ADC value,
 * converts that value to a voltage using the current supply voltage, and computes the temperature
 * in degrees Celsius via a calibration formula.
 *
 * @return The averaged temperature in degrees Celsius.
 */
float getTemp(void);

/**
 * @brief Retrieves the supply voltage (VDD) measurement.
 *
 * This function configures the ADC for measuring the internal reference voltage, performs a single-channel
 * ADC sample, and computes the supply voltage (VDD) using a calibration value. If the ADC sample is zero,
 * the function returns 0 to avoid division by zero.
 *
 * @return The measured supply voltage in volts.
 */
float getVolt(void);

/**
 * @brief Configures the ADC channels for temperature measurement.
 *
 * This function sets the ADC channel for the temperature sensor as the active conversion channel and
 * disables the internal reference voltage channel.
 */
void adc_config_temp(void);


/**
 * @brief Configures the ADC channels for internal reference voltage measurement.
 *
 * This function sets the ADC channel for the internal reference voltage as the active conversion channel and
 * disables the temperature sensor channel.
 */
void adc_config_vref(void);

/**
 * @brief Performs an ADC conversion on a specified channel.
 *
 * This function starts the ADC conversion on the provided channel, waits (blocking)
 * for the conversion to complete, retrieves the ADC value using HAL functions,
 * stops the conversion, and returns the raw value.
 *
 * @param channel The ADC channel to be sampled.
 *
 * @return The raw ADC conversion result as a 16-bit unsigned integer.
 */
uint16_t adc_sample(uint32_t channel);

#endif
