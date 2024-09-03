#include "BuzzerController.hpp"
#include "ConfigStorage.hpp"
#include "DeviceMonitor.hpp"
#include "DS3231.hpp"
#include "Display.hpp"
#include "GpioWrapper.hpp"
#include "HydroRS.hpp"
#include "LightController.hpp"
#include "LedController.hpp"
#include "PumpController.hpp"
#include "SerialWrapper.hpp"
#include "SystemIntergrator.hpp"
#include "UI.hpp"

#include <UtilitaryRS/Crc8.hpp>
#include <LovyanGFX.hpp>
#include <lvgl.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <esp_pthread.h>
#include <esp_task_wdt.h>

#include <functional>
#include <thread>

void displayTaskFunc(void *pvParameters)
{
	while(true) {
		static bool inited{false};

		if (!inited) {
			const auto time = TimeWrapper::seconds();
			if (time > std::chrono::seconds{1}) {
				displayMainPage();
				inited = true;
			}
		}

		lv_timer_handler();
		lv_tick_inc(5);
		esp_task_wdt_reset();
		vTaskDelay(5 / portTICK_PERIOD_MS);
	}
}

extern "C"
void app_main()
{
	ConfigStorage paramStorage;
	DisplayDriver displayDriver;
	UiEventObserver uiObserver;

	Gpio rsLatch(Hardware::SerialRS::kLatchPin, GPIO_MODE_OUTPUT);
	SerialWrapper serial(Hardware::SerialRS::kUsartPort, 148, 148, Hardware::SerialRS::aTxPin, Hardware::SerialRS::aRxPin, rsLatch);
	HydroRS<SerialWrapper, Crc8, 64> smartBus(serial, DeviceType::Master);

	DS3231 rtc(Hardware::RTCI2C::kI2CPort, Hardware::RTCI2C::kSdaPin, Hardware::RTCI2C::kSclPin);

	PumpController pumpController;
	LightController lightController;
	SystemIntegrator systemIntegrator;
	BuzzerController buzzController{Hardware::Buzzer::kPwmPin, Hardware::Buzzer::kPwmChannel};
	DeviceMonitor deviceMonitor;

	EventBus::registerObserver(&paramStorage);
	EventBus::registerObserver(&displayDriver);
	EventBus::registerObserver(&uiObserver);
	EventBus::registerObserver(&smartBus);
	EventBus::registerObserver(&rtc);
	EventBus::registerObserver(&pumpController);
	EventBus::registerObserver(&lightController);
	EventBus::registerObserver(&systemIntegrator);
	EventBus::registerObserver(&buzzController);
	EventBus::registerObserver(&deviceMonitor);

	LinearSched sched;
	sched.registerTask(&rtc);
	sched.registerTask(&pumpController);
	sched.registerTask(&lightController);
	sched.registerTask(&systemIntegrator);
	sched.registerTask(&buzzController);
	sched.registerTask(&deviceMonitor);
	sched.registerTask(&smartBus);

	// Включаем и отрисовываем экран
	displayDriver.setupDisplay();
	displayDriver.setupLvgl();
	uiInit();

	// Загружаем параметры во все модули
	paramStorage.firstLoad();

	// Поток для работы с дисплеем, увеличенный стек, припиненно к ядру
	TaskHandle_t displayTask;
	xTaskCreatePinnedToCore(displayTaskFunc, "Display", 8 * 1024, &displayDriver, 5, &displayTask, 1);
	esp_task_wdt_add(displayTask);

	while(true) {
		// Обработка SmartBus
		const size_t len = serial.bytesAvaillable();
		if (len) {
			uint8_t buffer[64];
			serial.read(buffer, len);
			smartBus.update(buffer, len);
		}

		sched.doTasks();
		lgfx::delay(50);
	}
}
