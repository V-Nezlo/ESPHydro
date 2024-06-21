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

void displayThreadFunc(DisplayDriver *aDriver)
{
	static bool inited{false};
	const auto time = TimeWrapper::seconds();
	if (!inited && time > std::chrono::seconds{5}) {
		// Показываем начальный экран ограниченное время
		displayMainPage();
		inited = true; // Больше не читаем время
	}

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
	HydroRS<SerialWrapper, Crc8, 64> smartBus(serial, DeviceType::Master, rsLatch);

	DS3231 rtc(Hardware::RTCI2C::kI2CPort, Hardware::RTCI2C::kSdaPin, Hardware::RTCI2C::kSclPin);

	PumpController pumpController;
	LightController lightController;
	SystemIntegrator systemIntegrator;
	BuzzerController buzzController{Hardware::Buzzer::kPwmPin, Hardware::Buzzer::kPwmChannel};
	DeviceMonitor deviceMonitor;

	Gpio greenLed{Hardware::Leds::kGreenPin, GPIO_MODE_OUTPUT};
	Gpio blueLed{Hardware::Leds::kBluePin, GPIO_MODE_OUTPUT};
	Gpio redLed{Hardware::Leds::kRedPin, GPIO_MODE_OUTPUT};
	LedController ledControl(greenLed, blueLed, redLed);

	EventBus::registerObserver(&paramStorage);
	EventBus::registerObserver(&displayDriver);
	EventBus::registerObserver(&uiObserver);
	EventBus::registerObserver(&smartBus);
	EventBus::registerObserver(&rtc);
	EventBus::registerObserver(&pumpController);
	EventBus::registerObserver(&lightController);
	EventBus::registerObserver(&systemIntegrator);
	EventBus::registerObserver(&buzzController);
	EventBus::registerObserver(&ledControl);
	EventBus::registerObserver(&deviceMonitor);

	LinearSched sched;
	sched.registerTask(&rtc);
	sched.registerTask(&pumpController);
	sched.registerTask(&lightController);
	sched.registerTask(&systemIntegrator);
	sched.registerTask(&buzzController);
	sched.registerTask(&ledControl);
	sched.registerTask(&deviceMonitor);

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
	std::thread displayTask(displayThreadFunc, &displayDriver);
	displayTask.detach();

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
