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

namespace Prohibited {
	static constexpr int8_t kProhibited1 = 6;
	static constexpr int8_t kProhibited2 = 7;
	static constexpr int8_t kProhibited3 = 8;
	static constexpr int8_t kProhibited4 = 9;
	static constexpr int8_t kProhibited5 = 10;
	static constexpr int8_t kProhibited6 = 11;
	static constexpr int8_t kProhibited7 = 1;
	static constexpr int8_t kProhibited8 = 3;
} // namespace Prohibited

namespace OnlyInput {
	static constexpr int8_t kOnlyInput1 = 35;
	static constexpr int8_t kOnlyInput2 = 34;
	static constexpr int8_t kOnlyInput3 = 36;
	static constexpr int8_t kOnlyInput4 = 39;
} // namespace OnlyInput

namespace Display {
	static constexpr int8_t kClkPin = 18;
	static constexpr int8_t kMosiPin = 19;
	static constexpr int8_t kMisoPin = 17;
	static constexpr int8_t kDcPin = 21;
	static constexpr int8_t kCsPin = 23;
	static constexpr int8_t kRstPin = 22;
	static constexpr int8_t kBusyPin = -1;

	static constexpr int8_t kBacklightPin = 5;
	static constexpr int8_t kBacklightPwmChannel = 7;
} // namespace Display

namespace Touch {
	static constexpr int8_t kI2CPort = 0;
	static constexpr int8_t kI2CAddr = 0x5D;
	static constexpr int8_t kSdaPin = 2;
	static constexpr int8_t kSclPin = 16;
	static constexpr int8_t kRstPin = 4;
	static constexpr int8_t kIntPin = 15;
} // namespace Touch

namespace RTCI2C {
	static constexpr int8_t kI2CPort = 1;
	static constexpr int8_t kSdaPin = 14;
	static constexpr int8_t kSclPin = 13;
} // namespace RTCI2C

namespace Buzzer {
	static constexpr int8_t kPwmPin = 12;
	static constexpr int8_t kPwmChannel = 1;
} // namespace Buzzer

namespace SerialRS {
	static constexpr int8_t kLatchPin = 26;
	static constexpr int8_t kUsartPort = 1;
	static constexpr int8_t aRxPin = 27;
	static constexpr int8_t aTxPin = 25;
	static constexpr int8_t kRecoveryPin = 0;
} // namespace SerialRS

namespace I2CLeds {
	static constexpr int8_t kGreenPin = 3;
	static constexpr int8_t kBluePin = 1;
	static constexpr int8_t kRedPin = 2;
} // namespace Leds

} // namespace Hardware

#endif // INCLUDE_HARDWARECONFIG_HPP_