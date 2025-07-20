/*!
@file
@brief Обработчик светодиодов
@author V-Nezlo (vlladimirka@gmail.com)
@date 11.06.2024
@version 1.0
*/

#ifndef INCLUDE_LEDCONTROLLER_HPP_
#define INCLUDE_LEDCONTROLLER_HPP_

#include "AbstractClasses.hpp"
#include "EventBus.hpp"
#include "LinearSched.hpp"
#include "MasterMonitor.hpp"
#include "Types.hpp"
#include <chrono>

class LedController : public AbstractEventObserver, public AbstractLinearTask {
	using milliseconds = std::chrono::milliseconds;

	class SmartLed {
		static constexpr milliseconds kFastBlinkingInterval{200};
		static constexpr milliseconds kSlowBlinkingInterval{1000};
	public:
		enum class LedState {
			Disabled,
			Steady,
			FastBlinking,
			SlowBlinking
		};

	private:
		milliseconds lastActionTime;
		LedState state;
		AbstractGpio *gpio;
		bool active;
		const bool level;

	public:
		SmartLed(AbstractGpio *aGpio, bool aLevel):
			lastActionTime{0},
			state{LedState::Disabled},
			gpio{aGpio},
			active{false},
			level{aLevel}
		{
			assert(aGpio);
			level ? gpio->reset() : gpio->set();
		}

		void process(milliseconds aCurrentTime)
		{
			switch(state) {
				case LedState::Disabled:
					if (active) {
						active = false;
						level ? gpio->reset() : gpio->set();
					}
					break;
				case LedState::Steady:
					if (!active) {
						active = true;
						level ? gpio->set() : gpio->reset();
					}
					break;
				case LedState::FastBlinking:
					if (aCurrentTime > lastActionTime + kFastBlinkingInterval) {
						lastActionTime = aCurrentTime;
						active = !active;
						gpio->setState(active);
					}
					break;
				case LedState::SlowBlinking:
					if (aCurrentTime > lastActionTime + kSlowBlinkingInterval) {
						lastActionTime = aCurrentTime;
						active = !active;
						gpio->setState(active);
					}
					break;
			}
		}

		void setState(LedState aState)
		{
			state = aState;
		}
	};

public:
	LedController(AbstractGpio *aGreen, AbstractGpio *aBlue, AbstractGpio *aRed, bool aLevel):
		green{aGreen, aLevel},
		blue{aBlue, aLevel},
		red{aRed, aLevel},
		pumpMode{PumpModes::EBBNormal},
		recoverTime{0}
	{
	}

	void process(milliseconds aCurrentTime) override
	{
		green.process(aCurrentTime);
		blue.process(aCurrentTime);
		red.process(aCurrentTime);

		// Обновим состояние светодиодов в любом случае для избежания залипаний
		if (aCurrentTime > recoverTime) {
			recoverTime = aCurrentTime + Options::kLedRecoverTime;
			update();
		}
	}

	EventResult handleEvent(Event *e) override
	{
		switch(e->type) {
			case EventType::UpdateDeviceHealth:
				if (e->data.updateHealth.type == DeviceType::Master) {
					update();
				} return EventResult::PASS_ON;
			case EventType::SettingsUpdated:
				pumpMode = e->data.settings.pump.mode;
				update();
				return EventResult::PASS_ON;
			default:
				return EventResult::IGNORED;
		}
	}

private:
	SmartLed green;
	SmartLed blue;
	SmartLed red;

	PumpModes pumpMode;
	milliseconds recoverTime;
	
	void update()
	{
		switch(MasterMonitor::instance().getHealth()) {
			case DeviceHealth::DeviceWorking:
				if (pumpMode == PumpModes::Maintance) {
					green.setState(SmartLed::LedState::SlowBlinking);
				} else {
					green.setState(SmartLed::LedState::Steady);
				}

				red.setState(SmartLed::LedState::Disabled);
				break;
			case DeviceHealth::DeviceWarning:
				green.setState(SmartLed::LedState::FastBlinking);
				red.setState(SmartLed::LedState::Disabled);
			break;
				case DeviceHealth::DeviceError:
				// Fallthrough
			case DeviceHealth::DeviceCritical:
				red.setState(SmartLed::LedState::FastBlinking);
				green.setState(SmartLed::LedState::Disabled);
				break;
			default:
				break;
		}
	}
};

#endif // INCLUDE_LEDCONTROLLER_HPP_