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
	DeviceMonitor():
		queue{},
		previousUpperFlags{0xFF},
		previousLowerFlags{0xFF}
	{

	}

	EventResult handleEvent(Event *e) override
	{
		switch (e->type) {
			case EventType::RsDeviceAttached:{
				HealthUpdate update;
				update.health = DeviceHealth::DeviceWorking;
				update.type = e->data.device;
				queue.push(update);

				return EventResult::PASS_ON;}
			case EventType::RsDeviceDetached: {
				HealthUpdate updated;
				updated.health = DeviceHealth::DeviceDisabled;
				updated.type = e->data.device;
				queue.push(updated);

				return EventResult::PASS_ON;
				}
			case EventType::UpdateLowerData: {
				if (e->data.lowerData.flags == previousLowerFlags) {
					return EventResult::IGNORED;
				} else {
					previousLowerFlags = e->data.lowerData.flags;
				}

				HealthUpdate update;
				update.type = DeviceType::Lower;

				if (e->data.lowerData.flags & LowerFlags::LowerNoWaterFlag || e->data.lowerData.flags & LowerFlags::LowerPumpOverCurrentFlag ||
				e->data.lowerData.flags & LowerFlags::LowerPumpLowCurrentFlag) {
					update.health = DeviceHealth::DeviceError;
				} else if (e->data.lowerData.flags & LowerFlags::LowerPHSensorErrorFlag || e->data.lowerData.flags &
					LowerFlags::LowerPPMSensorErrorFlag || e->data.lowerData.flags & LowerFlags::LowerTempSensorErrorFlag) {
					update.health = DeviceHealth::DeviceWarning;
				} else {
					update.health = DeviceHealth::DeviceWorking;
				}
				queue.push(update);

				return EventResult::PASS_ON;}
			case EventType::UpdateUpperData: {
				if (e->data.upperData.flags == previousUpperFlags) {
					return EventResult::IGNORED;
				} else {
					previousUpperFlags = e->data.upperData.flags;
				}

				HealthUpdate update;
				update.type = DeviceType::Upper;

				if (e->data.upperData.flags & UpperFlags::UpperPowerError) {
					update.health = DeviceHealth::DeviceError;
				} else if (e->data.upperData.flags & UpperFlags::UpperTopWaterLevelStuck) {
					update.health = DeviceHealth::DeviceWarning;
				} else {
					update.health = DeviceHealth::DeviceWorking;
				}

				queue.push(update);
				return EventResult::PASS_ON;}
			default:
				return EventResult::IGNORED;
		}
	}

	void process(std::chrono::milliseconds aCurrentTime)
	{
		if (!queue.empty()) {
			auto entry = queue.front();

			Event ev;
			ev.type = EventType::HealthUpdated;
			ev.data.healthUpdate = entry;
			EventBus::throwEvent(&ev);

			queue.pop();
		}
	}

private:
	std::queue<HealthUpdate> queue;
	uint8_t previousUpperFlags;
	uint8_t previousLowerFlags;
};

#endif