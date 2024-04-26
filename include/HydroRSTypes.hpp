/*!
@file
@brief Типы для работы UtilitaryRS
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#ifndef INCLUDE_HYDRORSTYPES_HPP_
#define INCLUDE_HYDRORSTYPES_HPP_

enum class DeviceType {
	Unknown = 0,
	Upper,
    Lower
};

struct LowerTelemetry {
    uint8_t pumpState;
    uint8_t waterLevelPerc;
    float waterTemperature;
    uint8_t waterPH;
    uint16_t waterPPM;
    uint8_t deviceFlags;
} __attribute__((packed));

enum class Commands {
    SetPumpState = 0,
    SetLampState
};

enum class Requests {
    RequestTelemetry = 0
};

#endif