#ifndef EVENTBUS_HPP
#define EVENTBUS_HPP

#include <cctype>
#include <cstddef>
#include <functional>
#include <iostream>
#include <bitset>
#include <math.h>
#include <limits>
#include <string>
#include <variant>
#include <chrono>
#include <vector>

using namespace std;

enum class EventType {
    SetCurrentTime,
    UpdateSensorData,
    ErrorCaused,
    ActionRequest
};

enum class EventResult {
    HANDLED,
    IGNORED,
    PASS_ON
};

struct SensorData {
    float temp;
    float ph;
    float ppm;
};

enum class ErrorType {
    Leak,
    ParamError,
    PumpLost,
    LowWater
};

enum class Action {
    TurnPumpOn,
    TurnPumpOff,
    TurnLampOn,
    TurnLampOff
};

struct Event{
    EventType type;
    union {
        CurrentTime time;
        SensorData sensorData;
        ErrorType errorType;
        Action action;
    } data;
};

class AbstractEventObserver {
public:
    virtual EventResult handleEvent(Event *e) = 0;
};

class EventBus {
public:
    static void throwEvent(Event *aEvent)
    {
        for (auto &pos : observers) {
            auto result = pos->handleEvent(aEvent);
            if (result == EventResult::HANDLED) {
                break;
            }
            // If result == PASS_ON or IGNORED - continue
        }
    }

    void registerObserver(AbstractEventObserver *aObserver)
    {
        observers.push_back(aObserver);
    }

private:
    static std::vector<AbstractEventObserver *> observers;
};

class TimeHandler : public AbstractEventObserver {
public:
    EventResult handleEvent(Event *e) override
    {
        switch (e->type) {
            case EventType::SetCurrentTime:
                setCurrentTime(e->data.time);
                return EventResult::HANDLED;
                break;
            default:
                return EventResult::IGNORED;
                break;
        }
    }

private:
    void setCurrentTime(CurrentTime time)
    {

    }
};

#endif
