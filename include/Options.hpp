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

static constexpr std::chrono::seconds kMaxTimeForFullFlooding{30}; ///< Время, за которое рабочий резервуар должен успеть полностью заполниться

}

#endif // INCLUDE_OPTIONS_HPP_