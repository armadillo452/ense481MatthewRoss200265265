#ifndef ADC_H
#define ADC_H

#include "stdint.h" // uint_t types

float getTemp(void);
float getVolt(void);
void adc_config_temp(void);
void adc_config_vref(void);
uint16_t adc_sample(uint32_t channel);

#endif
