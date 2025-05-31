#include "MasterMonitor.hpp"

MasterMonitor& MasterMonitor::instance()
{
	static MasterMonitor inst;
	return inst;
}

MasterMonitor::MasterMonitor():
	flags(0),
	health(DeviceHealth::DeviceWorking)
{}

void MasterMonitor::setFlag(MasterFlags flag)
{
	flags |= static_cast<uint32_t>(flag);
	updateHealth();
}

void MasterMonitor::clearFlag(MasterFlags flag)
{
	flags &= ~static_cast<uint32_t>(flag);
	updateHealth();
}

bool MasterMonitor::hasFlag(MasterFlags flag) const
{
	return flags & static_cast<uint32_t>(flag);
}

uint32_t MasterMonitor::getFlags() const
{
	return flags;
}

DeviceHealth MasterMonitor::getHealth() const
{
	return health;
}

bool MasterMonitor::isPresent() const
{
	return health != DeviceHealth::DeviceDisabled;
}

void MasterMonitor::invoke()
{
	updateHealth();
}

void MasterMonitor::updateHealth()
{
	for (const auto& rule : rules) {
		if (flags & rule.mask) {
			health = rule.health;
		} else {
			health = DeviceHealth::DeviceWorking;
		}
	}

	sendDataToEventBus();
}

void MasterMonitor::sendDataToEventBus()
{
	// Отправим новые флаги в обсерверы
	Event ev;
	ev.type = EventType::UpdateSystemData;
	ev.data.systemData.flags = flags;
	EventBus::throwEvent(&ev, nullptr);

	// Отправим новый health
	Event ev2;
	ev2.type = EventType::UpdateDeviceHealth;
	ev2.data.updateHealth.type = DeviceType::Master;
	ev2.data.updateHealth.health = health;
	EventBus::throwEvent(&ev2, nullptr);
}