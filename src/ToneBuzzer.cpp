/*!
@file
@brief Класс тонового буззера
@author V-Nezlo (vlladimirka@gmail.com)
@date 10.09.2024
@version 1.0
*/

#include "Options.hpp"
#include "ToneBuzzer.hpp"
#include "driver/gpio.h"
#include "driver/ledc.h"

ToneBuzzer::ToneBuzzer(uint8_t aPin, uint8_t aPwmChannel):
	nextActionTime{0},
	currentSignal{ToneBuzzerSignal::Disabled},
	pin{aPin},
	ledcChannel{aPwmChannel},
	ledcTimer{static_cast<uint8_t>((aPwmChannel >> 1) & 3)},
	noteCounter{0},
	volume{0x3F}
{
	ledc_fade_func_install(0);

	ledc_timer_config_t timerConf{};
	timerConf.speed_mode = LEDC_HIGH_SPEED_MODE;
	timerConf.duty_resolution = LEDC_TIMER_10_BIT;
	timerConf.timer_num = static_cast<ledc_timer_t>(ledcTimer);
	timerConf.freq_hz = 1000;
	ledc_timer_config(&timerConf);

	ledc_channel_config_t channelConf{};
	channelConf.gpio_num = pin;
	channelConf.speed_mode = LEDC_HIGH_SPEED_MODE;
	channelConf.channel = static_cast<ledc_channel_t>(ledcChannel);
	channelConf.intr_type = LEDC_INTR_DISABLE;
	channelConf.timer_sel = timerConf.timer_num;
	channelConf.duty = 0;
	ledc_channel_config(&channelConf);
}

EventResult ToneBuzzer::handleEvent(Event *e)
{
	if (e->type == EventType::ToneBuzzerSignal) {
		if (currentSignal == ToneBuzzerSignal::Disabled) {
			currentSignal = e->data.buzToneSignal;
			noteCounter = 0;
		} else {
			signalQueue.push(e->data.buzToneSignal);
		}

		return EventResult::HANDLED;
	} else if (e->type == EventType::NewBuzVolume) {
		setVolume(e->data.volume);
		return EventResult::PASS_ON;
	} else {
		return EventResult::IGNORED;
	}
}

void ToneBuzzer::process(std::chrono::milliseconds aCurrentTime)
{
	if (aCurrentTime < nextActionTime) {
		return;
	}

	if (currentSignal != ToneBuzzerSignal::Disabled) {
		switch (currentSignal) {
			case ToneBuzzerSignal::Enabling:
				playEnablingSound(aCurrentTime);
				break;
			case ToneBuzzerSignal::Error:
				playErrorSound(aCurrentTime);
				break;
			case ToneBuzzerSignal::Warning:
				playWarningSignal(aCurrentTime);
				break;
			case ToneBuzzerSignal::Connected:
				playConnectedSound(aCurrentTime);
				break;
			case ToneBuzzerSignal::Disconnected:
				playDisconnectedSound(aCurrentTime);
				break;
			case ToneBuzzerSignal::Touch:
				playTouchSound(aCurrentTime);
				break;
			case ToneBuzzerSignal::CriticalError:
				playCriticalErrorSound(aCurrentTime);
				break;
			case ToneBuzzerSignal::Information:
				playInformationSignal(aCurrentTime);
				break;
			default:
				mute(aCurrentTime);
				break;
		}
	} else if (!signalQueue.isEmpty()) {
		ToneBuzzerSignal nextSignal;
		if (signalQueue.pop(nextSignal)) {
			currentSignal = nextSignal;
			noteCounter = 0;
		}
	}
}

void ToneBuzzer::setVolume(uint8_t aVolume)
{
	volume = aVolume;
}

