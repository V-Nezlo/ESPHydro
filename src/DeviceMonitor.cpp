#include "DeviceMonitor.hpp"

DeviceMonitor::DeviceMonitor():
	queue{},
	previousUpperFlags{0xFF},
	previousLowerFlags{0xFF}
{
}

EventResult DeviceMonitor::handleEvent(Event *e)
{
	switch (e->type) {
		case EventType::RsDeviceAttached:{
			HealthUpdate update;
			update.health = DeviceHealth::DeviceWorking;
			update.type = e->data.device;
			queue.push(update);

			return EventResult::PASS_ON;}
		case EventType::RsDeviceDetached: {
			HealthUpdate updated;
			updated.health = DeviceHealth::DeviceDisabled;
			updated.type = e->data.device;
			queue.push(updated);

			return EventResult::PASS_ON;
			}
		case EventType::UpdateLowerData: {
			if (e->data.lowerData.flags == previousLowerFlags) {
				return EventResult::IGNORED;
			} else {
				previousLowerFlags = e->data.lowerData.flags;
			}

			HealthUpdate update;
			update.type = DeviceType::Lower;

			if (e->data.lowerData.flags & LowerFlags::LowerNoWaterFlag || e->data.lowerData.flags & LowerFlags::LowerPumpOverCurrentFlag ||
			e->data.lowerData.flags & LowerFlags::LowerPumpLowCurrentFlag) {
				update.health = DeviceHealth::DeviceError;
			} else if (e->data.lowerData.flags & LowerFlags::LowerPHSensorErrorFlag || e->data.lowerData.flags &
				LowerFlags::LowerPPMSensorErrorFlag || e->data.lowerData.flags & LowerFlags::LowerTempSensorErrorFlag) {
				update.health = DeviceHealth::DeviceWarning;
			} else {
				update.health = DeviceHealth::DeviceWorking;
			}
			queue.push(update);

			return EventResult::PASS_ON;}
		case EventType::UpdateUpperData: {
			if (e->data.upperData.flags == previousUpperFlags) {
				return EventResult::IGNORED;
			} else {
				previousUpperFlags = e->data.upperData.flags;
			}

			HealthUpdate update;
			update.type = DeviceType::Upper;

			if (e->data.upperData.flags & UpperFlags::UpperPowerError) {
				update.health = DeviceHealth::DeviceError;
			} else if (e->data.upperData.flags & UpperFlags::UpperTopWaterLevelStuck) {
				update.health = DeviceHealth::DeviceWarning;
			} else {
				update.health = DeviceHealth::DeviceWorking;
			}

			queue.push(update);
			return EventResult::PASS_ON;}
		default:
			return EventResult::IGNORED;
		}
	}

void DeviceMonitor::process(std::chrono::milliseconds aCurrentTime)
{
	if (!queue.empty()) {
		auto entry = queue.front();

		Event ev;
		ev.type = EventType::HealthUpdated;
		ev.data.healthUpdate = entry;
		EventBus::throwEvent(&ev, this);

		queue.pop();
	}
}