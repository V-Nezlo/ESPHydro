/*!
@file
@brief Обработчик лампы
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#ifndef INCLUDE_LIGHTCONTROLLER_HPP_
#define INCLUDE_LIGHTCONTROLLER_HPP_

#include "EventBus.hpp"
#include "ConfigStorage.hpp"
#include "GpioWrapper.hpp"
#include "TimeWrapper.hpp"

class LightHandler : public AbstractEventObserver {
public:
    LightHandler() :
        enabled{false},
        lampOnTime{0,0,0},
        lampOffTime{0,0,0},
        currentTime{0,0,0},
        lastCheckTime{0}
    {

    }

    EventResult handleEvent(Event *e) override
    {
        switch (e->type) {
            case EventType::GetCurrentTime:
                currentTime = e->data.time;
                return EventResult::PASS_ON;
                break;
            case EventType::SettingsFirstLoad:
                // Fallthrough
            case EventType::SettingsUpdated:
                enabled = e->data.settings.lamp.enabled;
                lampOnTime.hour = e->data.settings.lamp.lampOnHour;
                lampOnTime.minutes = e->data.settings.lamp.lampOnMin;
                lampOffTime.hour = e->data.settings.lamp.lampOffHour;
                lampOffTime.minutes = e->data.settings.lamp.lampOffMin;
                return EventResult::PASS_ON;
                break;
            default:
                return EventResult::IGNORED;
        }
    }

    void process(std::chrono::milliseconds aCurrentInternalTime)
    {
        if (aCurrentInternalTime > lastCheckTime + std::chrono::milliseconds{5000}) {
            lastCheckTime = aCurrentInternalTime;

            const bool isNowIsActiveTime = isTimeForOn(currentTime, lampOnTime, lampOffTime);

            if (!enabled && isNowIsActiveTime) {
                sendCommandToEventBus(true);
            } else if (enabled && !isNowIsActiveTime) {
                sendCommandToEventBus(false);
            }
        }
    }

private:
    bool enabled;
    Time lampOnTime;
    Time lampOffTime;
    Time currentTime;

    std::chrono::milliseconds lastCheckTime;

    void sendCommandToEventBus(bool aNewLampState)
    {
        Event ev;
        ev.type = EventType::ActionRequest;
        ev.data.action = aNewLampState ? Action::TurnLampOn : Action::TurnLampOff;
        EventBus::throwEvent(&ev);
    }

    /// @brief Функция написана ChatGPT
    /// @param currentTime
    /// @param startTime
    /// @param endTime
    /// @return True если время находится в промежутке для включения, false - если для выключения
    bool isTimeForOn(const Time& currentTime, const Time& startTime, const Time& endTime)
    {
        if (startTime.hour < endTime.hour) {
            return (currentTime.hour > startTime.hour || (currentTime.hour == startTime.hour &&
                    (currentTime.minutes > startTime.minutes || (currentTime.minutes == startTime.minutes && currentTime.seconds >= startTime.seconds)))) &&
                (currentTime.hour < endTime.hour || (currentTime.hour == endTime.hour &&
                (currentTime.minutes < endTime.minutes || (currentTime.minutes == endTime.minutes && currentTime.seconds < endTime.seconds))));
        } else if (startTime.hour > endTime.hour) {
            return !(currentTime.hour > endTime.hour || (currentTime.hour == endTime.hour &&
                    (currentTime.minutes > endTime.minutes || (currentTime.minutes == endTime.minutes && currentTime.seconds >= endTime.seconds))) ||
                !(currentTime.hour < startTime.hour || (currentTime.hour == startTime.hour &&
                    (currentTime.minutes < startTime.minutes || (currentTime.minutes == startTime.minutes && currentTime.seconds < startTime.seconds)))));
        } else {
            return currentTime.minutes > startTime.minutes || (currentTime.minutes == startTime.minutes &&
                    currentTime.seconds >= startTime.seconds) &&
                currentTime.minutes < endTime.minutes || (currentTime.minutes == endTime.minutes &&
                    currentTime.seconds < endTime.seconds);
        }
    }

};

#endif // INCLUDE_LIGHTCONTROLLER_HPP_