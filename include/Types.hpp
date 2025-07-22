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
	Dripping,
	EBBDam
};

enum class DeviceHealth : uint8_t {
	DeviceWorking,
	DeviceWarning,
	DeviceError,
	DeviceCritical,
	DeviceDisabled
};

struct HealthRule {
    DeviceHealth health;
    uint32_t mask;
};

struct UpperInternalData {
	uint8_t flags;
	bool lampState;
	bool swingLevelState;
	bool damState;
};

struct LowerInternalData {
	uint8_t flags;
	uint8_t waterLevel;
	bool pumpState;
	int16_t waterTemp10;
	uint8_t ph10;
	uint16_t ppm;
};

struct AuxData {
	uint8_t flags;
};

struct PumpConfiguration {
	bool enabled;
	uint16_t onTime;
	uint16_t offTime;
	PumpModes mode;
	uint8_t swingTime;
	uint16_t maxFloodingTime;
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
	uint8_t buzzerVolume;
};

struct Modules {
	bool phSensor;
	bool ppmSensor;
};

struct Settings {
	struct PumpConfiguration pump;
	struct LampConfiguration lamp;
	struct CommonConfiguration common; 
	struct Modules modules;
};

struct Time {
	uint8_t hour;
	uint8_t minutes;
	uint8_t seconds;
};

struct UpdateHealth {
	DeviceType type;
	DeviceHealth health;
};

#endif // INCLUDE_TYPES_HPP_
