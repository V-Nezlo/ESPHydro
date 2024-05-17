/*!
@file
@brief ООП обертка для драйвера RTC DS3231 
@author V-Nezlo (vlladimirka@gmail.com)
@date 18.04.2024
@version 1.0
*/

#include "LinearSched.hpp"
#include "EventBus.hpp"
#include "Types.hpp"

#include <ds3231/ds3231.h>
#include <esp_log.h>

#include <cstring>
#include <cstdint>
#include <utility>

#ifndef INCLUDE_DS3231_HPP_
#define INCLUDE_DS3231_HPP_

class DS3231 : public AbstractEventObserver, public AbstractLinearTask {
public:
	DS3231(uint8_t aPort, uint8_t aSdaPin, uint8_t aSclPin);

	void process(std::chrono::milliseconds aCurrentTime) override;
	bool setCurrentTime(Time aTime);
	std::pair<Time, bool> getCurrentTime();
	EventResult handleEvent(Event *e) override;

private:
	i2c_dev_t dev;
	std::chrono::milliseconds lastReadTime;

	/// @brief Проверим настройку времени по установленному году
	bool isConfigured();
	bool initialConfigure();
};

#endif // INCLUDE_DS3231_HPP_