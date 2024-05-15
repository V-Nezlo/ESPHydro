/*!
@file
@brief Обработчик для буззера
@author V-Nezlo (vlladimirka@gmail.com)
@date 17.04.2024
@version 1.0
*/

#include "TimeWrapper.hpp"
#include "EventBus.hpp"
#include <LovyanGFX.hpp>
#include <chrono>

#ifndef SOURCES_BUZZERCONTROLLER_HPP_
#define SOURCES_BUZZERCONTROLLER_HPP_

class BuzzerController : public AbstractEventObserver {
	lgfx::Light_PWM pwmInstance;
	std::chrono::milliseconds lastActionTime;
	BuzzerSignal currentSignal;
	uint8_t pwmDuty;

	const std::chrono::milliseconds kShortBeepDuraction{100};
	const std::chrono::milliseconds kLongBeepDuraction{500};

public:
	BuzzerController(uint8_t aPin, uint8_t aPwmChannel) :
		pwmInstance{}, 
		lastActionTime{0}, 
		currentSignal{BuzzerSignal::Disable},
		pwmDuty{100}
	{
		auto config = pwmInstance.config();

		config.pin_bl = aPin;             // Пин
		config.invert = false;            // Инвертирование канала
		config.freq   = 44100;            // Частота шим подсветки
		config.pwm_channel = aPwmChannel; // Номер канала ШИМ для использования
		
		pwmInstance.config(config);
		pwmInstance.init(0);
	}

	EventResult handleEvent(Event *e) override
	{
		switch(e->type) {
			case EventType::BuzzerSignal:
				doAction(e->data.buzSignal);
				return EventResult::HANDLED;
			default:
				return EventResult::IGNORED;
		}
	}

	void process(std::chrono::milliseconds aCurrentTime)
	{
		switch (currentSignal) {
			case BuzzerSignal::Disable:
				return;
				break;
			case BuzzerSignal::Short:
				if (aCurrentTime > lastActionTime + kShortBeepDuraction) {
					setBuzzerState(false);
				}
				break;
			case BuzzerSignal::Long:
				if (aCurrentTime > lastActionTime + kLongBeepDuraction) {
					setBuzzerState(false);
				}
				break;
		}
	}

private:
	void doAction(BuzzerSignal aSignal)
	{
		currentSignal = aSignal;
		lastActionTime = TimeWrapper::milliseconds();
		
		if (aSignal != BuzzerSignal::Disable) {
			setBuzzerState(true);
		}
	}

	void setSoundVolume(uint8_t aDuty)
	{
		pwmDuty = aDuty;
	}

	void setBuzzerState(bool aState)
	{
		if (aState) {
			pwmInstance.setBrightness(pwmDuty);
		} else {
			pwmInstance.setBrightness(0);
		}
	}
};

#endif