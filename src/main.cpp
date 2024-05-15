#include <LovyanGFX.hpp>
#include <lvgl.h>
#include "GpioWrapper.hpp"
#include "esp_log.h"
#include "Display.hpp"
#include "GT911Initializer.hpp"
#include "UI.hpp"
#include "HydroRS.hpp"
#include <thread>
#include <DS3231.hpp>
#include <functional>
#include <SerialWrapper.hpp>
#include <Lib/Crc8.hpp>
#include <ConfigStorage.hpp>
#include <PumpController.hpp>
#include <esp_pthread.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

esp_pthread_cfg_t updateThreadConfig(const char *name, int core_id, int stack, int prio)
{
    auto cfg = esp_pthread_get_default_config();
    cfg.thread_name = name;
    cfg.pin_to_core = core_id;
    cfg.stack_size = stack;
    cfg.prio = prio;
    return cfg;
}

void displayThreadFunc()
{
	while(true) {
		lv_timer_handler(); /* let the GUI do its work */
		lv_tick_inc(5);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void rtcThreadFunc(std::reference_wrapper<DS3231> aRtc)
{
	while(true) {
		auto result = aRtc.get().getCurrentTime();
		if (result.second) {
			Event ev;
			ev.type = EventType::GetCurrentTime;
			ev.data.time.hour = result.first.hour;
			ev.data.time.minutes = result.first.minutes;
			ev.data.time.seconds = result.first.seconds;
			EventBus::throwEvent(&ev);
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void smartBusThreadFunc(std::reference_wrapper<HydroRS<SerialWrapper, Crc8, 64>> aSmartBusRef, std::reference_wrapper<SerialWrapper> aSerialRef)
{
	while(true) {
		const size_t len = aSerialRef.get().bytesAvaillable();
		if (len) {
			uint8_t buffer[64];
			aSerialRef.get().read(buffer, len);
			aSmartBusRef.get().update(buffer, len);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

extern "C"
void app_main()
{
	// Display and UI initializing
	GT911Initializer::init();
	DisplayDriver displayDriver;
	displayDriver.setupDisplay();
	displayDriver.setupLvgl();
	
	uiInit(true);
	
	DS3231 rtc(1, 20, 21);
	SerialWrapper serial(0, 64, 64, 15, 14);
	Gpio rsLatch(10, GPIO_MODE_OUTPUT);
	HydroRS<SerialWrapper, Crc8, 64> smartBus(serial, 0, rsLatch);
	UiEventObserver uiObserver;
	PumpController pumpController;

	ConfigStorage::instance();

	//EventBus::registerObserver(&rtc);
	//EventBus::registerObserver(&smartBus);
	EventBus::registerObserver(&uiObserver);
	EventBus::registerObserver(ConfigStorage::instance());
	EventBus::registerObserver(&displayDriver);
	ConfigStorage::instance()->firstLoad();

	auto cfg = esp_pthread_get_default_config();
	
	// Поток для работы с дисплеем, увеличенный стек
	cfg = updateThreadConfig("Display", 0, 5 * 1024, 5);
	esp_pthread_set_cfg(&cfg);
	std::thread displayTask(displayThreadFunc);
	displayTask.detach();

	// Поток для работы с RTC
	// cfg = updateThreadConfig("RTC", 0, 1 * 1024, 3);
	// esp_pthread_set_cfg(&cfg);
	// std::thread rtcTask(rtcThreadFunc, std::ref(rtc));
	// rtcTask.detach();

	// Поток для RS485
	// cfg = updateThreadConfig("SmartBus", 0, 2 * 1024, 3);
	// esp_pthread_set_cfg(&cfg);
	// std::thread smartBusTask(smartBusThreadFunc, std::ref(smartBus), std::ref(serial));
	// smartBusTask.detach();

	while(true) {
		lgfx::delay(500);
	}
}
