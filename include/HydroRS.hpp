/*!
@file
@brief Клиентская сторона RsHandler
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#ifndef INCLUDE_HYDRORS_HPP_
#define INCLUDE_HYDRORS_HPP_

#include "EventBus.hpp"
#include "LinearSched.hpp"
#include "MutexLock.hpp"
#include "TimeWrapper.hpp"
#include "Types.hpp"
#include "HydroRSTypes.hpp"
#include "LowerMonitor.hpp"
#include "UpperMonitor.hpp"

#include <UtilitaryRS/RsHandler.hpp>
#include <esp_log.h>

#include <chrono>
#include <array>
#include <utility>

template<class Interface, typename Crc, size_t ParserSize>
class HydroRS : public RS::RsHandler<Interface, Crc, ParserSize>, public AbstractEventObserver, public AbstractLinearTask {
	using BaseType = RS::RsHandler<Interface, Crc, ParserSize>;

	enum class DeviceState {
		Probing,
		Working,
		Suspended,
		Disabled
	};

	struct TelemetryUnit {
		DeviceType device;
		DeviceState state{DeviceState::Probing};
		std::chrono::milliseconds lastCallTime{0};
		std::chrono::milliseconds lastAckTime{0};
		std::chrono::milliseconds actionTimeout{0};

		TelemetryUnit(DeviceType aDevice) : device{aDevice} {}
	};

	std::array<TelemetryUnit, 2> devices;
	ModulesConfiguration modules;
	SemaphoreHandle_t mutex;	
public:
	HydroRS(Interface &aInterface, uint8_t aNodeUID):
		BaseType{aInterface, aNodeUID},
		devices{TelemetryUnit{DeviceType::Lower}, TelemetryUnit{DeviceType::Upper}},
		mutex{xSemaphoreCreateMutex()}
	{
	}

	void processDevice(TelemetryUnit &aUnit, std::chrono::milliseconds aCurrentTime)
	{
		const DeviceType &type = aUnit.device;
		DeviceState &state = aUnit.state;
		std::chrono::milliseconds &lastAckTime = aUnit.lastAckTime;
		std::chrono::milliseconds &lastCallTime = aUnit.lastCallTime;
		const uint8_t deviceUID = static_cast<uint8_t>(type);

		switch(state) {
			case DeviceState::Probing:
				// Раз в секунду отправляем probe до момента получения первого ack
				if (aCurrentTime > lastCallTime + std::chrono::milliseconds{1000}) {
					lastCallTime = aCurrentTime;

					if (lastAckTime == std::chrono::milliseconds{0}) {
						BaseType::sendProbe(deviceUID);
					} else {
						state = DeviceState::Working;
						ESP_LOGI("RS", "Device %u attached", static_cast<uint8_t>(type));
						// Отправляем событие подключения устройства
						sendAttachEventToBus(type); // Как реагировать на событие пока непонятно
						resetDeviceToInitialState(type); // Сбросим насос и дамбу при инициализации
					}
				}
				break;

			case DeviceState::Working:
				// Отправляем запрос телеметрии каждые 500мс
				if (aCurrentTime > lastCallTime + std::chrono::milliseconds{500}) {
					lastCallTime = aCurrentTime;

					if (type == DeviceType::Lower) {
						BaseType::sendRequest(deviceUID, static_cast<uint8_t>(Requests::RequestTelemetry), sizeof(LowerTelemetry));
					} else if (type == DeviceType::Upper) {
						BaseType::sendRequest(deviceUID, static_cast<uint8_t>(Requests::RequestTelemetry), sizeof(UpperTelemetry));
					}
				}
				// Параллельно проверяем факт получения Ack, если ack не приходят - возвращаемся в Probing
				if (aCurrentTime > lastAckTime + std::chrono::milliseconds{3000}) {
					lastAckTime = std::chrono::milliseconds{0};
					state = DeviceState::Probing;
					// Отправляем событие отключения устройства
					ESP_LOGI("RS", "Device %u detached", static_cast<uint8_t>(type));
					sendDetachEventToBus(type);
				}
				break;
			case DeviceState::Suspended:
				// Пока непонятно
				break;
			case DeviceState::Disabled:
				// Как выключать устроства непонятно
				break;
		}
	}

	void process(std::chrono::milliseconds aCurrentTime) override
	{
		static std::chrono::milliseconds lastDeviceCallTime{0};
		static uint8_t deviceToCall{0};

		if (aCurrentTime > lastDeviceCallTime + std::chrono::milliseconds{100}) {
			MutexLock lock(mutex);
			processDevice(devices[deviceToCall], aCurrentTime);

			if (deviceToCall == (devices.size() - 1)){
				deviceToCall = 0;
			} else {
				++deviceToCall;
			}
		}
	}

	// UtilitaryRS interface
	uint8_t handleCommand(uint8_t aCommand, uint8_t aArgument) override
	{
		return 0; // Мастер не принимает команд, вернется Ack с кодом 0
	}

	void handleAck(uint8_t aTranceiverUID, uint8_t aReturnCode) override
	{
		ESP_LOGV("RS", "Ack,Device: %u, Code: %u", aTranceiverUID, aReturnCode);

		if (aTranceiverUID == devices[0].device) {
			devices[0].lastAckTime = TimeWrapper::milliseconds();
		} else if (aTranceiverUID == devices[1].device) {
			devices[1].lastAckTime = TimeWrapper::milliseconds();
		}
	}

	uint8_t handleAnswer(uint8_t aTranceiverUID, uint8_t aRequest, const uint8_t *aData, uint8_t aLength) override
	{
		ESP_LOGV("RS", "Answer,Device: %u, Req: %u", aTranceiverUID, aRequest);

		if ((aTranceiverUID == devices[0].device) && (aRequest == static_cast<uint8_t>(Requests::RequestTelemetry))) {
			devices[0].lastAckTime = TimeWrapper::milliseconds();

			// Если ожидаемая длина не сходится - ошибка
			if (aLength != sizeof(LowerTelemetry)) {
				MasterMonitor::instance().setFlag(MasterFlags::DeviceMismatch);
				return 0;
			}

			LowerTelemetry telemetry;
			memcpy(&telemetry, aData, aLength);
			processLowerTelemetry(telemetry);

			return 1;
		} else if ((aTranceiverUID == devices[1].device) && (aRequest == static_cast<uint8_t>(Requests::RequestTelemetry))) {
			devices[1].lastAckTime = TimeWrapper::milliseconds();

			// Если ожидаемая длина не сходится - ошибка
			if (aLength != sizeof(UpperTelemetry)) {
				MasterMonitor::instance().setFlag(MasterFlags::DeviceMismatch);
				return 0;
			}

			UpperTelemetry telemetry;
			memcpy(&telemetry, aData, aLength);
			processUpperTelemetry(telemetry);

			return 1;
		} else {
			return 0;
		}
	}

	bool processRequest(uint8_t aTransmitUID, uint8_t aRequest, uint8_t aRequestedDataSize) override
	{
		return false; // Мастер не принимает запросов, вернется Ack с кодом 0
	}

	// TODO MADE ASYNC
	bool sendCommand(DeviceType aType, Commands aCommand, uint8_t aArgument)
	{
		if (isDevicePresent(aType)) {
			BaseType::sendCommand(static_cast<uint8_t>(aType), static_cast<uint8_t>(aCommand), aArgument);
			return true;
		} else {
			return false;
		}
	}

	void processLowerTelemetry(LowerTelemetry &aTelem)
	{
		Event ev;
		ev.type = EventType::UpdateLowerData;
		ev.data.lowerData.ppm = aTelem.waterPPM;
		ev.data.lowerData.ph10 = aTelem.waterPH10;
		ev.data.lowerData.waterTemp10 = aTelem.waterTemperature10;
		ev.data.lowerData.pumpState = aTelem.pumpState == 1 ? true : false;
		ev.data.lowerData.waterLevel = aTelem.waterLevelPerc;
		ev.data.lowerData.flags = aTelem.deviceFlags;

		// Снимем флаги ошибок модулей если они выключены через CLI
		if (aTelem.deviceFlags & static_cast<uint8_t>(LowerFlags::PHSensorError) && !modules.phSensor) {
			ev.data.lowerData.flags &= ~static_cast<uint8_t>(LowerFlags::PHSensorError);
		}
		if (aTelem.deviceFlags & static_cast<uint8_t>(LowerFlags::PPMSensorError) && !modules.ppmSensor) {
			ev.data.lowerData.flags &= ~static_cast<uint8_t>(LowerFlags::PPMSensorError);
		}

		// Обновим синглтон без учатия EventBus
		LowerMonitor::instance().updateFromTelemetry(ev.data.lowerData.flags);
		// Внутри автоматически считается Health
		EventBus::throwEvent(&ev, this);
	}

	void processUpperTelemetry(UpperTelemetry &aTelem)
	{
		Event ev;
		ev.type = EventType::UpdateUpperData;
		ev.data.upperData.swingLevelState = aTelem.swingLevelState == 1 ? true : false;
		ev.data.upperData.lampState = aTelem.lampState == 1 ? true : false;
		ev.data.upperData.damState = aTelem.damState == 1 ? true : false;
		ev.data.upperData.flags = aTelem.deviceFlags;

		// Обновим синглтон без учатия EventBus
		UpperMonitor::instance().updateFromTelemetry(aTelem.deviceFlags);
		EventBus::throwEvent(&ev, this);
	}

	void sendDetachEventToBus(DeviceType aDevice)
	{
		Event ev;
		ev.type = EventType::RsDeviceDetached;
		ev.data.device = aDevice;
		EventBus::throwEvent(&ev, this);
	}

	void sendAttachEventToBus(DeviceType aDevice)
	{
		Event ev;
		ev.type = EventType::RsDeviceAttached;
		ev.data.device = aDevice;
		EventBus::throwEvent(&ev, this);
	}

	/// @brief Сбрасываем устройство в начальное состояние
	/// @param aDevice
	void resetDeviceToInitialState(DeviceType aDevice)
	{
		switch (aDevice) {
			case DeviceType::Lower:
				sendCommand(DeviceType::Lower, Commands::SetPumpState, 0);
				break;
			case DeviceType::Upper:
				sendCommand(DeviceType::Upper, Commands::SetDamState, 1);
				break;
			default:
				break;
		}
	}

	// AbstractEventObserver interface
	EventResult handleEvent(Event *e) override
	{
		switch (e->type) {
			case EventType::ActionRequest: {
				MutexLock lock(mutex);
				switch (e->data.action) {
					case Action::TurnPumpOn:
						sendCommand(DeviceType::Lower, Commands::SetPumpState, 1);
						return EventResult::HANDLED;

					case Action::TurnPumpOff:
						sendCommand(DeviceType::Lower, Commands::SetPumpState, 0);
						return EventResult::HANDLED;

					case Action::TurnLampOn:
						sendCommand(DeviceType::Upper, Commands::SetLampState, 1);
						return EventResult::HANDLED;

					case Action::TurnLampOff:
						sendCommand(DeviceType::Upper, Commands::SetLampState, 0);
						return EventResult::HANDLED;

					case Action::OpenDam:
						sendCommand(DeviceType::Upper, Commands::SetDamState, 1);
						return EventResult::HANDLED;

					case Action::CloseDam:
						sendCommand(DeviceType::Upper, Commands::SetDamState, 0);
						return EventResult::HANDLED;

					case Action::ECCalibSens:
						sendCommand(DeviceType::Lower, Commands::CalibECSensor, 0);
						return EventResult::HANDLED;

					default:
						return EventResult::IGNORED;
					}
				}

			case EventType::SettingsUpdated: {
				MutexLock lock(mutex);
				modules = e->data.settings.modules;
				return EventResult::PASS_ON;
			}

			default:
				return EventResult::IGNORED;
		}
	}

	bool isDevicePresent(DeviceType aType)
	{
		switch (aType) {
			case DeviceType::Lower:
				return devices[0].state != DeviceState::Disabled;
			case DeviceType::Upper:
				return devices[1].state != DeviceState::Disabled;
			case DeviceType::AUX:
				return false;
			case DeviceType::Master:
				return true;

			default:
				return false;
		}
	}
};

#endif // INCLUDE_HYDRORS_HPP_