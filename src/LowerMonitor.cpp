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

void LowerMonitor::updateHealth()
{
	for (const auto& rule : rules) {
		if (flags & rule.mask) {
			health = rule.health;
			return;
		}
	}
	health = DeviceHealth::DeviceWorking;
}