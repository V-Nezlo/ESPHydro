/*!
@file
@brief Базовый класс для мониторов устройств
@author V-Nezlo (vlladimirka@gmail.com)
@date 27.05.2025
@version 1.0
*/

#ifndef SOURCES_BASEMONITOR_HPP_
#define SOURCES_BASEMONITOR_HPP_

#include "EventBus.hpp"
#include "Types.hpp"
#include <cstdint>
#include <type_traits>

template<typename FlagType>
class BaseMonitor : public AbstractEventObserver {
private:
	using FlagValueType = std::underlying_type_t<FlagType>;
public:
	void setFlag(FlagType aFlag)
	{
		flags |= static_cast<FlagValueType>(aFlag);
		updateHealth();
	}

	void clearFlag(FlagType aFlag)
	{
		flags &= ~static_cast<FlagValueType>(aFlag);
		updateHealth();
	}

	bool hasFlag(FlagType aFlag) const
	{
		return flags & static_cast<FlagValueType>(aFlag);
	}

	FlagValueType getFlags() const
	{
		return flags;
	}

	DeviceHealth getHealth() const
	{
		return health;
	}

	bool isPresent() const
	{
		return health != DeviceHealth::DeviceDisabled;
	}

	void updateFromTelemetry(FlagValueType aTelemetryFlags)
	{
		flags = aTelemetryFlags;
		updateHealth();
	}

protected:
	BaseMonitor(DeviceType aDeviceType, DeviceHealth aInitialHealth = DeviceHealth::DeviceDisabled):
		flags(0),
		health(aInitialHealth),
		deviceType(aDeviceType)
	{}

	virtual const HealthRule* getRules() const = 0;
	virtual size_t getRulesCount() const = 0;

	void updateHealth()
	{
		DeviceHealth newHealth = DeviceHealth::DeviceWorking;

		const HealthRule* rules = getRules();
		size_t rulesCount = getRulesCount();

		for (size_t i = 0; i < rulesCount; ++i) {
			if (flags & rules[i].mask) {
				newHealth = rules[i].health;
				break;
			}
		}

		if (newHealth != health) {
			health = newHealth;
			sendNewHealthToEventBus();
		}
	}

	void handleDeviceDetached(DeviceType aDetachedDeviceType)
	{
		if (aDetachedDeviceType == deviceType) {
			flags = 0;
			health = DeviceHealth::DeviceDisabled;
			sendNewHealthToEventBus();
		}
	}

	void handleDeviceAttached(DeviceType aAttachedDeviceType)
	{
		if (aAttachedDeviceType == deviceType) {
			Event ev2;
			ev2.type = EventType::ToneBuzzerSignal;
			ev2.data.buzToneSignal = ToneBuzzerSignal::Connected;
			EventBus::throwEvent(&ev2, nullptr);
		}
	}

	static inline ToneBuzzerSignal getSignalForHealth(DeviceHealth aHealth)
	{
		switch(aHealth) {
			case DeviceHealth::DeviceWorking:
				return ToneBuzzerSignal::Information;
			case DeviceHealth::DeviceError:
				return ToneBuzzerSignal::Error;
			case DeviceHealth::DeviceCritical:
				return ToneBuzzerSignal::CriticalError;
			case DeviceHealth::DeviceWarning:
				return ToneBuzzerSignal::Warning;
			case DeviceHealth::DeviceDisabled:
				return ToneBuzzerSignal::Disconnected;
		}

		return ToneBuzzerSignal::Information;
	}

private:
	void sendNewHealthToEventBus()
	{
		// Отправим новый health
		Event ev;
		ev.type = EventType::UpdateDeviceHealth;
		ev.data.updateHealth.type = deviceType;
		ev.data.updateHealth.health = health;
		EventBus::throwEvent(&ev, nullptr);

		// Отправим звуковой сигнал исходя из нового health
		Event ev2;
		ev2.type = EventType::ToneBuzzerSignal;
		ev2.data.buzToneSignal = getSignalForHealth(health);
		EventBus::throwEvent(&ev2, nullptr);
	}

protected:
	FlagValueType flags;
	DeviceHealth health;

private:
	DeviceType deviceType;
};

#endif // SOURCES_BASEMONITOR_HPP_