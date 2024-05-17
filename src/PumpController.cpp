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
	controlState{ControlState::PumpOff},
	swingState{SwingState::SwingOff},
	damTankState{DamTankState::DamUnlocked},
	lastActionTime{0},
	lastSwingTime{0},
	waterFillingTimer{0},
	enabled{false},
	pumpOnTime{0},
	pumpOffTime{0},
	swingTime{0},
	currentWaterLevel{0},
	upperState{false},
	damState{false},
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
			currentWaterLevel = e->data.lowerData.waterLevel;
			return EventResult::PASS_ON;
		case EventType::UpdateUpperData:
			upperState = e->data.upperData.swingLevelState;
			damState = e->data.upperData.damState;
			return EventResult::PASS_ON;
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
			case PumpModes::EBBDam:
				processEBBDumMode(currentTime);
				break;
		}
	} else {
		if (controlState == ControlState::PumpOn) {
			setPumpState(ControlState::PumpOff);
			setDamState(DamTankState::DamUnlocked);
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

	mode = aNewMode;
	setPumpState(ControlState::PumpOff);
	setDamState(DamTankState::DamUnlocked);
}

void PumpController::setPumpState(ControlState aState)
{
	controlState = aState;

	if (aState == ControlState::PumpOff) {
		sendCommandToPump(false);
	} else {
		sendCommandToPump(true);
	}
}

void PumpController::setDamState(DamTankState aState)
{
	damTankState = aState;

	if (aState == DamTankState::DamLocked) {
		sendCommandToDam(false);
	} else {
		sendCommandToDam(true);
	}
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

void PumpController::sendCommandToPump(bool aNewPumpState)
{
	Event ev;
	ev.type = EventType::ActionRequest;
	ev.data.action = aNewPumpState ? Action::TurnPumpOn : Action::TurnPumpOff;
	EventBus::throwEvent(&ev);
}

void PumpController::sendCommandToDam(bool aNewDamState)
{
	Event ev;
	ev.type = EventType::ActionRequest;
	ev.data.action = aNewDamState ? Action::OpenDam : Action::CloseDam;
	EventBus::throwEvent(&ev);
}

/// @brief EBB режим, вкл выкл насоса по времени и проверки на флудинг
void PumpController::processEBBNormalMode(std::chrono::milliseconds aCurrentTime)
{
	switch (controlState) {
		case ControlState::PumpOn:
		// Если насос сейчас включен - смотрим, не пора ли выключать
			if (aCurrentTime > lastActionTime + pumpOnTime) {
				lastActionTime = aCurrentTime;
				setPumpState(ControlState::PumpOff);
			}
			break;
		case ControlState::PumpOff:
			if (aCurrentTime > lastActionTime + pumpOffTime) {
				waterFillingTimer = aCurrentTime + Options::kMaxTimeForFullFlooding;
				lastActionTime = aCurrentTime;
				setPumpState(ControlState::PumpOn);
			}
			break;
	}

	if (controlState == ControlState::PumpOn && aCurrentTime > waterFillingTimer ) {
		setPumpState(ControlState::PumpOff);
		throwErrorToEventBus(SystemErrors::SystemTankNotFloodedInTime);
	}
}

/// @brief Расширенный EBB режим, вкл выкл насоса по времени и отработка "качелей"
void PumpController::processEBBSwingMode(std::chrono::milliseconds aCurrentTime)
{
	switch (controlState) {
		case ControlState::PumpOn:
		// Если насос сейчас включен - смотрим, не пора ли выключать
			if (aCurrentTime > lastActionTime + pumpOnTime) {
				lastActionTime = aCurrentTime;
				setPumpState(ControlState::PumpOff);
			}
			break;
		case ControlState::PumpOff:
			if (aCurrentTime > lastActionTime + pumpOffTime) {
				lastActionTime = aCurrentTime;
				setPumpState(ControlState::PumpOn);
			}
			break;
	}

	if (controlState == ControlState::PumpOn) {
		if (swingState == SwingState::SwingOff && aCurrentTime > lastSwingTime + swingTime) {
			setPumpState(ControlState::PumpOn);
			swingState = SwingState::SwingOn;
			waterFillingTimer = aCurrentTime + Options::kMaxTimeForFullFlooding;
		} else if (swingState == SwingState::SwingOn && upperState == true) {
			setPumpState(ControlState::PumpOff);
			swingState = SwingState::SwingOff;
			lastSwingTime = aCurrentTime;
		} else if (swingState == SwingState::SwingOn && aCurrentTime > waterFillingTimer) {
			throwErrorToEventBus(SystemErrors::SystemTankNotFloodedInTime);
			setPumpState(ControlState::PumpOff);
			swingState = SwingState::SwingOff;
		}
	}
}

/// @brief Самый простой режим, просто вкл-выкл насоса по времени
void PumpController::processDripMode(std::chrono::milliseconds aCurrentTime)
{
	switch (controlState) {
		case ControlState::PumpOn:
		// Если насос сейчас включен - смотрим, не пора ли выключать
			if (aCurrentTime > lastActionTime + pumpOnTime) {
				lastActionTime = aCurrentTime;
				setPumpState(ControlState::PumpOff);
			}
			break;
		case ControlState::PumpOff:
			if (aCurrentTime > lastActionTime + pumpOffTime) {
				lastActionTime = aCurrentTime;
				setPumpState(ControlState::PumpOn);
			}
			break;
	}
}

void PumpController::processMaintanceMode()
{
	return;
}

void PumpController::processEBBDumMode(std::chrono::milliseconds aCurrentTime)
{
	switch (controlState) {
		case ControlState::PumpOn:
		// Если насос сейчас включен - смотрим, не пора ли выключать
			if (aCurrentTime > lastActionTime + pumpOnTime) {
				lastActionTime = aCurrentTime;
				setDamState(DamTankState::DamUnlocked); // Открываем дамбу при выключении
			} else {
				if (upperState) { // При срабатывании поплавкового датчика закрываем дамбу и выключаем насос
					setPumpState(ControlState::PumpOff);
					setDamState(DamTankState::DamLocked);
				} else if (aCurrentTime > waterFillingTimer) {
					setPumpState(ControlState::PumpOff);
					setDamState(DamTankState::DamUnlocked);
				}
			}
			break;
		case ControlState::PumpOff:
			if (aCurrentTime > lastActionTime + pumpOffTime) {
				lastActionTime = aCurrentTime;
				setPumpState(ControlState::PumpOn);
				waterFillingTimer = aCurrentTime + Options::kMaxTimeForFullFlooding;
			}
			break;
	}
}

