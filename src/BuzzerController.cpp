/*!
@file
@brief Обработчик для буззера
@author V-Nezlo (vlladimirka@gmail.com)
@date 17.04.2024
@version 1.0
*/

#include "BuzzerController.hpp"

BuzzerController::BuzzerController(uint8_t aPin, uint8_t aPwmChannel) :
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

EventResult BuzzerController::handleEvent(Event *e)
{
	switch(e->type) {
		case EventType::BuzzerSignal:
			doAction(e->data.buzSignal);
			return EventResult::HANDLED;
		default:
			return EventResult::IGNORED;
	}
}

void BuzzerController::process(std::chrono::milliseconds aCurrentTime)
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

void BuzzerController::doAction(BuzzerSignal aSignal)
{
	currentSignal = aSignal;
	lastActionTime = TimeWrapper::milliseconds();

	if (aSignal != BuzzerSignal::Disable) {
		setBuzzerState(true);
	}
}

void BuzzerController::setSoundVolume(uint8_t aDuty)
{
	pwmDuty = aDuty;
}

void BuzzerController::setBuzzerState(bool aState)
{
	if (aState) {
		pwmInstance.setBrightness(pwmDuty);
	} else {
		pwmInstance.setBrightness(0);
	}
}

