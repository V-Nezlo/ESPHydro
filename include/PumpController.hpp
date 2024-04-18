/*!
@file
@brief Обработчик насоса
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#ifndef INCLUDE_PUMPCONTROLLER_HPP_
#define INCLUDE_PUMPCONTROLLER_HPP_

#include <EventBus.hpp>
#include <Types.hpp>
#include <ConfigStorage.hpp>
#include <TimeWrapper.hpp>
#include <chrono>

class PumpController : public AbstractEventObserver {
    enum class Modes {
        Normal,
        Swing,
        Drip
    };

    enum class PumpState{
        PUMPON,
        PUMPOFF
    };

public:
    PumpController()
    {

    }

    EventResult handleEvent(Event *e) override
    {
        switch(e->type) {
            case EventType::SettingsUpdated:
                pumpOnTime = std::chrono::seconds(e->data.settings.pump.onTime);
                pumpOffTime = std::chrono::seconds(e->data.settings.pump.offTime);
                return EventResult::PASS_ON;
            default:
                return EventResult::IGNORED;
        }
    }

void PumpController::process()
{
    auto currentTime = TimeWrapper::milliseconds();

    switch (pumpState) {
        case PumpState::PUMPON:
        // Если насос сейчас включен - смотрим, не пора ли выключать
            if (currentTime > lastActionTime + pumpOnTime) {
                lastActionTime = currentTime;

                setPumpState(false);
            }
            break;
        case PumpState::PUMPOFF:
            if (currentTime > lastActionTime + pumpOffTime) {
                lastActionTime = currentTime;

                setPumpState(true);
            }
            break;
    }
}

private:
    Modes mode;
    PumpState pumpState;
    std::chrono::milliseconds lastActionTime;

    std::chrono::seconds pumpOnTime;
    std::chrono::seconds pumpOffTime;

    void setPumpState(bool aState)
    {
        Event ev;
        ev.type == EventType::ActionRequest;
        ev.data.action = aState ? Action::TurnPumpOn : Action::TurnPumpOff;
        EventBus::throwEvent(&ev);
    }
};

#endif