void ToneBuzzer::setTone(Tones aTone, std::chrono::milliseconds aCurrentTime, std::chrono::milliseconds aPeriod)
{
	ledc_set_freq(LEDC_HIGH_SPEED_MODE, static_cast<ledc_timer_t>(ledcTimer), static_cast<uint32_t>(aTone));
	ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, static_cast<ledc_channel_t>(ledcChannel), volume, 0);
	nextActionTime = aCurrentTime + aPeriod;
	++noteCounter;
}

void ToneBuzzer::mute(std::chrono::milliseconds aCurrentTime)
{
	ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, static_cast<ledc_channel_t>(ledcChannel), 0, 0);
	currentSignal = ToneBuzzerSignal::Disabled;
	noteCounter = 0;
	nextActionTime = aCurrentTime + Options::kTimeBetweenSignals;
}

void ToneBuzzer::playSequence(const TonePeriod* sequence, size_t length, std::chrono::milliseconds aCurrentTime)
{
	if (noteCounter < length) {
		setTone(sequence[noteCounter].tone, aCurrentTime, sequence[noteCounter].duration);
	} else {
		mute(aCurrentTime);
	}
}

void ToneBuzzer::playEnablingSound(std::chrono::milliseconds aCurrentTime)
{
	static constexpr TonePeriod sequence[] = {
		{ Tones::C, 80ms },
		{ Tones::E, 80ms },
		{ Tones::G, 80ms },
		{ Tones::CH, 80ms },
		{ Tones::EH, 80ms }
	};
	playSequence(sequence, std::size(sequence), aCurrentTime);
}

void ToneBuzzer::playErrorSound(std::chrono::milliseconds aCurrentTime)
{
	static constexpr TonePeriod sequence[] = {
		{ Tones::G, 150ms },
		{ Tones::G, 150ms },
		{ Tones::G, 150ms }
	};
	playSequence(sequence, std::size(sequence), aCurrentTime);
}

void ToneBuzzer::playWarningSignal(std::chrono::milliseconds aCurrentTime)
{
	static constexpr TonePeriod sequence[] = {
		{ Tones::A, 100ms },
		{ Tones::A, 100ms }
	};
	playSequence(sequence, std::size(sequence), aCurrentTime);
}

void ToneBuzzer::playConnectedSound(std::chrono::milliseconds aCurrentTime)
{
	static constexpr TonePeriod sequence[] = {
		{Tones::C, 100ms},
		{Tones::E, 100ms},
		{Tones::G, 200ms}};
	playSequence(sequence, std::size(sequence), aCurrentTime);
}

void ToneBuzzer::playDisconnectedSound(std::chrono::milliseconds aCurrentTime)
{
	static constexpr TonePeriod sequence[] = {
		{Tones::G, 200ms},
		{Tones::E, 100ms},
		{Tones::C, 100ms}};
	playSequence(sequence, std::size(sequence), aCurrentTime);
}

void ToneBuzzer::playTouchSound(std::chrono::milliseconds aCurrentTime)
{
	static constexpr TonePeriod sequence[] = {
		{Tones::G, 50ms},
		{Tones::CH, 30ms}};
	playSequence(sequence, std::size(sequence), aCurrentTime);
}

void ToneBuzzer::playCriticalErrorSound(std::chrono::milliseconds aCurrentTime)
{
	static constexpr TonePeriod sequence[] = {
		{Tones::A, 150ms},
		{Tones::AS, 150ms},
		{Tones::A, 150ms},
		{Tones::AS, 150ms},
		{Tones::A, 150ms},
		{Tones::AS, 150ms},
		{Tones::A, 150ms},
		{Tones::AS, 150ms}
	};
	playSequence(sequence, std::size(sequence), aCurrentTime);
}

void ToneBuzzer::playInformationSignal(std::chrono::milliseconds aCurrentTime)
{
	static constexpr TonePeriod sequence[] = {
		{Tones::F, 120ms},
		{Tones::D, 120ms},
		{Tones::CH, 180ms}
	};
	playSequence(sequence, std::size(sequence), aCurrentTime);
}
