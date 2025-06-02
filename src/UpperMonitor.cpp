#include "UpperMonitor.hpp"

constexpr HealthRule UpperMonitor::rules[];

UpperMonitor& UpperMonitor::instance() {
	static UpperMonitor inst;
	return inst;
}

UpperMonitor::UpperMonitor():
	BaseMonitor(DeviceType::Upper, DeviceHealth::DeviceDisabled)
{}

EventResult UpperMonitor::handleEvent(Event *e) {
	switch (e->type) {
		case EventType::RsDeviceDetached:
			handleDeviceDetached(e->data.device);
			return EventResult::PASS_ON;
		default:
			return EventResult::IGNORED;
	}
}