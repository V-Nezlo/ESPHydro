/*!
@file
@brief Типы для работы UtilitaryRS
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#ifndef INCLUDE_HYDRORSTYPES_HPP_
#define INCLUDE_HYDRORSTYPES_HPP_

enum class Commands {
    SetPumpState = 0,
    SetLampState = 1,
};

enum class Requests {
    RequestTankData = 0,
    RequestUpperData = 0
};

#endif