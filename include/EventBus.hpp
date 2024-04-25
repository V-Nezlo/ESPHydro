/*!
@file
@brief Шина событий-данных
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#ifndef INCLUDE_EVENTBUS_HPP_
#define INCLUDE_EVENTBUS_HPP_

#include "Types.hpp"

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
    GetCurrentTime,
    UpdateUpperData,
    UpdateLowerData,
    ErrorCaused,
    ActionRequest,
    SettingsUpdated,
    SettingsFirstLoad,
    NewBrightness,
    BuzzerSignal
};

enum class EventResult {
    HANDLED,
    IGNORED,
    PASS_ON
};

struct UpperData {
    float temp;
    float ph;
    float ppm;
    bool upperState;
};

struct LowerData {
    uint8_t waterLevel;
    bool pumpState;
};

enum class ErrorType {
    UpperNotFloodedInTime,
    Leak,
    ParamError,
    PumpLost,
    LowWater
};

enum class Action : uint8_t {
    TurnPumpOn,
    TurnPumpOff,
    TurnLampOn,
    TurnLampOff
};

enum class BuzzerSignal {
    Disable,
    Short,
    Long
};

struct Event{
    EventType type;
    union {
        CurrentTime time;
        UpperData sensorData;
        ErrorType errorType;
        Action action;
        Settings settings;
        uint8_t brightness;
        BuzzerSignal buzSignal;

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
            // If result == PASS_ON or IGNORED - continue
            if (result == EventResult::HANDLED) {
                break;
            }
        }
    }

    static void registerObserver(AbstractEventObserver *aObserver)
    {
        observers.push_back(aObserver);
    }

private:
    static std::vector<AbstractEventObserver *> observers;
};

#endif
