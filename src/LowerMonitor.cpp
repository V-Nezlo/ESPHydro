#include "LowerMonitor.hpp"

constexpr HealthRule LowerMonitor::rules[];

LowerMonitor& LowerMonitor::instance() {
	static LowerMonitor inst;
	return inst;
}

LowerMonitor::LowerMonitor():
	flags(0),
	health(DeviceHealth::DeviceDisabled)
{}

void LowerMonitor::setFlag(LowerFlags flag)
{
	flags |= static_cast<uint8_t>(flag);
	updateHealth();
}

void LowerMonitor::clearFlag(LowerFlags flag)
{
	flags &= ~static_cast<uint8_t>(flag);
	updateHealth();
}

bool LowerMonitor::hasFlag(LowerFlags flag) const
{
	return flags & static_cast<uint8_t>(flag);
}

uint8_t LowerMonitor::getFlags() const
{
	return flags;
}

DeviceHealth LowerMonitor::getHealth() const
{
	return health;
}

bool LowerMonitor::isPresent() const
{
	return health != DeviceHealth::DeviceDisabled;
}

void LowerMonitor::updateFromTelemetry(uint8_t telemetryFlags)
{
	flags = telemetryFlags;
	updateHealth();
}

EventResult LowerMonitor::handleEvent(Event *e)
{
	switch (e->type) {
		case EventType::RsDeviceDetached:
			if (e->data.device == DeviceType::Lower) {
				flags = 0;
				health = DeviceHealth::DeviceDisabled;
				sendNewHealthToEventBus();
			} return EventResult::PASS_ON;
		default:
			return EventResult::IGNORED;
	}
}

void LowerMonitor::updateHealth()
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

void LowerMonitor::sendNewHealthToEventBus()
{
	// Отправим новый health
	Event ev;
	ev.type = EventType::UpdateDeviceHealth;
	ev.data.updateHealth.type = DeviceType::Lower;
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
