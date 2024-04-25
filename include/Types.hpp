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

struct SystemData {
    uint32_t deviceFlags;
    uint16_t ppm;
    uint8_t phd10;
    uint8_t waterLevelProcents;
    int16_t waterTempd10;
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