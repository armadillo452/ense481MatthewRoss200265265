#include "adc.h"
#include "main.h"          // Extern hadc1
#include "cli.h"           // Myprintf
#include "stdint.h"        // uint_t types
#include "stm32f1xx_hal.h" // ADC channels
extern ADC_HandleTypeDef hadc1;

#define ADC_RANK_CHANNEL_NUMBER ((uint32_t)1)
#define ADC_RANK_NONE ((uint32_t)0)

float getTemp() {
	float temperature_tally = 0;
	for (int i = 0; i < 100; i++) {
		adc_config_temp();
		uint16_t adcValue = adc_sample(ADC_CHANNEL_TEMPSENSOR);
		// Temperature (in °C) = {(V25 - VSENSE ) / Avg_Slope} + 25
		// V25 (min is 1.34, max is 1.52, typical is 1.43)
		// Tested and doped to 1.373 for the specific chip I am testing
		float vSense = (adcValue * getVolt()) / 4095.0f;
		float temperature = (1.373f - vSense) / 0.0043f + 25.0f;
		temperature_tally += temperature;
	}
	return (temperature_tally / 100);
}

float getVolt() {
	adc_config_vref();
	uint16_t adcValue = adc_sample(ADC_CHANNEL_VREFINT);
	// VDD = (VREFIN_calibration * 4095.0f) / adcValue;
	// VREFIN_calibration should be about 1.2V, doped for 1.155
	if (adcValue == 0)
		return 0; // Make sure not to divide by zero
	float VDD = (1.155f * 4095.0f) / adcValue;
	return VDD;
}

static void config_ch(uint32_t channel, uint32_t rank) {
	ADC_ChannelConfTypeDef sConfig = { 0 };
	sConfig.Channel = channel;
	sConfig.Rank = rank;
	sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5; // Adjusted for clock
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		myprintf("Failed to configure channel\r\n");
	}
}

void adc_config_temp(void) {
	config_ch(ADC_CHANNEL_TEMPSENSOR, ADC_RANK_CHANNEL_NUMBER);
	config_ch(ADC_CHANNEL_VREFINT, ADC_RANK_NONE);
}

void adc_config_vref(void) {
	config_ch(ADC_CHANNEL_VREFINT, ADC_RANK_CHANNEL_NUMBER);
	config_ch(ADC_CHANNEL_TEMPSENSOR, ADC_RANK_NONE);
}

uint16_t adc_sample(uint32_t channel) {
	uint16_t result;
	if (HAL_ADC_Start(&hadc1) != HAL_OK)
		myprintf("Failed to start ADC for single sample\r\n");
	if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) != HAL_OK)
		myprintf("ADC conversion didn't complete\r\n");
	result = HAL_ADC_GetValue(&hadc1);
	if (HAL_ADC_Stop(&hadc1) != HAL_OK)
		myprintf("Failed to stop ADC\r\n");
	return result;
}
