/*!
@file
@brief Типы для работы UtilitaryRS
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#ifndef INCLUDE_HYDRORSTYPES_HPP_
#define INCLUDE_HYDRORSTYPES_HPP_

#include <cstdint>

enum class DeviceType {
	Unknown = 0,
	Upper,
    Lower
};

// Структуры для HydroRS
struct LowerTelemetry {
    uint8_t pumpState;
    uint8_t waterLevelPerc;
    float waterTemperature;
    uint8_t waterPH;
    uint16_t waterPPM;
    uint8_t deviceFlags;
} __attribute__((packed));

struct UpperTelemetry {
    uint8_t lampState;
    uint8_t swingLevelState;
    uint8_t damState;
    uint8_t deviceFlags;
} __attribute__((packed));

enum class Commands {
    SetPumpState = 1,
    SetLampState
};

enum class Requests {
    RequestTelemetry = 1
};

#endif