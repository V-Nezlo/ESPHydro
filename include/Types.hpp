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

enum LowerFlags : uint8_t {
	LowerPumpOverCurrentFlag = 0x01,
	LowerNoWaterFlag = 0x02,
	LowerTempSensorErrorFlag = 0x04,
	LowerPHSensorErrorFlag = 0x08,
	LowerPPMSensorErrorFlag = 0x10,
	LowerPumpLowCurrentFlag = 0x20
};

enum UpperFlags : uint8_t {
	UpperTopWaterLevelStuck = 0x01,
	UpperPowerError = 0x02
};

enum SystemErrors : uint8_t {
	SystemRTCError = 0x01,
	SystemInternalMemError = 0x02,
	SystemRSBusError = 0x04,
	SystemTankNotFloodedInTime = 0x08,
	SystemLeak = 0x10,
	SystemPumpNotOperate = 0x20
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
	int8_t waterTemp10;
	uint8_t ph10;
	uint16_t ppm;
};

struct SystemData {
	uint8_t flags;
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

struct Time {
	uint8_t hour;
	uint8_t minutes;
	uint8_t seconds;
};

#endif // INCLUDE_TYPES_HPP_