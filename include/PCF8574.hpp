/*!
@file
@brief Драйвер для работы с i2c-gpio
@author V-Nezlo (vlladimirka@gmail.com)
@date 26.05.2025
@version 1.0
*/

#ifndef INCLUDE_PCF8574_HPP_
#define INCLUDE_PCF8574_HPP_

#include "Options.hpp"
#include "LinearSched.hpp"
#include <pcf8574/pcf8574.h>
#include <i2cdev/i2cdev.h>
#include <esp_log.h>
#include <string.h>
#include "MasterMonitor.hpp"
#include "EventBus.hpp"

class PCF8574 : public AbstractLinearTask {
public:
	PCF8574(uint8_t aAddr, uint8_t aPort, uint8_t aSdaPin, uint8_t aSclPin):
	updateTime{100},
	nextUpdateTime{0},
	dev{},
	present{false}
	{
		// Уберем мусор из структуры без конструктора
		memset(&dev, 0, sizeof(i2c_dev_t));
		// Функция только создает дескриптор, обращений внутри никаких нет
		ESP_ERROR_CHECK(pcf8574_init_desc(&dev, aAddr, static_cast<i2c_port_t>(aPort), static_cast<gpio_num_t>(aSdaPin),
			static_cast<gpio_num_t>(aSclPin)));

		const auto result = pcf8574_port_write(&dev, 0);
		if (result == ESP_OK) {
			present = true;
			ESP_LOGI("PCF", "PCF Initialized");
		} else {
			ESP_LOGE("PCF", "PCF init failed, %i", static_cast<int>(result));
			MasterMonitor::instance().setFlag(MasterFlags::PCFError);

			// Уничтожим дескриптор за ненужностью
			ESP_ERROR_CHECK(pcf8574_free_desc(&dev));
		}
	}
	virtual ~PCF8574() = default;

	void write(uint8_t aId, bool aState)
	{
		mask = aState ? (mask | (1 << aId)) : (mask & ~(1 << aId));
		updated = true;
	}

	void process(std::chrono::milliseconds aCurrentTime) override
	{
		if (!present) {
			return;
		}

		if (updated && aCurrentTime >= nextUpdateTime) {
			nextUpdateTime = aCurrentTime + updateTime;
			pcf8574_port_write(&dev, mask);
		}
	}

private:
	const std::chrono::milliseconds updateTime;
	std::chrono::milliseconds nextUpdateTime;

	i2c_dev_t dev;
	bool present;

	uint8_t mask;
	bool updated;
};

#endif // INCLUDE_PCF8574_HPP_
