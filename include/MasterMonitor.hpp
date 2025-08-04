/*!
@file
@brief Сигнлтон для хранения флагов и health
@author V-Nezlo (vlladimirka@gmail.com)
@date 27.05.2025
@version 1.0
*/

#ifndef SOURCES_MASTERMONITOR_HPP_
#define SOURCES_MASTERMONITOR_HPP_

#include "BaseMonitor.hpp"
#include "LinearSched.hpp"

enum class MasterFlags : uint32_t {
	RTCError               = 1 << 0,
	InternalMemError       = 1 << 1,
	RSBusError             = 1 << 2,
	PumpNotOperate         = 1 << 3,
	TankNotFloodedInTime   = 1 << 4,
	Leak                   = 1 << 5,
	PCFError               = 1 << 6,
	DeviceMismatch         = 1 << 7,
	SystemInitialized      = 1 << 8,
};

class MasterMonitor : public BaseMonitor<MasterFlags> {
public:
	static MasterMonitor& instance();
	EventResult handleEvent(Event *e) override;
	void invoke();

protected:
	const HealthRule* getRules() const override { return rules; }
	size_t getRulesCount() const override { return sizeof(rules) / sizeof(rules[0]); }

private:
	MasterMonitor();

	static constexpr HealthRule rules[] = {
		{ DeviceHealth::DeviceCritical,
			static_cast<uint32_t>(MasterFlags::Leak) |
			static_cast<uint32_t>(MasterFlags::RSBusError) },

		{ DeviceHealth::DeviceError,
			static_cast<uint32_t>(MasterFlags::RTCError) |
			static_cast<uint32_t>(MasterFlags::InternalMemError) |
			static_cast<uint32_t>(MasterFlags::PumpNotOperate) |
			static_cast<uint32_t>(MasterFlags::PCFError) |
			static_cast<uint32_t>(MasterFlags::DeviceMismatch) },

		{ DeviceHealth::DeviceWarning,
			static_cast<uint32_t>(MasterFlags::TankNotFloodedInTime) }
	};
};

#endif // SOURCES_MASTERMONITOR_HPP_