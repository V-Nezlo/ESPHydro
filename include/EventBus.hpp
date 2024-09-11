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
	ToneBuzzerSignal,
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

enum class ToneBuzzerSignal {
	Disabled,
	Enabling,
	Warning,
	Error
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
		ToneBuzzerSignal buzToneSignal;
		DeviceType device;
		HealthUpdate healthUpdate;
	} data;
};

class AbstractEventObserver {
public:
	virtual EventResult handleEvent(Event *e) = 0;
};

class EventBus {
public:
	static void throwEvent(Event *aEvent, AbstractEventObserver *aOwner)
	{
		for (auto &pos : observers) {
			// Для вызывающего throwEvent событие обработано не будет, в случае nullptr не проверяется
			if (aOwner != nullptr && pos == aOwner) {
				continue;
			} else {
				const auto result = pos->handleEvent(aEvent);
				// If result == PASS_ON or IGNORED - continue
				if (result == EventResult::HANDLED) {
					break;
				}
			}
		}
	}

	static void registerObserver(AbstractEventObserver *aObserver)
	{
		const auto iter = std::find(observers.begin(), observers.end(), aObserver);

		if (iter == observers.end()) {
			observers.push_back(aObserver);
		}
	}

private:
	static std::vector<AbstractEventObserver *> observers;
};

#endif // INCLUDE_EVENTBUS_HPP_
