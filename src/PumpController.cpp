/*!
@file
@brief Обработчик насоса
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#include "MutexLock.hpp"
#include "PumpController.hpp"
#include "Options.hpp"
#include "UpperMonitor.hpp"
#include "LowerMonitor.hpp"
#include "MasterMonitor.hpp"
#include "freertos/semphr.h"

PumpController::PumpController() :
	mode{PumpModes::EBBNormal},
	workingState{PlainType::Drainage},
	actualPumpState{PumpState::PumpOff},
	damTankState{DamTankState::DamUnlocked},
	currentWaterLevel{0},
	upperState{false},
	swingState{SwingState::SwingOff},
	desiredPumpState{PumpState::PumpOff},
	lastActionTime{0},
	lastSwingTime{0},
	waterFillingTimer{0},
	lastChecksTime{0},
	lastValidatorTime{0},
	fillingCheckEn{false},
	enabled{false},
	pumpOnTime{0},
	pumpOffTime{0},
	swingTime{0},
	maxFloodingTime{0},
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
			maxFloodingTime = std::chrono::seconds{e->data.settings.pump.maxFloodingTime};
			updateMode(e->data.settings.pump.mode);
			} return EventResult::PASS_ON;
		case EventType::UpdateLowerData:
			currentWaterLevel = e->data.lowerData.waterLevel;
			actualPumpState = e->data.lowerData.pumpState ? PumpState::PumpOn : PumpState::PumpOff;
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

	// Если ловера нет - то и управлять нечем
	if (enabled && LowerMonitor::instance().isPresent()) {
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
			default:
				break;
		}
		// Валидация состояния насоса чтобы точно быть защищенным от потери пакета управления
		if (mode != PumpModes::Maintance) {
			if (aCurrentTime > lastValidatorTime + Options::kPumpValidationTime) {
				lastValidatorTime = aCurrentTime;

				if (actualPumpState != desiredPumpState) {
					setPumpState(desiredPumpState);
				}
			}
		}
	} else {
		if (desiredPumpState == PumpState::PumpOn) {
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
	workingState = PlainType::Drainage;

	mode = aNewMode;
	setPumpState(PumpState::PumpOff);
	setDamState(DamTankState::DamUnlocked);
}

void PumpController::setPumpState(PumpState aState)
{
	desiredPumpState = aState;

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

/// @brief EBB режим, вкл выкл насоса по времени
void PumpController::processEBBNormalMode(std::chrono::milliseconds aCurrentTime)
{
	switch (workingState) {
		case PlainType::Irrigation:
		// Если насос сейчас включен - смотрим, не пора ли выключать
			if (aCurrentTime > lastActionTime + pumpOnTime) {
				lastActionTime = aCurrentTime;
				setPumpState(PumpState::PumpOff);
				workingState = PlainType::Drainage;
			}
			break;
		case PlainType::Drainage:
			if (aCurrentTime > lastActionTime + pumpOffTime) {
				lastActionTime = aCurrentTime;
				if (permitForAction()) {
					setPumpState(PumpState::PumpOn);
					workingState = PlainType::Irrigation;
					MasterMonitor::instance().clearFlag(MasterFlags::PumpNotOperate);
				} else {
					MasterMonitor::instance().setFlag(MasterFlags::PumpNotOperate);
				}
			}
			break;
	}
}

/// @brief Расширенный EBB режим, вкл выкл насоса по времени и отработка "качелей"
void PumpController::processEBBSwingMode(std::chrono::milliseconds aCurrentTime)
{
	switch (workingState) {
		case PlainType::Irrigation:
		// Если насос сейчас включен - смотрим, не пора ли выключать
			if (aCurrentTime > lastActionTime + pumpOnTime) {
				lastActionTime = aCurrentTime;
				setPumpState(PumpState::PumpOff);
				workingState = PlainType::Drainage;
			}
			break;
		case PlainType::Drainage:
			if (aCurrentTime > lastActionTime + pumpOffTime) {
				lastActionTime = aCurrentTime;
				if (permitForAction()) {
					setPumpState(PumpState::PumpOn);
					workingState = PlainType::Irrigation;
					MasterMonitor::instance().clearFlag(MasterFlags::PumpNotOperate);

					// Проверим время заполнения бака один раз после осушения
					waterFillingTimer = aCurrentTime + maxFloodingTime;
					fillingCheckEn = true;
				} else {
					MasterMonitor::instance().setFlag(MasterFlags::PumpNotOperate);
				}
			}
			break;
		}

	if (aCurrentTime > lastChecksTime + std::chrono::milliseconds{200}) {
		lastChecksTime = aCurrentTime;

		// Проверка на наличие датчика поплавого уровня, если нет - будет работать как обычный режим
		// Но проверка должна быть отложенной поскольку аппер не сразу появляется в системе
		if (MasterMonitor::instance().hasFlag(MasterFlags::SystemInitialized) && !UpperMonitor::instance().isPresent()) {
			MasterMonitor::instance().setFlag(MasterFlags::DeviceMismatch);

			if (desiredPumpState == PumpState::PumpOn) {
				setPumpState(PumpState::PumpOff);
			}

			return;
		}

		// Алгоритм свинга
		if (workingState == PlainType::Irrigation) {
			if (swingState == SwingState::SwingOff && aCurrentTime > lastSwingTime + swingTime) {
				setPumpState(PumpState::PumpOn);
				swingState = SwingState::SwingOn;
			} else if (swingState == SwingState::SwingOn && upperState == true) {
				setPumpState(PumpState::PumpOff);
				swingState = SwingState::SwingOff;
				lastSwingTime = aCurrentTime;

				// Вот тут считаем что бак заполнен, проверим за сколько он заполнился
				if (fillingCheckEn && aCurrentTime > waterFillingTimer) {
					MasterMonitor::instance().setFlag(MasterFlags::TankNotFloodedInTime);
				} else {
					MasterMonitor::instance().clearFlag(MasterFlags::TankNotFloodedInTime);
					fillingCheckEn = false;
				}
			} else if (swingState == SwingState::SwingOn && !permitForAction()) {
				setPumpState(PumpState::PumpOff);
				swingState = SwingState::SwingOff;
				MasterMonitor::instance().setFlag(MasterFlags::PumpNotOperate);
			}
		}
	}
}

/// @brief Самый простой режим, просто вкл-выкл насоса по времени
void PumpController::processDripMode(std::chrono::milliseconds aCurrentTime)
{
	switch (workingState) {
		case PlainType::Irrigation:
		// Если насос сейчас включен - смотрим, не пора ли выключать
			if (aCurrentTime > lastActionTime + pumpOnTime) {
				lastActionTime = aCurrentTime;
				setPumpState(PumpState::PumpOff);
				workingState = PlainType::Drainage;
			}
			break;
		case PlainType::Drainage:
			if (aCurrentTime > lastActionTime + pumpOffTime) {
				lastActionTime = aCurrentTime;
				if (permitForAction()) {
					setPumpState(PumpState::PumpOn);
					workingState = PlainType::Irrigation;
					MasterMonitor::instance().clearFlag(MasterFlags::PumpNotOperate);
				} else {
					MasterMonitor::instance().setFlag(MasterFlags::PumpNotOperate);
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
	switch (workingState) {
		case PlainType::Irrigation:
		// Если насос сейчас включен - смотрим, не пора ли выключать
			if (aCurrentTime > lastActionTime + pumpOnTime) {
				lastActionTime = aCurrentTime;
				setDamState(DamTankState::DamUnlocked); // Открываем дамбу при выключении
				setPumpState(PumpState::PumpOff);
				workingState = PlainType::Drainage;
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
		case PlainType::Drainage:
			if (aCurrentTime > lastActionTime + pumpOffTime) {
				lastActionTime = aCurrentTime;
				waterFillingTimer = aCurrentTime + maxFloodingTime;
				if (permitForAction()) {
					setPumpState(PumpState::PumpOn);
					workingState = PlainType::Irrigation;
					MasterMonitor::instance().clearFlag(MasterFlags::PumpNotOperate);
				} else {
					MasterMonitor::instance().setFlag(MasterFlags::PumpNotOperate);
				}
			}
			break;
	}
}

