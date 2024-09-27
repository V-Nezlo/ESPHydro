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
	updated{true}
{
}

void SystemIntegrator::process(std::chrono::milliseconds aCurrentTime)
{
	if (updated && aCurrentTime > lastCheckTime + std::chrono::milliseconds{1000}) {
		lastCheckTime = aCurrentTime;
		updated = false;

		Event ev;
		ev.type = EventType::UpdateSystemData;
		ev.data.systemData.flags = systemFlagStorage;
		EventBus::throwEvent(&ev, this);
	}
}

EventResult SystemIntegrator::handleEvent(Event *e)
{
	switch(e->type) {
		case EventType::SetError:
			systemFlagStorage |= e->data.error;
			updated = true;
			return EventResult::PASS_ON;

		case EventType::ClearError:
			systemFlagStorage &= ~e->data.error;
			updated = true;
			return EventResult::PASS_ON;

		default:
			return EventResult::IGNORED;
	}
}
