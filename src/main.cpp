#include "BuzzerController.hpp"
#include "ConfigStorage.hpp"
#include "DS3231.hpp"
#include "Display.hpp"
#include "GpioWrapper.hpp"
#include "HydroRS.hpp"
#include "LightController.hpp"
#include "PumpController.hpp"
#include "SerialWrapper.hpp"
#include "SystemIntergrator.hpp"
#include "UI.hpp"

#include <Lib/Crc8.hpp>
#include <LovyanGFX.hpp>
#include <lvgl.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_pthread.h>

#include <functional>
#include <thread>

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
		lv_timer_handler();
		lv_tick_inc(5);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

extern "C"
void app_main()
{
	ConfigStorage paramStorage;
	DisplayDriver displayDriver;
	UiEventObserver uiObserver;

	Gpio rsLatch(Hardware::SerialRS::kLatchPin, GPIO_MODE_OUTPUT);
	SerialWrapper serial(Hardware::SerialRS::kUsartPort, 148, 148, Hardware::SerialRS::aTxPin, Hardware::SerialRS::aRxPin);
	HydroRS<SerialWrapper, Crc8, 64> smartBus(serial, 0, rsLatch);

	DS3231 rtc(Hardware::RTCI2C::kI2CPort, Hardware::RTCI2C::kSdaPin, Hardware::RTCI2C::kSclPin);

	PumpController pumpController;
	LightController lightController;
	SystemIntegrator systemIntegrator;
	BuzzerController buzzController{Hardware::Buzzer::kPwmPin, Hardware::Buzzer::kPwmChannel};

	EventBus::registerObserver(&paramStorage);
	EventBus::registerObserver(&displayDriver);
	EventBus::registerObserver(&uiObserver);
	EventBus::registerObserver(&smartBus);
	EventBus::registerObserver(&rtc);
	EventBus::registerObserver(&pumpController);
	EventBus::registerObserver(&lightController);
	EventBus::registerObserver(&systemIntegrator);
	EventBus::registerObserver(&buzzController);

	// Включаем и отрисовываем экран
	displayDriver.setupDisplay();
	displayDriver.setupLvgl();
	uiInit(true);

	// Загружаем параметры во все модули
	paramStorage.firstLoad();

	// Поток для работы с дисплеем, увеличенный стек, припиненно к ядру
	auto cfg = esp_pthread_get_default_config();
	cfg = updateThreadConfig("Display", 1, 5 * 1024, 5);
	esp_pthread_set_cfg(&cfg);
	std::thread displayTask(displayThreadFunc);
	displayTask.detach();

	while(true) {
		// Обработка SmartBus
		const size_t len = serial.bytesAvaillable();
		if (len) {
			uint8_t buffer[64];
			serial.read(buffer, len);
			smartBus.update(buffer, len);
		}

		const std::chrono::milliseconds currentTime = TimeWrapper::milliseconds();
		// Прочие обработчики
		rtc.process(currentTime);
		pumpController.process(currentTime);
		lightController.process(currentTime);
		systemIntegrator.process(currentTime);
		buzzController.process(currentTime);

		lgfx::delay(50);
	}
}
