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
#include "TimeWrapper.hpp"
#include "Types.hpp"
#include "HydroRSTypes.hpp"

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
public:
	HydroRS(Interface &aInterface, uint8_t aNodeUID):
		BaseType{aInterface, aNodeUID},
		devices{TelemetryUnit{DeviceType::Lower}, TelemetryUnit{DeviceType::Upper}}
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

	void sendCommand(uint8_t aReceiverUID, uint8_t aCommand, uint8_t aArgument)
	{
		return BaseType::sendCommand(aReceiverUID, aCommand, aArgument);
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
				sendCommand(DeviceType::Lower, static_cast<uint8_t>(Commands::SetPumpState), 0);
				break;
			case DeviceType::Upper:
				sendCommand(DeviceType::Upper, static_cast<uint8_t>(Commands::SetDamState), 1);
				break;
			default:
				break;
		}
	}

	// AbstractEventObserver interface
	EventResult handleEvent(Event *e) override
	{
		switch (e->type) {
			case EventType::ActionRequest:
				switch (e->data.action) {
					case Action::TurnPumpOn:
						if (devices[0].state != DeviceState::Disabled) {
							sendCommand(DeviceType::Lower, static_cast<uint8_t>(Commands::SetPumpState), 1);
						}
						return EventResult::HANDLED;

					case Action::TurnPumpOff:
						if (devices[0].state != DeviceState::Disabled) {
							sendCommand(DeviceType::Lower, static_cast<uint8_t>(Commands::SetPumpState), 0);
						}
						return EventResult::HANDLED;

					case Action::TurnLampOn:
						if (devices[1].state != DeviceState::Disabled) {
							sendCommand(DeviceType::Upper, static_cast<uint8_t>(Commands::SetLampState), 1);
						}
						return EventResult::HANDLED;

					case Action::TurnLampOff:
						if (devices[1].state != DeviceState::Disabled) {
							sendCommand(DeviceType::Upper, static_cast<uint8_t>(Commands::SetLampState), 0);
						}
						return EventResult::HANDLED;

					case Action::OpenDam:
						if (devices[1].state != DeviceState::Disabled) {
							sendCommand(DeviceType::Upper, static_cast<uint8_t>(Commands::SetDamState), 1);
						}
						return EventResult::HANDLED;

					case Action::CloseDam:
						if (devices[1].state != DeviceState::Disabled) {
							sendCommand(DeviceType::Upper, static_cast<uint8_t>(Commands::SetDamState), 0);
						}
						return EventResult::HANDLED;

					default:
						return EventResult::IGNORED;
				}

			default:
				return EventResult::IGNORED;
				break;
		}
	}
};

#endif // INCLUDE_HYDRORS_HPP_