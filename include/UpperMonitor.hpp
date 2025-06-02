/*!
@file
@brief Сигнлтон для хранения флагов и health
@author V-Nezlo (vlladimirka@gmail.com)
@date 27.05.2025
@version 1.0
*/

#ifndef SOURCES_UPPERMONITOR_HPP_
#define SOURCES_UPPERMONITOR_HPP_

#include "BaseMonitor.hpp"

enum class UpperFlags : uint8_t {
	TopWaterLevelStuck = 0x01,
	PowerError = 0x02
};

class UpperMonitor : public BaseMonitor<UpperFlags> {
public:
	static UpperMonitor &instance();
	EventResult handleEvent(Event *e) override;

protected:
	const HealthRule *getRules() const override { return rules; }
	size_t getRulesCount() const override { return sizeof(rules) / sizeof(rules[0]); }

private:
	UpperMonitor();

	static constexpr HealthRule rules[] = {
		{DeviceHealth::DeviceError,
		 static_cast<uint8_t>(UpperFlags::PowerError)},

		{DeviceHealth::DeviceWarning,
		 static_cast<uint8_t>(UpperFlags::TopWaterLevelStuck)}};
};

#endif // SOURCES_UPPERMONITOR_HPP_