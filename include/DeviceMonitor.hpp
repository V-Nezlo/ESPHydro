/*!
@file
@brief Класс, генерирующий и раздающий Healths для внешних устройств
@author V-Nezlo (vlladimirka@gmail.com)
@date 13.06.2024
@version 1.0
*/

#ifndef INCLUDE_DEVICEMONITOR_HPP_
#define INCLUDE_DEVICEMONITOR_HPP_

#include "LinearSched.hpp"
#include "EventBus.hpp"
#include "Types.hpp"
#include <chrono>
#include <queue>

class DeviceMonitor : public AbstractEventObserver, public AbstractLinearTask {
public:
	DeviceMonitor();
	EventResult handleEvent(Event *e) override;
	void process(std::chrono::milliseconds aCurrentTime);

private:
	std::queue<HealthUpdate> queue;
	uint8_t previousUpperFlags;
	uint8_t previousLowerFlags;
	bool isAlarmSoundsEnabled;

	void sendSoundSignalIfAllowed();
};

#endif