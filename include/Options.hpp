/*!
@file
@brief Некоторые настройки
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#ifndef INCLUDE_OPTIONS_HPP_
#define INCLUDE_OPTIONS_HPP_

#include <cstdint>
#include <chrono>

namespace Options {

static constexpr std::chrono::milliseconds kShortBeepDuraction{100};
static constexpr std::chrono::milliseconds kLongBeepDuraction{500};

static constexpr uint8_t kMinWaterLevelForWork{10}; ///< Минимальный уровень воды для работы
static constexpr std::chrono::milliseconds kLedRecoverTime{1000}; ///< Время восстановления светодиода

static constexpr std::chrono::milliseconds kTimeBetweenSignals{500}; ///< Время между сигналами
static constexpr std::chrono::milliseconds kTimeBetweenTaps{250}; 

static constexpr std::chrono::seconds kMasterErrorSignalTimeout{30};
static constexpr std::chrono::seconds kMasterCriticalSignalTimeout{10};
static constexpr std::chrono::seconds kMasterWarningSignalTimeout{60};

}

#endif // INCLUDE_OPTIONS_HPP_