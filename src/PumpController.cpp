/*!
@file
@brief Обработчик насоса
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#include "MutexLock.hpp"
#include "PumpController.hpp"
#include "freertos/semphr.h"

PumpController::PumpController() :
	mode{PumpModes::EBBNormal},
	pumpState{PumpState::PumpOff},
	damTankState{DamTankState::DamUnlocked},
	currentWaterLevel{0},
	upperState{false},
	swingState{SwingState::SwingOff},
	lastActionTime{0},
	lastSwingTime{0},
	waterFillingTimer{0},
	enabled{false},
	pumpOnTime{0},
	pumpOffTime{0},
	swingTime{0},
	mutex{xSemaphoreCreateMutex()}
{
}

EventResult PumpController::handleEvent(Event *e)
{
	switch(e->type) {
		case EventType::SettingsUpdated: {
			// Чтобы обеспечить нормальный переход между режимами - замутексуем
			MutexLock lock(mutex);
			enabled = e->data.settings.pump.enabled;
			pumpOnTime = std::chrono::seconds(e->data.settings.pump.onTime);
			pumpOffTime = std::chrono::seconds(e->data.settings.pump.offTime);
			swingTime = std::chrono::seconds(e->data.settings.pump.swingTime);
			updateMode(e->data.settings.pump.mode);
			} return EventResult::PASS_ON;
		case EventType::UpdateLowerData:
			currentWaterLevel = e->data.lowerData.waterLevel;
			pumpState = e->data.lowerData.pumpState ? PumpState::PumpOn : PumpState::PumpOff;
			return EventResult::PASS_ON;
		case EventType::UpdateUpperData:
			upperState = e->data.upperData.swingLevelState;
			damTankState = e->data.upperData.damState ? DamTankState::DamLocked : DamTankState::DamUnlocked;
			return EventResult::PASS_ON;
		default:
			return EventResult::IGNORED;
	}

	return EventResult::IGNORED;
}

void PumpController::process(std::chrono::milliseconds aCurrentTime)
{
	// Используем мутекс как гарантию что переходы будут корректными
	MutexLock lock(mutex);

	if (enabled) {
		switch(mode) {
			case PumpModes::EBBNormal:
				processEBBNormalMode(aCurrentTime);
				break;
			case PumpModes::EBBSwing:
				processEBBSwingMode(aCurrentTime);
				break;
			case PumpModes::Dripping:
				processDripMode(aCurrentTime);
				break;
			case PumpModes::Maintance:
				processMaintanceMode();
				break;
			case PumpModes::EBBDam:
				processEBBDumMode(aCurrentTime);
				break;
		}
	} else {
		if (pumpState == PumpState::PumpOn) {
			setPumpState(PumpState::PumpOff);
			setDamState(DamTankState::DamUnlocked);
		}
	}
}

void PumpController::updateMode(PumpModes aNewMode)
{
	auto currentTime = TimeWrapper::milliseconds();
	lastActionTime = currentTime;
	lastSwingTime = currentTime;
	waterFillingTimer = std::chrono::milliseconds{0};

	mode = aNewMode;
	setPumpState(PumpState::PumpOff);
	setDamState(DamTankState::DamUnlocked);
}

void PumpController::setPumpState(PumpState aState)
{
	if (aState == PumpState::PumpOff) {
		sendCommandToPump(false);
	} else {
		sendCommandToPump(true);
	}
}

void PumpController::setDamState(DamTankState aState)
{
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
	EventBus::throwEvent(&ev, this);
}

void PumpController::clearErrorToEventBus(SystemErrors aError)
{
	Event ev;
	ev.type = EventType::ClearError;
	ev.data.error = aError;
	EventBus::throwEvent(&ev, this);
}

void PumpController::sendCommandToPump(bool aNewPumpState)
{
	Event ev;
	ev.type = EventType::ActionRequest;
	ev.data.action = aNewPumpState ? Action::TurnPumpOn : Action::TurnPumpOff;
	EventBus::throwEvent(&ev, this);
}

void PumpController::sendCommandToDam(bool aNewDamState)
{
	Event ev;
	ev.type = EventType::ActionRequest;
	ev.data.action = aNewDamState ? Action::OpenDam : Action::CloseDam;
	EventBus::throwEvent(&ev, this);
}

bool PumpController::permitForAction() const
{
	return currentWaterLevel > Options::kMinWaterLevelForWork;
}

/// @brief EBB режим, вкл выкл насоса по времени и проверки на флудинг
void PumpController::processEBBNormalMode(std::chrono::milliseconds aCurrentTime)
{
	switch (pumpState) {
		case PumpState::PumpOn:
		// Если насос сейчас включен - смотрим, не пора ли выключать
			if (aCurrentTime > lastActionTime + pumpOnTime) {
				lastActionTime = aCurrentTime;
				setPumpState(PumpState::PumpOff);
			}
			break;
		case PumpState::PumpOff:
			if (aCurrentTime > lastActionTime + pumpOffTime) {
				waterFillingTimer = aCurrentTime + Options::kMaxTimeForFullFlooding;
				lastActionTime = aCurrentTime;
				if (permitForAction()) {
					setPumpState(PumpState::PumpOn);
				} else {
					throwErrorToEventBus(SystemErrors::SystemPumpNotOperate);
				}
			}
			break;
	}

	if (pumpState == PumpState::PumpOn && aCurrentTime > waterFillingTimer) {
		setPumpState(PumpState::PumpOff);
		throwErrorToEventBus(SystemErrors::SystemTankNotFloodedInTime);
	} else if (pumpState == PumpState::PumpOn && !permitForAction()) {
		setPumpState(PumpState::PumpOff);
		throwErrorToEventBus(SystemErrors::SystemPumpNotOperate);
	}
}

/// @brief Расширенный EBB режим, вкл выкл насоса по времени и отработка "качелей"
void PumpController::processEBBSwingMode(std::chrono::milliseconds aCurrentTime)
{
	switch (pumpState) {
		case PumpState::PumpOn:
		// Если насос сейчас включен - смотрим, не пора ли выключать
			if (aCurrentTime > lastActionTime + pumpOnTime) {
				lastActionTime = aCurrentTime;
				setPumpState(PumpState::PumpOff);
			}
			break;
		case PumpState::PumpOff:
			if (aCurrentTime > lastActionTime + pumpOffTime) {
				lastActionTime = aCurrentTime;
				if (permitForAction()) {
					setPumpState(PumpState::PumpOn);
				} else {
					throwErrorToEventBus(SystemErrors::SystemPumpNotOperate);
				}
			}
			break;
	}

	if (pumpState == PumpState::PumpOn) {
		if (swingState == SwingState::SwingOff && aCurrentTime > lastSwingTime + swingTime) {
			setPumpState(PumpState::PumpOn);
			swingState = SwingState::SwingOn;
			waterFillingTimer = aCurrentTime + Options::kMaxTimeForFullFlooding;
		} else if (swingState == SwingState::SwingOn && upperState == true) {
			setPumpState(PumpState::PumpOff);
			swingState = SwingState::SwingOff;
			lastSwingTime = aCurrentTime;
		} else if (swingState == SwingState::SwingOn && aCurrentTime > waterFillingTimer) {
			throwErrorToEventBus(SystemErrors::SystemTankNotFloodedInTime);
			setPumpState(PumpState::PumpOff);
			swingState = SwingState::SwingOff;
		} else if (swingState == SwingState::SwingOn && !permitForAction()) {
			setPumpState(PumpState::PumpOff);
			swingState = SwingState::SwingOff;
			throwErrorToEventBus(SystemErrors::SystemPumpNotOperate);
		}
	}
}

/// @brief Самый простой режим, просто вкл-выкл насоса по времени
void PumpController::processDripMode(std::chrono::milliseconds aCurrentTime)
{
	switch (pumpState) {
		case PumpState::PumpOn:
		// Если насос сейчас включен - смотрим, не пора ли выключать
			if (aCurrentTime > lastActionTime + pumpOnTime) {
				lastActionTime = aCurrentTime;
				setPumpState(PumpState::PumpOff);
			}
			break;
		case PumpState::PumpOff:
			if (aCurrentTime > lastActionTime + pumpOffTime) {
				lastActionTime = aCurrentTime;
				if (permitForAction()) {
					setPumpState(PumpState::PumpOn);
				} else {
					throwErrorToEventBus(SystemErrors::SystemPumpNotOperate);
				}
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
	switch (pumpState) {
		case PumpState::PumpOn:
		// Если насос сейчас включен - смотрим, не пора ли выключать
			if (aCurrentTime > lastActionTime + pumpOnTime) {
				lastActionTime = aCurrentTime;
				setDamState(DamTankState::DamUnlocked); // Открываем дамбу при выключении
				setPumpState(PumpState::PumpOff);
			} else {
				if (upperState) { // При срабатывании поплавкового датчика закрываем дамбу и выключаем насос
					setPumpState(PumpState::PumpOff);
					setDamState(DamTankState::DamLocked);
					// Как телеметрировать здесь пока непонятно
				} else if (aCurrentTime > waterFillingTimer) {
					setPumpState(PumpState::PumpOff);
					setDamState(DamTankState::DamUnlocked);
				}
			}
			break;
		case PumpState::PumpOff:
			if (aCurrentTime > lastActionTime + pumpOffTime) {
				lastActionTime = aCurrentTime;
				waterFillingTimer = aCurrentTime + Options::kMaxTimeForFullFlooding;
				if (permitForAction()) {
					setPumpState(PumpState::PumpOn);
				} else {
					throwErrorToEventBus(SystemErrors::SystemPumpNotOperate);
				}
			}
			break;
	}
}

