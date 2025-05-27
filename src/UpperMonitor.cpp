#include "UpperMonitor.hpp"

UpperMonitor& UpperMonitor::instance()
{
	static UpperMonitor inst;
	return inst;
}

UpperMonitor::UpperMonitor():
	flags(0),
	health(DeviceHealth::DeviceWorking)
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

				// Отправим новый health
				Event ev;
				ev.type = EventType::UpdateDeviceHealth;
				ev.data.updateHealth.type = DeviceType::Upper;
				ev.data.updateHealth.health = health;
				EventBus::throwEvent(&ev, this);
			}
			return EventResult::PASS_ON;
		default:
			return EventResult::IGNORED;
	}
}

void UpperMonitor::updateHealth()
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
	ev.data.updateHealth.type = DeviceType::Upper;
	ev.data.updateHealth.health = health;
	EventBus::throwEvent(&ev, this);
}