/*!
@file
@brief Класс тонового буззера
@author V-Nezlo (vlladimirka@gmail.com)
@date 10.09.2024
@version 1.0
*/

#include "ToneBuzzer.hpp"
#include "driver/gpio.h"
#include "driver/ledc.h"

using milliseconds = std::chrono::milliseconds;

ToneBuzzer::ToneBuzzer(uint8_t aPin, uint8_t aPwmChannel):
	nextActionTime{},
	currentSignal{ToneBuzzerSignal::Disabled},
	pin{aPin},
	ledcChannel{aPwmChannel},
	ledcTimer{static_cast<uint8_t>((aPwmChannel >> 1) & 3)},
	noteCounter{0},
	volume{0x3F}
{
}

EventResult ToneBuzzer::handleEvent(Event *e)
{
	if (e->type == EventType::ToneBuzzerSignal) {
		currentSignal = e->data.buzToneSignal;
		noteCounter = 0;
		return EventResult::HANDLED;
	} else {
		return EventResult::IGNORED;
	}
}

void ToneBuzzer::process(std::chrono::milliseconds aCurrentTime)
{
	if (currentSignal != ToneBuzzerSignal::Disabled && aCurrentTime > nextActionTime) {
		switch(currentSignal) {
			case ToneBuzzerSignal::Enabling:
				playEnablingSound(aCurrentTime);
				break;
			case ToneBuzzerSignal::Error:
				break;
			case ToneBuzzerSignal::Warning:
				break;
			default:
				break;
		}
	}
}

void ToneBuzzer::setVolume(uint8_t aVolume)
{
	volume = aVolume;
}

void ToneBuzzer::setTone(Tones aTone, std::chrono::milliseconds aCurrentTime, std::chrono::milliseconds aPeriod)
{
	ledc_timer_config_t timerConf;
	timerConf.speed_mode = LEDC_HIGH_SPEED_MODE;
	timerConf.duty_resolution = LEDC_TIMER_10_BIT;
	timerConf.timer_num = static_cast <ledc_timer_t>(ledcTimer);
	timerConf.freq_hz = static_cast<uint32_t>(aTone);
	ledc_timer_config(&timerConf);

	ledc_channel_config_t channelConf;
	channelConf.gpio_num = pin;
	channelConf.speed_mode = LEDC_HIGH_SPEED_MODE;
	channelConf.channel = static_cast<ledc_channel_t>(ledcChannel);
	channelConf.intr_type = LEDC_INTR_DISABLE;
	channelConf.timer_sel = timerConf.timer_num;
	channelConf.duty = 0;
	ledc_channel_config(&channelConf);

	ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, channelConf.channel, volume, 0);

	nextActionTime = aCurrentTime + aPeriod;
	++noteCounter;
}

void ToneBuzzer::mute()
{
	ledc_timer_config_t timerConf;
	timerConf.speed_mode = LEDC_HIGH_SPEED_MODE;
	timerConf.duty_resolution = LEDC_TIMER_10_BIT;
	timerConf.timer_num = static_cast<ledc_timer_t>(ledcTimer);
	timerConf.freq_hz = static_cast<uint32_t>(0);
	ledc_timer_config(&timerConf);

	ledc_channel_config_t channelConf;
	channelConf.gpio_num = pin;
	channelConf.speed_mode = LEDC_HIGH_SPEED_MODE;
	channelConf.channel = static_cast<ledc_channel_t>(ledcChannel);
	channelConf.intr_type = LEDC_INTR_DISABLE;
	channelConf.timer_sel = timerConf.timer_num;
	channelConf.duty = 0;
	ledc_channel_config(&channelConf);
	ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, channelConf.channel, 0, 0);

	currentSignal = ToneBuzzerSignal::Disabled;
	noteCounter = 0;
}

void ToneBuzzer::playEnablingSound(std::chrono::milliseconds aCurrentTime)
{
	switch(noteCounter) {
		case 0:
			setTone(Tones::A, aCurrentTime, milliseconds(50)); break;
		case 1:
			setTone(Tones::A, aCurrentTime, milliseconds(50)); break;
		case 2:
			setTone(Tones::A, aCurrentTime, milliseconds(50)); break;
		case 3:
			setTone(Tones::A, aCurrentTime, milliseconds(50)); break;
		case 4:
			setTone(Tones::A, aCurrentTime, milliseconds(50)); break;
		case 5:
			setTone(Tones::A, aCurrentTime, milliseconds(50)); break;
		default:
			mute();
			break;
		break;
	}
}

void ToneBuzzer::playErrorSound(std::chrono::milliseconds aCurrentTime)
{

}
void ToneBuzzer::playWarningSignal(std::chrono::milliseconds aCurrentTime)
{

}