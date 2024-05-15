#ifndef INCLUDE_SYSTEMINTERGRATOR_HPP_
#define INCLUDE_SYSTEMINTERGRATOR_HPP_

#include "EventBus.hpp"
#include <chrono>

class SystemIntegrator : AbstractEventObserver {
public:
    SystemIntegrator() : systemFlagStorage{0}, lastCheckTime{0}, updated{false}
    {

    }

    void process(std::chrono::milliseconds aCurrentTime)
    {
        if (aCurrentTime > lastCheckTime + std::chrono::milliseconds{1000} && updated) {
            lastCheckTime = aCurrentTime;
            updated = false;

            // Составим health из флагов
            DeviceState state = DeviceState::DeviceWorking;
            if ((systemFlagStorage & SystemErrors::SystemLeak) || (systemFlagStorage & SystemErrors::SystemRSBusError)) {
                state = DeviceState::DeviceCritical;
            } else if ((systemFlagStorage & SystemErrors::SystemRTCError) || (systemFlagStorage & SystemErrors::SystemInternalMemError)) {
                state = DeviceState::DeviceError;
            } else if (systemFlagStorage & SystemErrors::SystemTankNotFloodedInTime) {
                state = DeviceState::DeviceWarning;
            }

            Event ev;
            ev.type = EventType::UpdateSystemData;
            ev.data.systemData.flags = systemFlagStorage;
            ev.data.systemData.health = state;
            EventBus::throwEvent(&ev);
        }
    }

    EventResult handleEvent(Event *e) override 
    {
        switch(e->type) {
            case EventType::SetError: 
                systemFlagStorage |= e->data.error;
                updated = true;
                return EventResult::PASS_ON;

            case EventType::ClearError:
                systemFlagStorage &= ~e->data.error;
                updated = true;

            default:
                return EventResult::IGNORED;
        }
    }
private:
    uint8_t systemFlagStorage;
    std::chrono::milliseconds lastCheckTime;
    bool updated;
};

#endif // INCLUDE_SYSTEMINTERGRATOR_HPP_