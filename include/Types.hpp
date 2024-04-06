#ifndef TYPES_HPP
#define TYPES_HPP

#include <stdint.h>

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
    uint8_t type;
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