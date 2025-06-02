/*!
@file
@brief Сигнлтон для хранения флагов и health
@author V-Nezlo (vlladimirka@gmail.com)
@date 27.05.2025
@version 1.0
*/

#ifndef SOURCES_LOWERMONITOR_HPP_
#define SOURCES_LOWERMONITOR_HPP_

#include "BaseMonitor.hpp"

enum class LowerFlags : uint8_t {
	PumpOverCurrent     = 0x01,
	NoWater             = 0x02,
	TempSensorError     = 0x04,
	PHSensorError       = 0x08,
	PPMSensorError      = 0x10,
	PumpLowCurrent      = 0x20,
	WaterLevelError     = 0x40
};

class LowerMonitor : public BaseMonitor<LowerFlags> {
public:
	static LowerMonitor& instance();
	EventResult handleEvent(Event *e) override;

protected:
	const HealthRule* getRules() const override
	{
		return rules;
	}

	size_t getRulesCount() const override
	{
		return sizeof(rules) / sizeof(rules[0]);
	}

private:
	LowerMonitor();

	static constexpr HealthRule rules[] = {
		{ DeviceHealth::DeviceError,
			static_cast<uint8_t>(LowerFlags::NoWater) |
			static_cast<uint8_t>(LowerFlags::PumpOverCurrent) |
			static_cast<uint8_t>(LowerFlags::PumpLowCurrent) |
			static_cast<uint8_t>(LowerFlags::WaterLevelError) },

		{ DeviceHealth::DeviceWarning,
			static_cast<uint8_t>(LowerFlags::PHSensorError) |
			static_cast<uint8_t>(LowerFlags::PPMSensorError) |
			static_cast<uint8_t>(LowerFlags::TempSensorError) }
	};
};

#endif // SOURCES_LOWERMONITOR_HPP_