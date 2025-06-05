/*!
@file
@brief Шина событий-данных с очередью внутри
@author V-Nezlo (vlladimirka@gmail.com)
@date 04.06.2025
@version 1.0
*/

#ifndef INCLUDE_EVENTBUSV2_HPP_
#define INCLUDE_EVENTBUSV2_HPP_

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "HydroRSTypes.hpp"
#include "Types.hpp"

#include <cctype>
#include <cstddef>
#include <functional>
#include <iostream>
#include <bitset>
#include <math.h>
#include <limits>
#include <string>
#include <variant>
#include <chrono>
#include <vector>

using namespace std;

enum class EventType : uint8_t {
	SetCurrentTime,
	GetCurrentTime,
	UpdateUpperData,
	UpdateLowerData,
	ActionRequest,
	SettingsUpdated,
	NewBrightness,
	NewBuzVolume,
	ToneBuzzerSignal,
	RsDeviceDetached,
	RsDeviceAttached,
	UpdateDeviceHealth
};

enum class EventResult : uint8_t {
	HANDLED,
	IGNORED,
	PASS_ON
};

enum class Action : uint8_t {
	TurnPumpOn,
	TurnPumpOff,
	TurnLampOn,
	TurnLampOff,
	CloseDam,
	OpenDam,
	ECCalibSens
};

enum class ToneBuzzerSignal {
	Disabled,
	Enabling,
	Error,
	CriticalError,
	Warning,
	Connected,
	Disconnected,
	Touch,
	Information
};

class AbstractEventObserver;

struct Event {
	EventType type;
	union {
		Time time;
		UpperInternalData upperData;
		LowerInternalData lowerData;
		Action action;
		Settings settings;
		uint8_t brightness;
		uint8_t volume;
		ToneBuzzerSignal buzToneSignal;
		DeviceType device;
		UpdateHealth updateHealth;
	} data;

	AbstractEventObserver *sender = nullptr;
};

class AbstractEventObserver {
public:
	virtual EventResult handleEvent(Event *e) = 0;
};

class EventBus
{
public:
	static bool init()
	{
		if (queue)
			return true;

		queue = xQueueCreate(16, sizeof(Event));
		if (!queue)
			return false;

		xTaskCreate(eventTask, "EventBusTask", 2048, nullptr, 5, nullptr);
		return true;
	}

	static void registerObserver(AbstractEventObserver *observer)
	{
		observers.push_back(observer);
	}

	static void unregisterObserver(AbstractEventObserver *observer)
	{
		observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
	}

	static bool throwEvent(Event *e, AbstractEventObserver *sender = nullptr)
	{
		e->sender = sender;
		return xQueueSend(queue, e, 0) == pdTRUE;
	}

	static bool throwEventFromISR(Event *e, AbstractEventObserver *sender = nullptr)
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		e->sender = sender;
		bool result = xQueueSendFromISR(queue, e, &xHigherPriorityTaskWoken) == pdTRUE;
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		return result;
	}

private:
	static inline QueueHandle_t queue = nullptr;
	static inline std::vector<AbstractEventObserver *> observers;

	static void eventTask(void *)
	{
		Event event;

		while (true)
		{
			if (xQueueReceive(queue, &event, portMAX_DELAY) == pdTRUE)
			{
				for (auto observer : observers)
				{
					if (event.sender == observer)
						continue;

					auto result = observer->handleEvent(&event);
					if (result == EventResult::HANDLED)
						break;
				}
			}
		}
	}
};

#endif