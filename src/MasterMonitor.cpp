#include "MasterMonitor.hpp"
#include <esp_log.h>

MasterMonitor& MasterMonitor::instance()
{
	static MasterMonitor inst;
	return inst;
}

MasterMonitor::MasterMonitor():
	flags(0),
	health(DeviceHealth::DeviceWarning)
{}

void MasterMonitor::setFlag(MasterFlags flag)
{
	flags |= static_cast<uint32_t>(flag);
	updateHealth();
}

void MasterMonitor::clearFlag(MasterFlags flag)
{
	flags &= ~static_cast<uint32_t>(flag);
	updateHealth();
}

bool MasterMonitor::hasFlag(MasterFlags flag) const
{
	return flags & static_cast<uint32_t>(flag);
}

uint32_t MasterMonitor::getFlags() const
{
	return flags;
}

DeviceHealth MasterMonitor::getHealth() const
{
	return health;
}

bool MasterMonitor::isPresent() const
{
	return health != DeviceHealth::DeviceDisabled;
}

void MasterMonitor::invoke()
{
	health = DeviceHealth::DeviceWorking;
	updateHealth();
}

EventResult MasterMonitor::handleEvent(Event *e)
{
	switch(e->type) {
		case EventType::RsDeviceDetached:
			clearFlag(MasterFlags::DeviceMismatch);
			return EventResult::PASS_ON;
		default:
			return EventResult::IGNORED;
	}
}

void MasterMonitor::updateHealth()
{
	DeviceHealth newHealth = DeviceHealth::DeviceWorking;

	for (const auto& rule : rules) {
		if (flags & rule.mask) {
			newHealth = rule.health;
			break;
		} else {
			newHealth = DeviceHealth::DeviceWorking;
		}
	}

	if (newHealth != health) {
		health = newHealth;
		ESP_LOGE("MasterMonitor", "New health: %d", static_cast<int>(newHealth));
		sendNewHealthToEventBus();
	}

	sendDataToEventBus();
}


void MasterMonitor::sendDataToEventBus()
{
	// Отправим новые флаги в обсерверы
	Event ev;
	ev.type = EventType::UpdateSystemData;
	ev.data.systemData.flags = flags;
	EventBus::throwEvent(&ev, nullptr);
}

void MasterMonitor::sendNewHealthToEventBus()
{
	// Отправим новый health
	Event ev;
	ev.type = EventType::UpdateDeviceHealth;
	ev.data.updateHealth.type = DeviceType::Master;
	ev.data.updateHealth.health = health;
	EventBus::throwEvent(&ev, nullptr);

	// Отправим звуковой сигнал исходя из нового health
	Event ev2;
	ev2.type = EventType::ToneBuzzerSignal;
	switch(health) {
		case DeviceHealth::DeviceWorking:
			ev2.data.buzToneSignal = ToneBuzzerSignal::Information;
			break;
		case DeviceHealth::DeviceError:
			ev2.data.buzToneSignal = ToneBuzzerSignal::Error;
			break;
		case DeviceHealth::DeviceCritical:
			ev2.data.buzToneSignal = ToneBuzzerSignal::CriticalError;
			break;
		case DeviceHealth::DeviceWarning:
			ev2.data.buzToneSignal = ToneBuzzerSignal::Warning;
			break;
		case DeviceHealth::DeviceDisabled:
			ev2.data.buzToneSignal = ToneBuzzerSignal::Disabled;
			break;	
	}
	EventBus::throwEvent(&ev2, nullptr);
}