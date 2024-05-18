/*!
@file
@brief Список используемых пинов для контроллера
@author V-Nezlo (vlladimirka@gmail.com)
@date 17.05.2024
@version 1.0
*/

#ifndef INCLUDE_HARDWARECONFIG_HPP_
#define INCLUDE_HARDWARECONFIG_HPP_

#include <cstdint>
#include <chrono>

namespace Hardware {

namespace Display {
	static constexpr int8_t kClkPin = 17;
	static constexpr int8_t kMosiPin = 16;
	static constexpr int8_t kMisoPin = 18;
	static constexpr int8_t kDcPin = 4;
	static constexpr int8_t kCsPin = 15;
	static constexpr int8_t kRstPin = 2;
	static constexpr int8_t kBusyPin = -1;

	static constexpr int8_t kBacklightPin = 5;
	static constexpr int8_t kBacklightPwmChannel = 7;
} // namespace Display

namespace Touch {
	static constexpr int8_t kI2CPort = 0;
	static constexpr int8_t kI2CAddr = 0x5D;
	static constexpr int8_t kSdaPin = 22;
	static constexpr int8_t kSclPin = 19;
	static constexpr int8_t kRstPin = 21;
	static constexpr int8_t kIntPin = 23;
} // namespace Touch

namespace RTCI2C {
	static constexpr int8_t kI2CPort = 1;
	static constexpr int8_t kSdaPin = 14;
	static constexpr int8_t kSclPin = 13;
} // namespace RTCI2C

namespace Buzzer {
	static constexpr int8_t kPwmPin = 27;
	static constexpr int8_t kPwmChannel = 1;
} // namespace Buzzer

namespace SerialRS {
	static constexpr int8_t kLatchPin = 25;
	static constexpr int8_t kUsartPort = 0;
	static constexpr int8_t aRxPin = 26;
	static constexpr int8_t aTxPin = 27;
} // namespace SerialRS

} // namespace Hardware

#endif // INCLUDE_HARDWARECONFIG_HPP_