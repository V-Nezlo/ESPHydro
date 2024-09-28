#include "DeviceMonitor.hpp"

DeviceMonitor::DeviceMonitor():
	queue{},
	healths{ DeviceHealth::DeviceDisabled,
			 DeviceHealth::DeviceDisabled,
			 DeviceHealth::DeviceDisabled,
			 DeviceHealth::DeviceDisabled}
{
}

EventResult DeviceMonitor::handleEvent(Event *e)
{
	switch (e->type) {
		case EventType::RsDeviceAttached:{
			HealthUpdate update;
			update.health = DeviceHealth::DeviceWorking;
			update.type = e->data.device;
			healths.update(update);
			queue.push(update);
			} return EventResult::PASS_ON;

		case EventType::RsDeviceDetached: {
			HealthUpdate update;
			update.health = DeviceHealth::DeviceDisabled;
			update.type = e->data.device;
			healths.update(update);
			queue.push(update);

			} return EventResult::PASS_ON;

		case EventType::UpdateLowerData: {
			HealthUpdate update;
			update.type = DeviceType::Lower;

			if (e->data.lowerData.flags & LowerFlags::LowerNoWaterFlag || e->data.lowerData.flags & LowerFlags::LowerPumpOverCurrentFlag ||
			e->data.lowerData.flags & LowerFlags::LowerPumpLowCurrentFlag) {
				update.health = DeviceHealth::DeviceError;
			} else if (e->data.lowerData.flags & LowerFlags::LowerPHSensorErrorFlag || e->data.lowerData.flags &
				LowerFlags::LowerPPMSensorErrorFlag || e->data.lowerData.flags & LowerFlags::LowerTempSensorErrorFlag || e->data.lowerData.flags & LowerFlags::Calibration) {
				update.health = DeviceHealth::DeviceWarning;
			} else {
				update.health = DeviceHealth::DeviceWorking;
			}

			if (healths.lower == update.health) {
				return EventResult::IGNORED;
			} else {
				healths.update(update);
				queue.push(update);
				return EventResult::PASS_ON;
			}
		}

		case EventType::UpdateUpperData: {
			HealthUpdate update;
			update.type = DeviceType::Upper;

			if (e->data.upperData.flags & UpperFlags::UpperPowerError) {
				update.health = DeviceHealth::DeviceError;
			} else if (e->data.upperData.flags & UpperFlags::UpperTopWaterLevelStuck) {
				update.health = DeviceHealth::DeviceWarning;
			} else {
				update.health = DeviceHealth::DeviceWorking;
			}

			if (healths.upper == update.health) {
				return EventResult::IGNORED;
			} else {
				healths.update(update);
				queue.push(update);
				return EventResult::PASS_ON;
			}
		}

		case EventType::UpdateSystemData: {
			HealthUpdate update;
			update.type = DeviceType::Master;

			if ((e->data.systemData.flags & SystemErrors::SystemLeak) || (e->data.systemData.flags & SystemErrors::SystemRSBusError)) {
				update.health = DeviceHealth::DeviceCritical;
			} else if ((e->data.systemData.flags & SystemErrors::SystemRTCError) 
				|| (e->data.systemData.flags & SystemErrors::SystemInternalMemError) 
				|| (e->data.systemData.flags & SystemErrors::SystemPumpNotOperate)) {
				update.health = DeviceHealth::DeviceError;
			} else if (e->data.systemData.flags & SystemErrors::SystemTankNotFloodedInTime) {
				update.health = DeviceHealth::DeviceWarning;
			} else {
				update.health = DeviceHealth::DeviceWorking;
			}

			if (healths.system == update.health) {
				return EventResult::IGNORED;
			} else {
				healths.update(update);
				queue.push(update);
				return EventResult::PASS_ON;
			}
		}

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
