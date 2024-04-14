#ifndef PUMPCONTROLLER_HPP_
#define PUMPCONTROLLER_HPP_

#include <EventBus.hpp>
#include <Types.hpp>
#include <chrono>
#include "TimeWrapper.hpp"

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

    }

void PumpController::process()
{
    auto currentTime = TimeWrapper::milliseconds();

    switch (pumpState) {
        case PumpState::PUMPON:
        // Если насос сейчас включен - смотрим, не пора ли выключать
            if (currentTime > lastActionTime + ConfigStorage::instance()->config.pumpOnTime) {
                lastActionTime = currentTime;

                setPumpState(false);
            }
            break;
        case PumpState::PUMPOFF:
            if (currentTime > lastActionTime + ConfigStorage::instance()->config.pumpOffTime) {
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

    void setPumpState(bool aState)
    {
        Event ev;
        ev.type == EventType::ActionRequest;
        ev.data.action = aState ? Action::TurnPumpOn : Action::TurnPumpOff;
        EventBus::throwEvent(&ev);
    }
};

#endif
