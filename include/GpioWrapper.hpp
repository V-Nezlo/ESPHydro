//-----------------------------------------------------------------------
//  File        : GpioWrapper.hpp
//  Created     : 6.10.2022
//  Modified    : 23.10.2022
//  Author      : V-Nezlo (vlladimirka@gmail.com)
//  Description : ООП обертка для gpio

#ifndef GPIOWRAPPER
#define GPIOWRAPPER

#include "driver/gpio.h"

class Gpio {
public:
	Gpio(uint64_t aNum, gpio_mode_t aMode, bool aState = false, gpio_pullup_t aPullUp = GPIO_PULLUP_DISABLE, gpio_pulldown_t aPullDown = GPIO_PULLDOWN_DISABLE, gpio_int_type_t aIntType = GPIO_INTR_DISABLE);
	// Чтобы не облажаться удалю ненужные конструкторы
	Gpio(const Gpio &) = delete;
	Gpio &operator=(const Gpio &) = delete;
	virtual ~Gpio();

	void set();
	void reset();
	void setState(bool aState);

private:
	gpio_config_t config;
	gpio_num_t number;
};

#endif