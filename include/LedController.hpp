/*!
@file
@brief Обработчик светодиодов
@author V-Nezlo (vlladimirka@gmail.com)
@date 11.06.2024
@version 1.0
*/

#ifndef INCLUDE_LEDCONTROLLER_HPP_
#define INCLUDE_LEDCONTROLLER_HPP_

#include "GpioWrapper.hpp"
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
		Gpio &gpio;
		bool active;

	public:
		SmartLed(Gpio &aGpio):
			lastActionTime{0},
			state{LedState::Disabled},
			gpio{aGpio},
			active{false}
		{
			gpio.reset();
		}

		void process(milliseconds aCurrentTime)
		{
			switch(state) {
				case LedState::Disabled:
					if (active) {
						active = false;
						gpio.reset();
					}
					break;
				case LedState::Stably:
					if (!active) {
						active = true;
						gpio.set();
					}
					break;
				case LedState::FastBlinking:
					if (aCurrentTime > lastActionTime + milliseconds{200}) {
						lastActionTime = aCurrentTime;
						active = !active;
						gpio.setState(active);
					}
					break;
				case LedState::LongBlinking:
					if (aCurrentTime > lastActionTime + milliseconds{1000}) {
						lastActionTime = aCurrentTime;
						active = !active;
						gpio.setState(active);
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
	LedController(Gpio &aGreen, Gpio &aBlue, Gpio &aRed):
		green{aGreen},
		blue{aBlue},
		red{aRed}
	{

	}

	void process(milliseconds aCurrentTime) override
	{
		green.process(aCurrentTime);
		blue.process(aCurrentTime);
		red.process(aCurrentTime);
	}

	EventResult handleEvent(Event *e) override
	{
		switch(e->type) {
			case EventType::UpdateSystemData:
				if (e->data.systemData.flags != 0) {
					red.setState(SmartLed::LedState::LongBlinking);
				} else {
					red.setState(SmartLed::LedState::Disabled);
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