#include "ConfigStorage.hpp"
#include "DS3231.hpp"
#include "Display.hpp"
#include "Gpio.hpp"
#include "HydroRS.hpp"
#include "I2CGpio.hpp"
#include "LightController.hpp"
#include "LedController.hpp"
#include "MutexLock.hpp"
#include "PCF8574.hpp"
#include "PumpController.hpp"
#include "SerialWrapper.hpp"
#include "ToneBuzzer.hpp"
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
	SemaphoreHandle_t *mutex = reinterpret_cast<SemaphoreHandle_t *>(pvParameters);

	while(true) {
		static bool inited{false};

		if (!inited) {
			const auto time = TimeWrapper::seconds();
			if (time > std::chrono::seconds{1}) {
				displayMainPage();
				inited = true;
			}
		}

		{
			MutexLock lock(*mutex);
			lv_timer_handler();
			lv_tick_inc(5);
		}

		esp_task_wdt_reset();
		vTaskDelay(5 / portTICK_PERIOD_MS);
	}
}

extern "C"
void app_main()
{
	EventBus::init();

	ConfigStorage paramStorage;
	DisplayDriver displayDriver;

	UiEventObserver uiObserver;
	SemaphoreHandle_t lvglMutex = xSemaphoreCreateMutex();
	uiObserver.registerLVGLMutex(&lvglMutex);

	Gpio rsLatch(Hardware::SerialRS::kLatchPin, GPIO_MODE_OUTPUT);
	SerialWrapper serial(Hardware::SerialRS::kUsartPort, 148, 148, Hardware::SerialRS::aTxPin, Hardware::SerialRS::aRxPin, rsLatch);
	HydroRS<SerialWrapper, Crc8, 64> smartBus(serial, DeviceType::Master);

	DS3231 rtc(Hardware::RTCI2C::kI2CPort, Hardware::RTCI2C::kSdaPin, Hardware::RTCI2C::kSclPin);
	PCF8574 pcf(0x20, Hardware ::RTCI2C::kI2CPort, Hardware::RTCI2C::kSdaPin, Hardware::RTCI2C::kSclPin);

	I2CGpio greenLed{pcf, Hardware::I2CLeds::kGreenPin};
	I2CGpio blueLed{pcf, Hardware::I2CLeds::kBluePin};
	I2CGpio redLed{pcf, Hardware::I2CLeds::kRedPin};

	PumpController pumpController;
	LightController lightController;
	ToneBuzzer buzzController{Hardware::Buzzer::kPwmPin, Hardware::Buzzer::kPwmChannel};

	LedController ledController{&greenLed, &blueLed, &redLed, false};

	EventBus::registerObserver(&paramStorage);
	EventBus::registerObserver(&displayDriver);
	EventBus::registerObserver(&uiObserver);
	EventBus::registerObserver(&smartBus);
	EventBus::registerObserver(&rtc);
	EventBus::registerObserver(&pumpController);
	EventBus::registerObserver(&lightController);
	EventBus::registerObserver(&buzzController);
	EventBus::registerObserver(&ledController);
	EventBus::registerObserver(&LowerMonitor::instance());
	EventBus::registerObserver(&UpperMonitor::instance());
	EventBus::registerObserver(&MasterMonitor::instance());

	LinearSched sched;
	sched.registerTask(&rtc);
	sched.registerTask(&pumpController);
	sched.registerTask(&lightController);
	sched.registerTask(&buzzController);
	sched.registerTask(&smartBus);
	sched.registerTask(&ledController);
	sched.registerTask(&pcf);
	sched.registerTask(&MasterMonitor::instance());

	// Включаем и отрисовываем экран
	displayDriver.setupDisplay();
	displayDriver.setupLvgl();
	uiInit(&uiObserver);

	// Загружаем параметры во все модули
	paramStorage.firstLoad();

	// Поток для работы с дисплеем, увеличенный стек, припиненно к ядру
	TaskHandle_t displayTask;
	xTaskCreatePinnedToCore(displayTaskFunc, "Display", 8 * 1024, &lvglMutex, 5, &displayTask, 1);
	esp_task_wdt_add(displayTask);

	Event ev;
	ev.type = EventType::ToneBuzzerSignal;
	ev.data.buzToneSignal = ToneBuzzerSignal::Enabling;
	EventBus::throwEvent(&ev, nullptr);

	MasterMonitor::instance().invoke();

	while(true) {
		// Обработка SmartBus
		const size_t len = serial.bytesAvaillable();
		if (len) {
			uint8_t buffer[64];
			const size_t finLen = std::min(len, sizeof(buffer));
			serial.read(buffer, finLen);
			smartBus.update(buffer, finLen);
		}

		sched.doTasks();
		lgfx::delay(5);
	}
}
