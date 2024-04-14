#ifndef TIMERWRAPPER
#define TIMERWRAPPER

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_freertos_hooks.h"
#include "driver/timer.h"
#include "freertos/timers.h"
#include <chrono>
#include <functional>

class TimeWrapper {
public:
    static std::chrono::microseconds microseconds()
    {
        return std::chrono::microseconds(esp_timer_get_time());
    }

    static std::chrono::milliseconds milliseconds()
    {
        return std::chrono::milliseconds(esp_timer_get_time() / 1000);
    }

    static std::chrono::seconds seconds()
    {
        return std::chrono::seconds(esp_timer_get_time() / 1000000);
    }

};

#endif