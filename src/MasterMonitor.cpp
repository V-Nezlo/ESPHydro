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
	flags |= static_cast<uint8_t>(flag);
	updateHealth();
}

void MasterMonitor::clearFlag(MasterFlags flag)
{
	flags &= ~static_cast<uint8_t>(flag);
	updateHealth();
}

bool MasterMonitor::hasFlag(MasterFlags flag) const
{
	return flags & static_cast<uint8_t>(flag);
}

uint8_t MasterMonitor::getFlags() const
{
	return flags;
}

DeviceHealth MasterMonitor::getHealth() const
{
	return health;
}

void MasterMonitor::updateHealth()
{
	for (const auto& rule : rules) {
		if (flags & rule.mask) {
			health = rule.health;
			return;
		}
	}
	health = DeviceHealth::DeviceWorking;
}