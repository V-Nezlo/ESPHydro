/*!
@file
@brief Обработчик лампы
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

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
		case EventType::GetCurrentTime:
			currentTime = e->data.time;
			return EventResult::PASS_ON;
		case EventType::SettingsUpdated:
			xSemaphoreTake(mutex, portMAX_DELAY);
			enabled = e->data.settings.lamp.enabled;
			pumpMode = e->data.settings.pump.mode;
			lampOnTime.hour = e->data.settings.lamp.lampOnHour;
			lampOnTime.minutes = e->data.settings.lamp.lampOnMin;
			lampOffTime.hour = e->data.settings.lamp.lampOffHour;
			lampOffTime.minutes = e->data.settings.lamp.lampOffMin;
			xSemaphoreGive(mutex);
			return EventResult::PASS_ON;
		case EventType::UpdateUpperData:
			lampState = e->data.upperData.lampState;
			return EventResult::PASS_ON;
		default:
			return EventResult::IGNORED;
	}
}

void LightController::process(std::chrono::milliseconds aCurrentInternalTime)
{
	if (enabled) {
		if (pumpMode != PumpModes::Maintance && aCurrentInternalTime > lastCheckTime + std::chrono::milliseconds{5000}) {
			lastCheckTime = aCurrentInternalTime;

			xSemaphoreTake(mutex, portMAX_DELAY);
			const bool isNowIsActiveTime = isTimeForOn(currentTime, lampOnTime, lampOffTime);

			if (!lampState && isNowIsActiveTime) {
				sendCommandToEventBus(true);
			} else if (lampState && !isNowIsActiveTime) {
				sendCommandToEventBus(false);
			}
			xSemaphoreGive(mutex);
		}
	} else if (lampState) {
		sendCommandToEventBus(false);
	}
}

void LightController::sendCommandToEventBus(bool aNewLampState)
{
	Event ev;
	ev.type = EventType::ActionRequest;
	ev.data.action = aNewLampState ? Action::TurnLampOn : Action::TurnLampOff;
	EventBus::throwEvent(&ev, this);
}

bool LightController::isTimeForOn(const Time& currentTime, const Time& startTime, const Time& endTime) {
	if (startTime.hour < endTime.hour) {
		return (currentTime.hour > startTime.hour ||
			   (currentTime.hour == startTime.hour &&
				(currentTime.minutes > startTime.minutes ||
				(currentTime.minutes == startTime.minutes && currentTime.seconds >= startTime.seconds))))
			   &&
			   (currentTime.hour < endTime.hour ||
			   (currentTime.hour == endTime.hour &&
				(currentTime.minutes < endTime.minutes ||
				(currentTime.minutes == endTime.minutes && currentTime.seconds < endTime.seconds))));
	} else if (startTime.hour > endTime.hour) {
		return !(currentTime.hour > endTime.hour ||
				(currentTime.hour == endTime.hour &&
				(currentTime.minutes > endTime.minutes ||
				(currentTime.minutes == endTime.minutes && currentTime.seconds >= endTime.seconds))) ||
			   !(currentTime.hour < startTime.hour ||
				(currentTime.hour == startTime.hour &&
				(currentTime.minutes < startTime.minutes ||
				(currentTime.minutes == startTime.minutes && currentTime.seconds < startTime.seconds)))));
	} else {
		return (currentTime.minutes > startTime.minutes ||
				(currentTime.minutes == startTime.minutes &&
				currentTime.seconds >= startTime.seconds)) &&
			   (currentTime.minutes < endTime.minutes ||
				(currentTime.minutes == endTime.minutes &&
				currentTime.seconds < endTime.seconds));
	}
}

