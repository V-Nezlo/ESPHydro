/*!
@file
@brief Класс тонового буззера
@author V-Nezlo (vlladimirka@gmail.com)
@date 10.09.2024
@version 1.0
*/

#include "EventBus.hpp"
#include "LinearSched.hpp"
#include "TimeWrapper.hpp"
#include <chrono>

#ifndef SOURCES_TONEBUZZER_HPP_
#define SOURCES_TONEBUZZER_HPP_

class ToneBuzzer : public AbstractEventObserver, public AbstractLinearTask
{
    enum class Tones {
        C = 261, // До I
        D = 294, // Ре I
        E = 329, // Ми I
        F = 349, // Фа I
        G = 392, // Соль I
        GS = 415, // Соль I#
        A = 440, // Ля I
        AS = 466, // Ля I#
        B = 493, // Си I
        CH = 523, // До II
        CSH = 554, // До II#
        DH = 587, // Ре II
        DSH = 622, // Ре II#
        EH = 659, // Ми II
        FH = 698, // Фа II
        FSH = 740, // Фа II#
        GH = 784, // Соль II
        GSH = 830, // Соль II#
        AH = 880, // Ля II
        ASH = 932, // Ля II#
        BH = 987 // Си II
    };

public:
    ToneBuzzer(uint8_t aPin, uint8_t aLedcChannel);
    EventResult handleEvent(Event *e) override;
    void process(std::chrono::milliseconds aCurrentTime) override;

private:
    std::chrono::milliseconds nextActionTime;
    ToneBuzzerSignal currentSignal;
    const uint8_t pin;
    const uint8_t ledcChannel;
    const uint8_t ledcTimer;
    uint32_t noteCounter;
    uint8_t volume;

    void setVolume(uint8_t aVolume);
    void setTone(Tones aTone, std::chrono::milliseconds aCurrentTime, std::chrono::milliseconds aPeriod);
    void mute();

    void playEnablingSound(std::chrono::milliseconds aCurrentTime);
    void playErrorSound(std::chrono::milliseconds aCurrentTime);
    void playWarningSignal(std::chrono::milliseconds aCurrentTime);
};

#endif // SOURCES_TONEBUZZER_HPP_