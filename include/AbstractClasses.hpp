/*!
@file
@brief Абстрактные классы
@author V-Nezlo (vlladimirka@gmail.com)
@date 26.05.2025
@version 1.0
*/

#ifndef INCLUDE_ABSTRACTCLASSES_HPP_
#define INCLUDE_ABSTRACTCLASSES_HPP_

#include "EventBus.hpp"
#include <cstdint>

class AbstractGpio {
public:
	virtual void set() = 0;
	virtual void reset() = 0;
	virtual void setState(bool aState) = 0;
	virtual bool read() = 0;
};

#endif // INCLUDE_ABSTRACTCLASSES_HPP_