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

	struct {
		DeviceHealth lower;
		DeviceHealth upper;
		DeviceHealth aux;
		DeviceHealth system;

		void update(HealthUpdate update)
		{
			switch(update.type) {
				case DeviceType::Lower:
					lower = update.health;
					break;
				case DeviceType::Upper:
					upper = update.health;
					break;
				case DeviceType::AUX:
					aux = update.health;
					break;
				case DeviceType::Master:
					system = update.health;
					break;
			}
		}
	} healths;
};

#endif