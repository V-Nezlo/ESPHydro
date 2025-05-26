/*!
@file
@brief Обработчик лампы
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#include "MutexLock.hpp"
#include "LightController.hpp"

LightController::LightController() :
	enabled{false},
	pumpMode{PumpModes::Maintance},
	lampState{false},
	lampOnTime{0,0,0},
	lampOffTime{0,0,0},
	currentTime{0,0,0},
	lastCheckTime{0},
	mutex{xSemaphoreCreateMutex()}
{
}

EventResult LightController::handleEvent(Event *e)
{
	switch (e->type) {
		case EventType::GetCurrentTime: {
			MutexLock lock(mutex);
			currentTime = e->data.time;
			} return EventResult::PASS_ON;
		case EventType::SettingsUpdated: {
			MutexLock lock(mutex);
			enabled = e->data.settings.lamp.enabled;
			pumpMode = e->data.settings.pump.mode;
			lampOnTime.hour = e->data.settings.lamp.lampOnHour;
			lampOnTime.minutes = e->data.settings.lamp.lampOnMin;
			lampOffTime.hour = e->data.settings.lamp.lampOffHour;
			lampOffTime.minutes = e->data.settings.lamp.lampOffMin;
		} return EventResult::PASS_ON;
		case EventType::UpdateUpperData:
			lampState = e->data.upperData.lampState;
			return EventResult::PASS_ON;
		default:
			return EventResult::IGNORED;
	}
}

void LightController::process(std::chrono::milliseconds aCurrentTime)
{
	static constexpr std::chrono::milliseconds kCheckInterval{5000};

	if (aCurrentTime > lastCheckTime + kCheckInterval) {
		lastCheckTime = aCurrentTime;

		if (pumpMode != PumpModes::Maintance) {
			if (enabled) {
				MutexLock lock(mutex);
				const bool isNowIsActiveTime = isTimeForOn(currentTime, lampOnTime, lampOffTime);

				if (!lampState && isNowIsActiveTime) {
					sendCommandToEventBus(true);
				} else if (lampState && !isNowIsActiveTime) {
					sendCommandToEventBus(false);
				}
			} else if (lampState) {
				sendCommandToEventBus(false);
			}
		}
	}
}

void LightController::sendCommandToEventBus(bool aNewLampState)
{
	Event ev;
	ev.type = EventType::ActionRequest;
	ev.data.action = aNewLampState ? Action::TurnLampOn : Action::TurnLampOff;
	EventBus::throwEvent(&ev, this);
}

bool LightController::isTimeForOn(const Time& currentTime, const Time& startTime, const Time& endTime)
{
	// Helper function to convert time to total seconds for easier comparison
	auto timeToSeconds = [](const Time& t) -> int {
		return t.hour * 3600 + t.minutes * 60 + t.seconds;
	};

	int current = timeToSeconds(currentTime);
	int start = timeToSeconds(startTime);
	int end = timeToSeconds(endTime);

	// Case 1: Same day operation (e.g., 08:00 to 18:00)
	if (start < end) {
		return current >= start && current < end;
	}
	// Case 2: Overnight operation (e.g., 18:00 to 08:00 next day)
	else if (start > end) {
		return current >= start || current < end;
	}
	// Case 3: Same time for start and end (edge case - should probably be disabled)
	else {
		return false;
	}
}

