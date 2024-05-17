/*!
@file
@brief Обработчик для буззера
@author V-Nezlo (vlladimirka@gmail.com)
@date 17.04.2024
@version 1.0
*/

#include "EventBus.hpp"
#include "LinearSched.hpp"
#include "TimeWrapper.hpp"
#include <LovyanGFX.hpp>
#include <chrono>

#ifndef SOURCES_BUZZERCONTROLLER_HPP_
#define SOURCES_BUZZERCONTROLLER_HPP_

class BuzzerController : public AbstractEventObserver, public AbstractLinearTask {
	lgfx::Light_PWM pwmInstance;
	std::chrono::milliseconds lastActionTime;
	BuzzerSignal currentSignal;
	uint8_t pwmDuty;

	const std::chrono::milliseconds kShortBeepDuraction{100};
	const std::chrono::milliseconds kLongBeepDuraction{500};

public:
	BuzzerController(uint8_t aPin, uint8_t aPwmChannel);
	EventResult handleEvent(Event *e) override;
	void process(std::chrono::milliseconds aCurrentTime) override;

private:
	void doAction(BuzzerSignal aSignal);
	void setSoundVolume(uint8_t aDuty);
	void setBuzzerState(bool aState);
};

#endif // SOURCES_BUZZERCONTROLLER_HPP_