/*!
@file
@brief Обработчик светодиодов
@author V-Nezlo (vlladimirka@gmail.com)
@date 11.06.2024
@version 1.0
*/

#ifndef INCLUDE_LEDCONTROLLER_HPP_
#define INCLUDE_LEDCONTROLLER_HPP_

#include "Gpio.hpp"
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
		Gpio *gpio;
		bool active;
		const bool present;

	public:
		SmartLed(Gpio *aGpio):
			lastActionTime{0},
			state{LedState::Disabled},
			gpio{aGpio},
			active{false},
			present{aGpio != nullptr}
		{
			if (gpio != nullptr) {
				gpio->reset();
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
						gpio->reset();
					}
					break;
				case LedState::Stably:
					if (!active) {
						active = true;
						gpio->set();
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
	LedController(Gpio *aGreen, Gpio *aBlue, Gpio *aRed):
		green{aGreen},
		blue{aBlue},
		red{aRed}
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
			case EventType::UpdateSystemData:
				if (e->data.systemData.flags != 0) {
					red.setState(SmartLed::LedState::LongBlinking);
					green.setState(SmartLed::LedState::FastBlinking);
				} else {
					red.setState(SmartLed::LedState::Disabled);
					green.setState(SmartLed::LedState::LongBlinking);
				}
				return EventResult::PASS_ON;
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