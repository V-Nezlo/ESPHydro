/*!
@file
@brief ООП обертка для драйвера RTC DS3231 
@author V-Nezlo (vlladimirka@gmail.com)
@date 18.04.2024
@version 1.0
*/

#include <EventBus.hpp>
#include <ds3231/ds3231.h>
#include <Types.hpp>
#include <esp_log.h>

#include <string.h>
#include <stdio.h>
#include <utility>

#ifndef INCLUDE_DS3231_HPP_
#define INCLUDE_DS3231_HPP_

class DS3231 : public AbstractEventObserver {
public:
    DS3231(uint8_t aPort, uint8_t aSdaPin, uint8_t aSclPin)
    {
        // Уберем мусор из структуры без конструктора
        memset(&dev, 0, sizeof(i2c_dev_t));
        const auto result = ds3231_init_desc(&dev, static_cast<i2c_port_t>(aPort), static_cast<gpio_num_t>(aSdaPin), 
            static_cast<gpio_num_t>(aSclPin));

        if (result != ESP_OK) {
            ESP_LOGE("RTC", "DS3231 init failed, %i", static_cast<int>(result));
        }

        if (!isConfigured()) {
            initialConfigure();
        }
    }

    bool setCurrentTime(CurrentTime aTime)
    {
        tm time;
        ds3231_get_time(&dev, &time);

        // Забираем все данные, обновляем только часы минуты секунды
        time.tm_hour = aTime.currentHour;
        time.tm_min = aTime.currentMinutes;
        time.tm_sec = aTime.currentSeconds;
        
        const auto result = ds3231_set_time(&dev, &time);

        if (result == ESP_OK) {
            return true;
        } else {
            return false;
        }
    }

    std::pair<CurrentTime, bool> getCurrentTime()
    {
        tm time;
        CurrentTime newTime{0,0,0};
        bool valid{false};

        const auto result = ds3231_get_time(&dev, &time);

        if (result == ESP_OK) {
            newTime.currentHour = time.tm_hour;
            newTime.currentMinutes = time.tm_min;
            newTime.currentSeconds = time.tm_sec;
            valid = true;
        } 

        return std::make_pair(newTime, valid);
    }

    EventResult handleEvent(Event *e) override
    {
        switch(e->type) {
            case EventType::SetCurrentTime:
                setCurrentTime(e->data.time);
                return EventResult::HANDLED;
                break;
            default:
                return EventResult::IGNORED;
                break;
        }
    }

private:
    i2c_dev_t dev;

    /// @brief Проверим настройку времени по установленному году
    bool isConfigured()
    {
        tm time;
        const auto result = ds3231_get_time(&dev, &time);

        if (time.tm_year >= atoi(AUTO_YEAR) + 1900) {
            return true;
        } else {
            return false;
        }
    }

    bool initialConfigure()
    {
        tm time;
        time.tm_year = atoi(AUTO_YEAR) - 1900;
        time.tm_mon = atoi(AUTO_MONTH) - 1;
        time.tm_mday = atoi(AUTO_MDAY);
        time.tm_wday = atoi(AUTO_WDAY) - 1;

        time.tm_hour = atoi(AUTO_HOUR);
        time.tm_min = atoi(AUTO_MIN);
        time.tm_sec = atoi(AUTO_YEAR);

        const auto result = ds3231_set_time(&dev, &time);

        if (result == ESP_OK) {
            return true;
        } else {
            return false;
        }
    }
};

#endif