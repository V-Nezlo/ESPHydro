/*!
@file
@brief Файл линейного планировщика - просто вызывает process у всех своих детей
@author V-Nezlo (vlladimirka@gmail.com)
@date 16.05.2024
@version 1.0
*/

#ifndef INCLUDE_LINEARSCHED_HPP_
#define INCLUDE_LINEARSCHED_HPP_

#include <chrono>

class AbstractLinearTask {
public:
	virtual void process(std::chrono::milliseconds aCurrentTime) = 0;
};

#endif // INCLUDE_LINEARSCHED_HPP_