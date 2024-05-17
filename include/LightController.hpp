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
#include "GpioWrapper.hpp"
#include "TimeWrapper.hpp"

class LightController : public AbstractEventObserver, public AbstractLinearTask {
public:
	LightController();

	EventResult handleEvent(Event *e) override;
	void process(std::chrono::milliseconds aCurrentInternalTime) override;

private:
	bool enabled;
	Time lampOnTime;
	Time lampOffTime;
	Time currentTime;

	std::chrono::milliseconds lastCheckTime;

	void sendCommandToEventBus(bool aNewLampState);

	/// @brief Функция написана ChatGPT
	/// @param currentTime
	/// @param startTime
	/// @param endTime
	/// @return True если время находится в промежутке для включения, false - если для выключения
	bool isTimeForOn(const Time& currentTime, const Time& startTime, const Time& endTime);
};

#endif // INCLUDE_LIGHTCONTROLLER_HPP_