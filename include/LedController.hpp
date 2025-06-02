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
#include <chrono>

class LedController : public AbstractEventObserver, public AbstractLinearTask {
	using milliseconds = std::chrono::milliseconds;

	class SmartLed {
	public:
		enum class LedState {
			Disabled,
			Stably,
			FastBlinking,
			LongBlinking
		};

	private:
		milliseconds lastActionTime;
		LedState state;
		AbstractGpio *gpio;
		bool active;
		bool level;
		const bool present;

	public:
		SmartLed(AbstractGpio *aGpio, bool aLevel):
			lastActionTime{0},
			state{LedState::Disabled},
			gpio{aGpio},
			active{false},
			level{aLevel},
			present{aGpio != nullptr}
		{
			if (gpio != nullptr) {
				level ? gpio->reset() : gpio->set();
			}
		}

		void process(milliseconds aCurrentTime)
		{
			if (!present) {
				return;
			}

			switch(state) {
				case LedState::Disabled:
					if (active) {
						active = false;
						level ? gpio->reset() : gpio->set();
					}
					break;
				case LedState::Stably:
					if (!active) {
						active = true;
						level ? gpio->set() : gpio->reset();
					}
					break;
				case LedState::FastBlinking:
					if (aCurrentTime > lastActionTime + milliseconds{200}) {
						lastActionTime = aCurrentTime;
						active = !active;
						gpio->setState(active);
					}
					break;
				case LedState::LongBlinking:
					if (aCurrentTime > lastActionTime + milliseconds{1000}) {
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

		bool isPresent()
		{
			return present;
		}
	};

public:
	LedController(AbstractGpio *aGreen, AbstractGpio *aBlue, AbstractGpio *aRed, bool aLevel):
		green{aGreen, aLevel},
		blue{aBlue, aLevel},
		red{aRed, aLevel}
	{
		green.setState(SmartLed::LedState::LongBlinking);
	}

	void process(milliseconds aCurrentTime) override
	{
		if (green.isPresent()) green.process(aCurrentTime);
		if (blue.isPresent()) blue.process(aCurrentTime);
		if (red.isPresent()) red.process(aCurrentTime);
	}

	EventResult handleEvent(Event *e) override
	{
		switch(e->type) {
			case EventType::UpdateDeviceHealth:
				if (e->data.updateHealth.type == DeviceType::Master) {
					switch(e->data.updateHealth.health) {
						case DeviceHealth::DeviceWorking:
							green.setState(SmartLed::LedState::LongBlinking);
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
				} return EventResult::PASS_ON;
			default:
				return EventResult::IGNORED;
		}
	}

private:
	SmartLed green;
	SmartLed blue;
	SmartLed red;
};

#endif // INCLUDE_LEDCONTROLLER_HPP_