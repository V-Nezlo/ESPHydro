
#ifndef INCLUDE_OPTIONS_HPP_
#define INCLUDE_OPTIONS_HPP_

#include <cstdint>
#include <chrono>

namespace Options {

static constexpr std::chrono::seconds kMaxTimeForFullFlooding{30}; ///< Время, за которое рабочий резервуар должен успеть полностью заполниться

}

#endif