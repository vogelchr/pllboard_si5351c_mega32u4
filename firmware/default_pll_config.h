#ifndef DEFAULT_PLL_CONFIG_H
#define DEFAULT_PLL_CONFIG_H

#include <avr/pgmspace.h>

struct si5351c_reg
{
	uint8_t addr;
	uint8_t val;
};

extern const struct si5351c_reg si5351c_default_config[] PROGMEM;
extern const uint8_t si5351c_default_config_nelems;

#endif
