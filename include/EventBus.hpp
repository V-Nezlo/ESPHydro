/*!
@file
@brief Шина событий-данных
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#ifndef INCLUDE_EVENTBUS_HPP_
#define INCLUDE_EVENTBUS_HPP_

#include "HydroRSTypes.hpp"
#include "Types.hpp"

#include <cctype>
#include <cstddef>
#include <functional>
#include <iostream>
#include <bitset>
#include <math.h>
#include <limits>
#include <string>
#include <variant>
#include <chrono>
#include <vector>
#include <queue>

using namespace std;

enum class EventType : uint8_t {
	SetCurrentTime,
	GetCurrentTime,
	UpdateUpperData,
	UpdateLowerData,
	UpdateSystemData,
	SetError,
	ClearError,
	ActionRequest,
	SettingsUpdated,
	NewBrightness,
	BuzzerSignal,
	RsDeviceDetached,
	RsDeviceAttached,
	HealthUpdated
};

enum class EventResult : uint8_t {
	HANDLED,
	IGNORED,
	PASS_ON
};

enum class Action : uint8_t {
	TurnPumpOn,
	TurnPumpOff,
	TurnLampOn,
	TurnLampOff,
	CloseDam,
	OpenDam
};

enum class BuzzerSignal {
	Disable,
	Short,
	Long
};

struct HealthUpdate {
	DeviceType type;
	DeviceHealth health;
};

struct Event{
	EventType type;
	union {
		Time time;
		UpperInternalData upperData;
		LowerInternalData lowerData;
		SystemData systemData;
		SystemErrors error;
		Action action;
		Settings settings;
		uint8_t brightness;
		BuzzerSignal buzSignal;
		DeviceType device;
		HealthUpdate healthUpdate;
	} data;
};

class AbstractEventObserver {
public:
	virtual EventResult handleEvent(const Event const e) = 0;
};

class EventBus {
public:
	static void throwEvent(Event aEvent, AbstractEventObserver *aOwner)
	{
		queue.push(std::make_pair(aEvent, aOwner));
	}

	static void registerObserver(AbstractEventObserver *aObserver)
	{
		const auto iter = std::find(observers.begin(), observers.end(), aObserver);

		if (iter == observers.end()) {
			observers.push_back(aObserver);
		}
	}

	static void handler()
	{
		if (!queue.empty()) {
			auto pair = queue.front();
			queue.pop();

			const auto event = pair.first;
			const auto owner = pair.second;

			for (auto &pos : observers){
				// Для вызывающего throwEvent событие обработано не будет, в случае nullptr не проверяется
				if (owner != nullptr && pos == owner) {
					continue;
				} else {
					const auto result = pos->handleEvent(event);
					// If result == PASS_ON or IGNORED - continue
					if (result == EventResult::HANDLED) {
						break;
					}
				}
			}
		}
	}

private:
	static std::vector<AbstractEventObserver *> observers;
	static std::queue<std::pair<Event, AbstractEventObserver *>> queue;
};

#endif // INCLUDE_EVENTBUS_HPP_
