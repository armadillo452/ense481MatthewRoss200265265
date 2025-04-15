#ifndef MAX6675_H
#define MAX6675_H

// VCC => 3.3V or 5V
// GND => GND
// SCK => SPI1_SCK (PA5)
// CS  => GPIO (PA9)
// SO  => SPI1_MISO (PA6)

#include "stdint.h" // uint_t types

float getMAX6675(void);

#endif
