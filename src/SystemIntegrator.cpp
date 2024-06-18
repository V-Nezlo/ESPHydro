/*!
@file
@brief Интегратор флагов системы, обьеденяющий их для GUI
@author V-Nezlo (vlladimirka@gmail.com)
@date 16.05.2024
@version 1.0
*/

#include "SystemIntergrator.hpp"

SystemIntegrator::SystemIntegrator() :
	systemFlagStorage{0},
	lastCheckTime{0},
	updated{false},
	signalNeeds{BuzzerSignal::Disable}
{
}

void SystemIntegrator::process(std::chrono::milliseconds aCurrentTime)
{
	if (aCurrentTime > lastCheckTime + std::chrono::milliseconds{1000} && updated) {
		lastCheckTime = aCurrentTime;
		updated = false;

		// Составим health из флагов
		DeviceHealth health = DeviceHealth::DeviceWorking;
		if ((systemFlagStorage & SystemErrors::SystemLeak) || (systemFlagStorage & SystemErrors::SystemRSBusError)) {
			health = DeviceHealth::DeviceCritical;
		} else if ((systemFlagStorage & SystemErrors::SystemRTCError) || (systemFlagStorage & SystemErrors::SystemInternalMemError)) {
			health = DeviceHealth::DeviceError;
		} else if (systemFlagStorage & SystemErrors::SystemTankNotFloodedInTime) {
			health = DeviceHealth::DeviceWarning;
		}

		// Обновляем GUI
		Event ev;
		ev.type = EventType::UpdateSystemData;
		ev.data.systemData.flags = systemFlagStorage;
		EventBus::throwEvent(&ev, this);
		// Обновляем Health устройства System
		Event ev2;
		ev2.type = EventType::HealthUpdated;
		ev2.data.healthUpdate.type = DeviceType::Master;
		ev2.data.healthUpdate.health = health;
		EventBus::throwEvent(&ev2, this);

		// Оповещаем буззером
		if (signalNeeds == BuzzerSignal::Short || signalNeeds == BuzzerSignal::Long) {
			Event ev;
			ev.type = EventType::BuzzerSignal;
			ev.data.buzSignal = signalNeeds;
			EventBus::throwEvent(&ev, this);
		}
	}
}

EventResult SystemIntegrator::handleEvent(Event *e)
{
	switch(e->type) {
		case EventType::SetError:
			systemFlagStorage |= e->data.error;
			updated = true;
			signalNeeds = BuzzerSignal::Long;
			return EventResult::PASS_ON;

		case EventType::ClearError:
			systemFlagStorage &= ~e->data.error;
			updated = true;
			signalNeeds = BuzzerSignal::Short;
			return EventResult::PASS_ON;

		default:
			return EventResult::IGNORED;
	}
}
