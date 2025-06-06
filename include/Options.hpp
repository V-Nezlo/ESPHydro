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

static constexpr std::chrono::seconds kMaxTimeForFullFlooding{30}; ///< Время, за которое рабочий резервуар должен успеть полностью заполниться
static constexpr uint8_t kMinWaterLevelForWork{20}; ///< Минимальный уровень воды для работы

static constexpr std::chrono::milliseconds kTimeBetweenSignals{500}; ///< Время между сигналами
static constexpr std::chrono::milliseconds kTimeBetweenTaps{250}; 

static constexpr std::chrono::seconds kMasterErrorSignalTimeout{30};
static constexpr std::chrono::seconds kMasterCriticalSignalTimeout{10};
static constexpr std::chrono::seconds kMasterWarningSignalTimeout{60};

}

#endif // INCLUDE_OPTIONS_HPP_