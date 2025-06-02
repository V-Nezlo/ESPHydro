#include "LowerMonitor.hpp"

constexpr HealthRule LowerMonitor::rules[];

LowerMonitor& LowerMonitor::instance()
{
	static LowerMonitor inst;
	return inst;
}

LowerMonitor::LowerMonitor():
	BaseMonitor(DeviceType::Lower, DeviceHealth::DeviceDisabled)
{}

EventResult LowerMonitor::handleEvent(Event *e)
{
	switch (e->type) {
		case EventType::RsDeviceDetached:
			handleDeviceDetached(e->data.device);
			return EventResult::PASS_ON;
		default:
			return EventResult::IGNORED;
	}
}