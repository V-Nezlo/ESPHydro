#include "Options.hpp"
#include "MasterMonitor.hpp"
#include <esp_log.h>

constexpr HealthRule MasterMonitor::rules[];

MasterMonitor &MasterMonitor::instance()
{
	static MasterMonitor inst;
	return inst;
}

MasterMonitor::MasterMonitor():
	BaseMonitor(DeviceType::Master, DeviceHealth::DeviceDisabled)
{}

void MasterMonitor::invoke()
{
	updateHealth();
}

EventResult MasterMonitor::handleEvent(Event *e)
{
	switch (e->type) {
		case EventType::RsDeviceDetached:
			clearFlag(MasterFlags::DeviceMismatch);
			return EventResult::PASS_ON;
		default:
			return EventResult::IGNORED;
	}
}