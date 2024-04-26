//-----------------------------------------------------------------------
//  File        : PumpHandler.hpp
//  Created     : 3.12.2022
//  Modified    : 12.10.2023
//  Author      : V-Nezlo (vlladimirka@gmail.com)
//  Description : Обработчик лампы или эквивалентного устройства

#ifndef LIGHTHANDLER_HPP
#define LIGHTHANDLER_HPP

#include "EventBus.hpp"
#include "ConfigStorage.hpp"
#include "GpioWrapper.hpp"
#include "TimeWrapper.hpp"

class LightHandler : public AbstractEventObserver {
public:
    LightHandler() :
        enabled{false},
        lampOnTime{0,0,0},
        lampOffTime{0,0,0}
    {

    }

    EventResult handleEvent(Event *e) override
    {
        switch (e->type) {
            case EventType::GetCurrentTime:
                process(e->data.time);
                return EventResult::PASS_ON;
                break;
            case EventType::SettingsFirstLoad:
                // Fallthrough
            case EventType::SettingsUpdated:
                enabled = e->data.settings.lamp.enabled;
                lampOnTime.currentHour = e->data.settings.lamp.lampOnHour;
                lampOnTime.currentMinutes = e->data.settings.lamp.lampOnMin;
                lampOffTime.currentHour = e->data.settings.lamp.lampOffHour;
                lampOffTime.currentMinutes = e->data.settings.lamp.lampOffMin;
                return EventResult::PASS_ON;
                break;
            default:
                return EventResult::IGNORED;
        }
    }

    void process(CurrentTime aTime)
    {
        
    }

private:
    bool enabled;
    CurrentTime lampOnTime;
    CurrentTime lampOffTime;
};

#endif