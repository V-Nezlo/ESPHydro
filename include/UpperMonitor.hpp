/*!
@file
@brief Сигнлтон для хранения флагов и health
@author V-Nezlo (vlladimirka@gmail.com)
@date 27.05.2025
@version 1.0
*/

#ifndef SOURCES_UPPERMONITOR_HPP_
#define SOURCES_UPPERMONITOR_HPP_

#include "Types.hpp"
#include <cstdint>

enum class UpperFlags : uint8_t {
	TopWaterLevelStuck = 0x01,
	PowerError         = 0x02
};

class UpperMonitor {
public:
	static UpperMonitor& instance();

	void setFlag(UpperFlags flag);
	void clearFlag(UpperFlags flag);
	bool hasFlag(UpperFlags flag) const;

	uint8_t getFlags() const;
	DeviceHealth getHealth() const;

	void updateFromTelemetry(uint8_t telemetryFlags);

private:
	UpperMonitor();
	void updateHealth();

	uint8_t flags;
	DeviceHealth health;

	static constexpr HealthRule rules[] = {
		{ DeviceHealth::DeviceError,
			static_cast<uint8_t>(UpperFlags::PowerError) },

		{ DeviceHealth::DeviceWarning,
			static_cast<uint8_t>(UpperFlags::TopWaterLevelStuck) }
	};
};

#endif // SOURCES_UPPERMONITOR_HPP_