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
	BaseMonitor(DeviceType::Master, DeviceHealth::DeviceWarning),
	nextSignalTime{0}
{}

void MasterMonitor::invoke()
{
	health = DeviceHealth::DeviceWorking;
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

void MasterMonitor::process(std::chrono::milliseconds aCurrentTime)
{
	if (health == DeviceHealth::DeviceDisabled || health == DeviceHealth::DeviceWorking) {
		return;
	}
	if (aCurrentTime < nextSignalTime) {
		return;
	}

	Event ev;
	ev.type = EventType::ToneBuzzerSignal;
	ev.data.buzToneSignal = getSignalForHealth(health);
	EventBus::throwEvent(&ev, this);

	switch (health) {
		case DeviceHealth::DeviceWarning:
			nextSignalTime = std::chrono::duration_cast<std::chrono::seconds>(aCurrentTime) + Options::kMasterWarningSignalTimeout;
			break;
		case DeviceHealth::DeviceCritical:
			nextSignalTime = std::chrono::duration_cast<std::chrono::seconds>(aCurrentTime) + Options::kMasterCriticalSignalTimeout;
			break;
		case DeviceHealth::DeviceError:
			nextSignalTime = std::chrono::duration_cast<std::chrono::seconds>(aCurrentTime) + Options::kMasterErrorSignalTimeout;
			break;
		default:
			break;
	}
}