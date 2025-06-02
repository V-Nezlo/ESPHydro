#include "UpperMonitor.hpp"

constexpr HealthRule UpperMonitor::rules[];

UpperMonitor& UpperMonitor::instance()
{
	static UpperMonitor inst;
	return inst;
}

UpperMonitor::UpperMonitor():
	flags(0),
	health(DeviceHealth::DeviceDisabled)
{}

void UpperMonitor::setFlag(UpperFlags flag)
{
	flags |= static_cast<uint8_t>(flag);
	updateHealth();
}

void UpperMonitor::clearFlag(UpperFlags flag)
{
	flags &= ~static_cast<uint8_t>(flag);
	updateHealth();
}

bool UpperMonitor::hasFlag(UpperFlags flag) const
{
	return flags & static_cast<uint8_t>(flag);
}

uint8_t UpperMonitor::getFlags() const
{
	return flags;
}

DeviceHealth UpperMonitor::getHealth() const
{
	return health;
}

bool UpperMonitor::isPresent() const
{
	return health != DeviceHealth::DeviceDisabled;
}

void UpperMonitor::updateFromTelemetry(uint8_t telemetryFlags)
{
	flags = telemetryFlags;
	updateHealth();
}

EventResult UpperMonitor::handleEvent(Event *e)
{
	switch (e->type) {
		case EventType::RsDeviceDetached:
			if (e->data.device == DeviceType::Upper) {
				flags = 0;
				health = DeviceHealth::DeviceDisabled;
				sendNewHealthToEventBus();
			} return EventResult::PASS_ON;
		default:
			return EventResult::IGNORED;
	}
}

void UpperMonitor::updateHealth()
{
	DeviceHealth newHealth = DeviceHealth::DeviceDisabled;
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
		sendNewHealthToEventBus();
	};
}

void UpperMonitor::sendNewHealthToEventBus()
{
	// Отправим новый health
	Event ev;
	ev.type = EventType::UpdateDeviceHealth;
	ev.data.updateHealth.type = DeviceType::Upper;
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
			ev2.data.buzToneSignal = ToneBuzzerSignal::Disconnected;
			break;	
	}
	EventBus::throwEvent(&ev2, nullptr);
}
