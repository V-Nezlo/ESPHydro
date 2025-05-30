/*!
@file
@brief Сигнлтон для хранения флагов и health
@author V-Nezlo (vlladimirka@gmail.com)
@date 27.05.2025
@version 1.0
*/

#ifndef SOURCES_MASTERMONITOR_HPP_
#define SOURCES_MASTERMONITOR_HPP_

#include "EventBus.hpp"
#include "Types.hpp"
#include <cstdint>

enum class MasterFlags : uint32_t {
	RTCError               = 0x01,
	InternalMemError       = 0x02,
	RSBusError             = 0x04,
	PumpNotOperate         = 0x08,
	TankNotFloodedInTime   = 0x10,
	Leak                   = 0x20,
	PCFError               = 0x40,
	DeviceMismatch         = 0x80
};

class MasterMonitor {
public:
	static MasterMonitor& instance();

	void setFlag(MasterFlags flag);
	void clearFlag(MasterFlags flag);
	bool hasFlag(MasterFlags flag) const;

	uint32_t getFlags() const;
	DeviceHealth getHealth() const;
	bool isPresent() const;

private:
	MasterMonitor();
	void updateHealth();
	void sendDataToEventBus();

	uint32_t flags;
	DeviceHealth health;

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