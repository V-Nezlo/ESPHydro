/*!
@file
@brief ООП-обертка для I2C gpio
@author V-Nezlo (vlladimirka@gmail.com)
@date 26.05.2025
@version 1.0
*/

#ifndef INCLUDE_I2CGPIO_HPP_
#define INCLUDE_I2CGPIO_HPP_

#include "PCF8574.hpp"
#include "AbstractClasses.hpp"

class I2CGpio : public AbstractGpio {
public:
	I2CGpio(PCF8574 &aPcf, uint8_t aPin):
		pcf(aPcf), pin(aPin)
	{
		assert(aPin < 8);
	}

	void set() override
	{
		pcf.write(pin, true);
	}

	void reset() override
	{
		pcf.write(pin, false);
	}

	void setState(bool aState)
	{
		pcf.write(pin, aState);
	}

	bool read() override
	{
		return false;
	}
private:
	PCF8574 &pcf;
	uint8_t pin;
};

#endif