/*!
@file
@brief Шина событий-данных
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#ifndef INCLUDE_EVENTBUS_HPP_
#define INCLUDE_EVENTBUS_HPP_

#include "HydroRSTypes.hpp"
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

enum class EventType : uint8_t {
    SetCurrentTime,
    GetCurrentTime,
    UpdateUpperData,
    UpdateLowerData,
    UpdateSystemData,
    SetError,
    ClearError,
    ActionRequest,
    SettingsUpdated,
    SettingsFirstLoad,
    NewBrightness,
    BuzzerSignal,
    RsDeviceDetached,
    RsDeviceAttached
};

enum class EventResult : uint8_t {
    HANDLED,
    IGNORED,
    PASS_ON
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
        Time time;
        UpperInternalData upperData;
        LowerInternalData lowerData;
        SystemData systemData;
        SystemErrors error;
        Action action;
        Settings settings;
        uint8_t brightness;
        BuzzerSignal buzSignal;
        DeviceType device;
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
