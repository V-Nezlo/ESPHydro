/*!
@file
@brief ООП-обертка для gpio
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#ifndef INCLUDE_GPIOWRAPPER_HPP_
#define INCLUDE_GPIOWRAPPER_HPP_

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
	bool read();

private:
	gpio_config_t config;
	gpio_num_t number;
};

#endif // INCLUDE_GPIOWRAPPER_HPP_