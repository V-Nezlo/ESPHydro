/*!
@file
@brief Компиляция типов
@author V-Nezlo (vlladimirka@gmail.com)
@date 17.04.2024
@version 1.0
*/

#ifndef INCLUDE_TYPES_HPP_
#define INCLUDE_TYPES_HPP_

#include <stdint.h>

enum class PumpModes : uint8_t {
    EBBNormal = 0,
    EBBSwing,
    Maintance,
    Dripping
};

enum class DeviceFlags : uint8_t {
	DeviceWorking,
	DeviceWarning,
	DeviceError,
	DeviceCritical,
	DeviceDisabled
};

struct UpperInternalData {
    DeviceFlags flags;
    bool lampState;
    bool swingLevelState;
    bool damState;
};

struct LowerInternalData {
    DeviceFlags flags;
    uint8_t waterLevel;
    bool pumpState;
    uint8_t waterTemp10;
    uint8_t ph10;
    uint16_t ppm;
};

struct SystemData {
    DeviceFlags flags;
};

struct AuxData {
    DeviceFlags flags;
};

struct PumpConfiguration {
    bool enabled;
    uint16_t onTime;
    uint16_t offTime;
    PumpModes mode;
    uint8_t swingTime;
};

struct LampConfiguration {
    bool enabled;
    uint8_t lampOnHour;
    uint8_t lampOnMin;
    uint8_t lampOffHour;
    uint8_t lampOffMin;
};

struct CommonConfiguration {
    bool tapSoundEnabled;
    bool alarmSoundEnabled;
    bool loggingEnabled;
    uint8_t displayBrightness;
};

struct Settings {
    struct PumpConfiguration pump;
    struct LampConfiguration lamp;
    struct CommonConfiguration common; 
};

struct CurrentTime {
    uint8_t currentHour;
    uint8_t currentMinutes;
    uint8_t currentSeconds;
};

#endif