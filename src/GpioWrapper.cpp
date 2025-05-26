//-----------------------------------------------------------------------
//  File        : GpioWrapper.cpp
//  Created     : 23.10.2022
//  Modified    : 10.12.2022
//  Author      : V-Nezlo (vlladimirka@gmail.com)
//  Description : ООП обертка для gpio

#include "Gpio.hpp"

Gpio::Gpio(uint64_t aNum, gpio_mode_t aMode, bool aState, gpio_pullup_t aPullUp, gpio_pulldown_t aPullDown, gpio_int_type_t aIntType) :
	config{
		static_cast<uint64_t>(1 << aNum),
		aMode,
		aPullUp,
		aPullDown,
		aIntType
	},
	number{static_cast<gpio_num_t>(aNum)}
{
	gpio_config(&config);
	gpio_set_level(number, aState);
}

Gpio::~Gpio()
{
	gpio_reset_pin(number);
}

void Gpio::set()
{
	gpio_set_level(number, true);
}

void Gpio::reset()
{
	gpio_set_level(number, false);
}

void Gpio::setState(bool aState)
{
	gpio_set_level(number, aState);
}

bool Gpio::read()
{
	return static_cast<bool>(gpio_get_level(number));
}
