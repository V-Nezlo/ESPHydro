/*!
@file
@brief Обработчик насоса
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#include "PumpController.hpp"
#include "freertos/semphr.h"

PumpController::PumpController() :
	mode{PumpModes::EBBNormal},
	pumpState{PumpState::PUMPOFF},
	swingState{SwingState::SwingOff},
	lastActionTime{0},
	lastSwingTime{0},
	waterFillingTimer{0},
	enabled{false},
	pumpOnTime{0},
	pumpOffTime{0},
	swingTime{0},
	currentWaterLevel{0},
	upperState{false},
	mutex{}
{
}

EventResult PumpController::handleEvent(Event *e)
{
	switch(e->type) {
		case EventType::SettingsFirstLoad:
			// Fallthrough
		case EventType::SettingsUpdated:
			// Чтобы обеспечить нормальный переход между режимами - замутексуем
			xSemaphoreTake(mutex, portMAX_DELAY);
			enabled = e->data.settings.pump.enabled;
			pumpOnTime = std::chrono::seconds(e->data.settings.pump.onTime);
			pumpOffTime = std::chrono::seconds(e->data.settings.pump.offTime);
			swingTime = std::chrono::seconds(e->data.settings.pump.swingTime);
			updateMode(e->data.settings.pump.mode);
			xSemaphoreGive(mutex);
			return EventResult::PASS_ON;
		case EventType::UpdateLowerData:
			break;
		case EventType::UpdateUpperData:
			break;
		default:
			return EventResult::IGNORED;
	}

	return EventResult::IGNORED;
}

void PumpController::process(std::chrono::milliseconds aCurrentTime)
{
	// Используем мутекс как гарантию что переходы будут корректными
	xSemaphoreTake(mutex, portMAX_DELAY);
	auto currentTime = TimeWrapper::milliseconds();

	if (enabled) {
		switch(mode) {
			case PumpModes::EBBNormal:
				processEBBNormalMode(currentTime);
				break;
			case PumpModes::EBBSwing:
				processEBBSwingMode(currentTime);
				break;
			case PumpModes::Dripping:
				processDripMode(currentTime);
				break;
			case PumpModes::Maintance:
				processMaintanceMode();
				break;
		}
	} else {
		if (pumpState == PumpState::PUMPON) {
			setPumpState(false);
			pumpState = PumpState::PUMPOFF;
			}
	}

	xSemaphoreGive(mutex);
}

void PumpController::updateMode(PumpModes aNewMode)
{
	auto currentTime = TimeWrapper::milliseconds();

	lastActionTime = currentTime;
	lastSwingTime = currentTime;
	waterFillingTimer = std::chrono::milliseconds{0};
}

void PumpController::setPumpState(bool aState)
{
	Event ev;
	ev.type = EventType::ActionRequest;
	ev.data.action = aState ? Action::TurnPumpOn : Action::TurnPumpOff;
	EventBus::throwEvent(&ev);
}

void PumpController::throwErrorToEventBus(SystemErrors aError)
{
	Event ev;
	ev.type = EventType::SetError;
	ev.data.error = aError;
	EventBus::throwEvent(&ev);
}

void PumpController::clearErrorToEventBus(SystemErrors aError)
{
	Event ev;
	ev.type = EventType::ClearError;
	ev.data.error = aError;
	EventBus::throwEvent(&ev);
}

/// @brief EBB режим, вкл выкл насоса по времени и проверки на флудинг
void PumpController::processEBBNormalMode(std::chrono::milliseconds aCurrentTime)
{
	switch (pumpState) {
		case PumpState::PUMPON:
		// Если насос сейчас включен - смотрим, не пора ли выключать
			if (aCurrentTime > lastActionTime + pumpOnTime) {
				lastActionTime = aCurrentTime;
				setPumpState(false);
			}
			break;
		case PumpState::PUMPOFF:
			if (aCurrentTime > lastActionTime + pumpOffTime) {
				waterFillingTimer = aCurrentTime + Options::kMaxTimeForFullFlooding;
				lastActionTime = aCurrentTime;
				setPumpState(true);
			}
			break;
	}

	if (pumpState == PumpState::PUMPON && aCurrentTime > waterFillingTimer ) {
		setPumpState(false);
		throwErrorToEventBus(SystemErrors::SystemTankNotFloodedInTime);
		pumpState = PumpState::PUMPOFF;
	}
}

/// @brief Расширенный EBB режим, вкл выкл насоса по времени и отработка "качелей"
void PumpController::processEBBSwingMode(std::chrono::milliseconds aCurrentTime)
{
	switch (pumpState) {
		case PumpState::PUMPON:
		// Если насос сейчас включен - смотрим, не пора ли выключать
			if (aCurrentTime > lastActionTime + pumpOnTime) {
				lastActionTime = aCurrentTime;
				setPumpState(false);
			}
			break;
		case PumpState::PUMPOFF:
			if (aCurrentTime > lastActionTime + pumpOffTime) {
				lastActionTime = aCurrentTime;
				setPumpState(true);
			}
			break;
	}

	if (pumpState == PumpState::PUMPON) {
		if (swingState == SwingState::SwingOff && aCurrentTime > lastSwingTime + swingTime) {
			setPumpState(true);
			swingState = SwingState::SwingOn;
			waterFillingTimer = aCurrentTime + Options::kMaxTimeForFullFlooding;
		} else if (swingState == SwingState::SwingOn && upperState == true) {
			setPumpState(false);
			swingState = SwingState::SwingOff;
			lastSwingTime = aCurrentTime;
		} else if (swingState == SwingState::SwingOn && aCurrentTime > waterFillingTimer) {
			throwErrorToEventBus(SystemErrors::SystemTankNotFloodedInTime);
			setPumpState(false);
			pumpState = PumpState::PUMPOFF;
			swingState = SwingState::SwingOff;
		}
	}
}

/// @brief Самый простой режим, просто вкл-выкл насоса по времени
void PumpController::processDripMode(std::chrono::milliseconds aCurrentTime)
{
	switch (pumpState) {
		case PumpState::PUMPON:
		// Если насос сейчас включен - смотрим, не пора ли выключать
			if (aCurrentTime > lastActionTime + pumpOnTime) {
				lastActionTime = aCurrentTime;
				setPumpState(false);
			}
			break;
		case PumpState::PUMPOFF:
			if (aCurrentTime > lastActionTime + pumpOffTime) {
				lastActionTime = aCurrentTime;
				setPumpState(true);
			}
			break;
	}
}

void PumpController::processMaintanceMode()
{
	return;
}

void processEBBDumMode(std::chrono::milliseconds aCurrentTime)
{
    // TODO
}

