/*!
@file
@brief RAII-механизм для блокировки мьютекса
@author V-Nezlo (vlladimirka@gmail.com)
@date 26.05.2025
@version 1.0
*/

#ifndef INCLUDE_MUTEXLOCK_HPP_
#define INCLUDE_MUTEXLOCK_HPP_

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class MutexLock {
	SemaphoreHandle_t &mutex;
public:
	MutexLock(SemaphoreHandle_t &aMutex): 
	mutex{aMutex} 
	{
		xSemaphoreTake(mutex, portMAX_DELAY);
	}

	MutexLock() = delete;
	MutexLock(const MutexLock&) = delete;

	virtual ~MutexLock() 
	{
		xSemaphoreGive(mutex);
	}
};

#endif // INCLUDE_MUTEXLOCK_HPP_