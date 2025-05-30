#include "LowerMonitor.hpp"

LowerMonitor& LowerMonitor::instance() {
	static LowerMonitor inst;
	return inst;
}

LowerMonitor::LowerMonitor():
	flags(0),
	health(DeviceHealth::DeviceWorking)
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

				// Отправим новый health
				Event ev;
				ev.type = EventType::UpdateDeviceHealth;
				ev.data.updateHealth.type = DeviceType::Lower;
				ev.data.updateHealth.health = health;
				EventBus::throwEvent(&ev, this);
			}
			return EventResult::PASS_ON;
		default:
			return EventResult::IGNORED;
	}
}

void LowerMonitor::updateHealth()
{
	for (const auto& rule : rules) {
		if (flags & rule.mask) {
			health = rule.health;
		} else {
			health = DeviceHealth::DeviceWorking;
		}
	}

	Event ev;
	ev.type = EventType::UpdateDeviceHealth;
	ev.data.updateHealth.type = DeviceType::Lower;
	ev.data.updateHealth.health = health;
	EventBus::throwEvent(&ev, this);
}