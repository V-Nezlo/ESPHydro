/*!
@file
@brief Обработчик лампы
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#ifndef INCLUDE_LIGHTCONTROLLER_HPP_
#define INCLUDE_LIGHTCONTROLLER_HPP_

#include "ConfigStorage.hpp"
#include "EventBus.hpp"
#include "LinearSched.hpp"
#include "Gpio.hpp"
#include "TimeWrapper.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class LightController : public AbstractEventObserver, public AbstractLinearTask {
public:
	LightController();

	EventResult handleEvent(Event *e) override;
	void process(std::chrono::milliseconds aCurrentInternalTime) override;

private:
	bool enabled;
	PumpModes pumpMode;
	bool lampState;
	Time lampOnTime;
	Time lampOffTime;
	Time currentTime;

	std::chrono::milliseconds lastCheckTime;
	SemaphoreHandle_t mutex;

	void sendCommandToEventBus(bool aNewLampState);

	/// @brief Функция написана ChatGPT
	/// @param currentTime
	/// @param startTime
	/// @param endTime
	/// @return True если время находится в промежутке для включения, false - если для выключения
	static inline bool isTimeForOn(const Time& currentTime, const Time& startTime, const Time& endTime);
};

#endif // INCLUDE_LIGHTCONTROLLER_HPP_