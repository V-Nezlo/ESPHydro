/*!
@file
@brief Сигнлтон для хранения флагов и health
@author V-Nezlo (vlladimirka@gmail.com)
@date 27.05.2025
@version 1.0
*/

#ifndef SOURCES_LOWERMONITOR_HPP_
#define SOURCES_LOWERMONITOR_HPP_

#include "Types.hpp"
#include <cstdint>

enum class LowerFlags : uint8_t {
	PumpOverCurrent     = 0x01,
	NoWater             = 0x02,
	TempSensorError     = 0x04,
	PHSensorError       = 0x08,
	PPMSensorError      = 0x10,
	PumpLowCurrent      = 0x20,
	Calibration         = 0x40,
	PumpStuck           = 0x80
};

class LowerMonitor {
public:
	static LowerMonitor& instance();

	void setFlag(LowerFlags flag);
	void clearFlag(LowerFlags flag);
	bool hasFlag(LowerFlags flag) const;

	uint8_t getFlags() const;
	DeviceHealth getHealth() const;

	void updateFromTelemetry(uint8_t telemetryFlags);

private:
	LowerMonitor();
	void updateHealth();

	uint8_t flags;
	DeviceHealth health;

	static constexpr HealthRule rules[] = {
		{ DeviceHealth::DeviceError,
			static_cast<uint8_t>(LowerFlags::NoWater) |
			static_cast<uint8_t>(LowerFlags::PumpOverCurrent) |
			static_cast<uint8_t>(LowerFlags::PumpLowCurrent) },

		{ DeviceHealth::DeviceWarning,
			static_cast<uint8_t>(LowerFlags::PHSensorError) |
			static_cast<uint8_t>(LowerFlags::PPMSensorError) |
			static_cast<uint8_t>(LowerFlags::TempSensorError) |
			static_cast<uint8_t>(LowerFlags::Calibration) }
	};
};

#endif // SOURCES_LOWERMONITOR_HPP_