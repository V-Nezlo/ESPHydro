/*!
@file
@brief Клиентская сторона RsHandler
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#ifndef INCLUDE_HYDRORS_HPP_
#define INCLUDE_HYDRORS_HPP_

#include "TimeWrapper.hpp"
#include <Lib/RsHandler.hpp>
#include "HydroRSTypes.hpp"
#include <EventBus.hpp>
#include <chrono>
#include <array>

template<class Interface, typename Crc, size_t ParserSize>
class HydroRS : public RS::RsHandler<Interface, Crc, ParserSize>, public AbstractEventObserver {
	using BaseType = RS::RsHandler<Interface, Crc, ParserSize>;

	enum class DeviceState {
		Probing,
		Working,
		Suspended,
		Disabled
	};

	struct TelemetryUnit {
		DeviceType device{DeviceType::Unknown};
		DeviceState state{DeviceState::Probing};
		std::chrono::milliseconds lastCallTime{0};
		std::chrono::milliseconds lastAckTime{0};
		std::chrono::milliseconds actionTimeout{0};

		TelemetryUnit(DeviceType aDevice)
		{
			device = aDevice;
		}
	};

	TelemetryUnit lowerDevice;

	Gpio &latch;
public:
	HydroRS(Interface &aInterface, uint8_t aNodeUID, Gpio &aLatchPin):
		BaseType{aInterface, aNodeUID},
		lowerDevice{DeviceType::Lower},
		latch{aLatchPin}
	{

	}

	DeviceType getDeviceTypeFromUID(uint8_t aUID)
	{
		return static_cast<DeviceType>(aUID);
	}

	void process(std::chrono::milliseconds aCurrentTime)
	{
		const DeviceType &type = lowerDevice.device;
		const DeviceState &state = lowerDevice.state;
		const std::chrono::milliseconds &lastAckTime = lowerDevice.lastAckTime;
		const std::chrono::milliseconds &lastCallTime = lowerDevice.lastCallTime;
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
					}
				}
				break;
			case DeviceState::Working:
				// Отправляем запрос телеметрии каждые 500мс
				if (aCurrentTime > lastCallTime + std::chrono::milliseconds{500}) {
					lastCallTime = aCurrentTime;

					BaseType::sendRequest(deviceUID, static_cast<uint8_t>(Requests::RequestTelemetry), sizeof(LowerTelemetry));
				}
				// Параллельно проверяем факт получения Ack, если ack не приходят - возвращаемся в Probing
				if (aCurrentTime > lastAckTime + std::chrono::milliseconds{3000}) {
					state = DeviceState::Probing;
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

	// UtilitaryRS interface
	uint8_t handleCommand(uint8_t aCommand, uint8_t aArgument) override
	{
		return 0; // Мастер не принимает команд, вернется Ack с кодом 0
	}

	void handleAck(uint8_t aTranceiverUID, uint8_t aReturnCode) override
	{
		const auto device = getDeviceTypeFromUID(aTranceiverUID);

		if (device == lowerDevice.device) {
			lowerDevice.lastAckTime = TimeWrapper::milliseconds();
		}
	}

	uint8_t handleAnswer(uint8_t aTranceiverUID, uint8_t aRequest, const uint8_t *aData, uint8_t aLength) override
	{
		const auto device = getDeviceTypeFromUID(aTranceiverUID);

		if ((device == lowerDevice.device) && (aRequest == static_cast<uint8_t>(Requests::RequestTelemetry))) {
			lowerDevice.lastAckTime = TimeWrapper::milliseconds();

			// Если ожидаемая длина не сходится - ошибка
			if (aLength != sizeof(LowerTelemetry)) {
				return 0;
			}

			LowerTelemetry telemetry;
			memcpy(&telemetry, aData, aLength);

			// Передача телеметрии

			return 1;
		}

		// Тут парсинг телеметрии от устройств
		return 0;
	}

	bool processRequest(uint8_t aTransmitUID, uint8_t aRequest, uint8_t aRequestedDataSize) override
	{
		return false; // Мастер не принимает запросов, вернется Ack с кодом 0
	}

	void sendCommand(uint8_t aReceiverUID, uint8_t aCommand, uint8_t aArgument)
	{
		latch.set();
		return BaseType::sendCommand(aReceiverUID, aCommand, aArgument);
		latch.reset();
	}

	// AbstractEventObserver interface
	EventResult handleEvent(Event *e) override
	{
		switch (e->type) {
			case EventType::ActionRequest: {
				switch (e->data.action) {
					case Action::TurnPumpOn:
						sendCommand(1, 1, 1);
						return EventResult::HANDLED;
						break;
					case Action::TurnPumpOff:
						sendCommand(1, 1, 0);
						return EventResult::HANDLED;
						break;
					case Action::TurnLampOn:
						sendCommand(2, 1, 1);
						return EventResult::HANDLED;
						break;
					case Action::TurnLampOff:
						sendCommand(2, 1, 0);
						return EventResult::HANDLED;
						break;
				}
			} break;

			default:
				return EventResult::IGNORED;
				break;
		}

		return EventResult::IGNORED;
	}
};

#endif