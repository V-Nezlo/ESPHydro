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

void displayThreadFunc()
{
	lv_timer_handler(); /* let the GUI do its work */
	lv_tick_inc(5);
	std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

void rtcThreadFunc(std::reference_wrapper<DS3231> aRtc)
{
	auto result = aRtc.get().getCurrentTime();
	if (result.second) {
		Event ev;
		ev.type = EventType::GetCurrentTime;
		ev.data.time.currentHour = result.first.currentHour;
		ev.data.time.currentMinutes = result.first.currentMinutes;
		ev.data.time.currentSeconds = result.first.currentSeconds;
		EventBus::throwEvent(&ev);
	}

	std::this_thread::sleep_for(std::chrono::seconds(1));
}

void smartBusThreadFunc(std::reference_wrapper<HydroRS<SerialWrapper, Crc8, 64>> aSmartBusRef, std::reference_wrapper<SerialWrapper> aSerialRef)
{
	const size_t len = aSerialRef.get().bytesAvaillable();
	if (len) {
		uint8_t buffer[64];
		aSerialRef.get().read(buffer, len);
		aSmartBusRef.get().update(buffer, len);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

extern "C"
void app_main()
{
	// Display and UI initializing
	GT911Initializer::init();
	setupDisplay();
	lvlgSetup();
	uiInit(true);
	
	DS3231 rtc(1, 20, 21);
	SerialWrapper serial(0, 64, 64, 15, 14);
	Gpio rsLatch(10, GPIO_MODE_OUTPUT);
	HydroRS<SerialWrapper, Crc8, 64> smartBus(serial, 0, rsLatch);
	UiEventObserver uiObserver;
	ConfigStorage::instance();

	EventBus::registerObserver(&rtc);
	EventBus::registerObserver(&smartBus);
	EventBus::registerObserver(&uiObserver);

	// Поток для работы с дисплеем
	std::thread displayTask(displayThreadFunc);
	displayTask.detach();

	// Поток для работы с RTC
	std::thread rtcTask(rtcThreadFunc, std::ref(rtc));
	rtcTask.detach();
	// Поток для RS485
	std::thread smartBusTask(smartBusThreadFunc, std::ref(smartBus), std::ref(serial));
	smartBusTask.detach();

	while(true) {
		vTaskDelay(pdTICKS_TO_MS(100));
	}
}
