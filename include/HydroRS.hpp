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

	struct Devices {
		TelemetryUnit lower{DeviceType::Lower};
		TelemetryUnit upper{DeviceType::Upper};
	} devices;

	Gpio &latch;
public:
	HydroRS(Interface &aInterface, uint8_t aNodeUID, Gpio &aLatchPin):
		BaseType{aInterface, aNodeUID},
		devices{},
		latch{aLatchPin}
	{

	}

	DeviceType getDeviceTypeFromUID(uint8_t aUID)
	{
		return static_cast<DeviceType>(aUID);
	}

	uint8_t getUIDFromDeviceType(DeviceType aDevice) 
	{
		return static_cast<uint8_t>(aDevice);
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
						handleDeviceAttaching(type);
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
					handleDeviceDetaching(type);
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
		processDevice(devices.lower, aCurrentTime);
		processDevice(devices.upper, aCurrentTime);
	}

	// UtilitaryRS interface
	uint8_t handleCommand(uint8_t aCommand, uint8_t aArgument) override
	{
		return 0; // Мастер не принимает команд, вернется Ack с кодом 0
	}

	void handleAck(uint8_t aTranceiverUID, uint8_t aReturnCode) override
	{
		const auto device = getDeviceTypeFromUID(aTranceiverUID);

		if (device == devices.lower.device) {
			devices.lower.lastAckTime = TimeWrapper::milliseconds();
		} else if (device == devices.upper.device) {
			devices.upper.lastAckTime = TimeWrapper::milliseconds();
		}
	}

	uint8_t handleAnswer(uint8_t aTranceiverUID, uint8_t aRequest, const uint8_t *aData, uint8_t aLength) override
	{
		const auto device = getDeviceTypeFromUID(aTranceiverUID);

		if ((device == devices.lower.device) && (aRequest == static_cast<uint8_t>(Requests::RequestTelemetry))) {
			devices.lower.lastAckTime = TimeWrapper::milliseconds();

			// Если ожидаемая длина не сходится - ошибка
			if (aLength != sizeof(LowerTelemetry)) {
				return 0;
			}

			LowerTelemetry telemetry;
			memcpy(&telemetry, aData, aLength);
			processLowerTelemetry(telemetry);

			return 1;
		} else if ((device == devices.upper.device) && (aRequest == static_cast<uint8_t>(Requests::RequestTelemetry))) {
			devices.lower.lastAckTime = TimeWrapper::milliseconds();

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
		latch.set();
		return BaseType::sendCommand(aReceiverUID, aCommand, aArgument);
		latch.reset();
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

		if (ev.data.lowerData.flags & LowerFlags::LowerNoWaterFlag 
			|| ev.data.lowerData.flags & LowerFlags::LowerPumpOverCurrentFlag 
			|| ev.data.lowerData.flags & LowerFlags::LowerPumpLowCurrentFlag) {
			ev.data.lowerData.health = DeviceHealth::DeviceError;
		} else if (ev.data.lowerData.flags & LowerFlags::LowerPHSensorErrorFlag
			|| ev.data.lowerData.flags & LowerFlags::LowerPPMSensorErrorFlag
			|| ev.data.lowerData.flags & LowerFlags::LowerTempSensorErrorFlag) {
			ev.data.lowerData.health = DeviceHealth::DeviceWarning;
		} else {
			ev.data.lowerData.health = DeviceHealth::DeviceWorking;
		}

		EventBus::throwEvent(&ev);
	}

	void processUpperTelemetry(UpperTelemetry &aTelem)
	{
		Event ev;
		ev.type = EventType::UpdateUpperData;
		ev.data.upperData.swingLevelState = aTelem.swingLevelState == 1 ? true : false;
		ev.data.upperData.lampState = aTelem.lampState == 1 ? true : false;
		ev.data.upperData.damState = aTelem.damState == 1 ? true : false;
		ev.data.upperData.flags = aTelem.deviceFlags;

		if (ev.data.upperData.flags & UpperFlags::UpperPowerError) {
			ev.data.upperData.health = DeviceHealth::DeviceError;
		} else if (ev.data.upperData.flags & UpperFlags::UpperTopWaterLevelStuck) {
			ev.data.upperData.health = DeviceHealth::DeviceWarning;
		} else {
			ev.data.upperData.health = DeviceHealth::DeviceWorking;
		}

		EventBus::throwEvent(&ev);
	}

	void handleDeviceDetaching(DeviceType aDevice)
	{
		Event ev;
		ev.type = EventType::RsDeviceDetached;
		ev.data.device = aDevice;
		EventBus::throwEvent(&ev);
	}

	void handleDeviceAttaching(DeviceType aDevice)
	{
		Event ev;
		ev.type = EventType::RsDeviceAttached;
		ev.data.device = aDevice;
		EventBus::throwEvent(&ev);
	}

	// AbstractEventObserver interface
	EventResult handleEvent(Event *e) override
	{
		switch (e->type) {
			case EventType::ActionRequest: {
				switch (e->data.action) {
					case Action::TurnPumpOn:
						if (devices.lower.state == DeviceState::Working) {
							sendCommand(getUIDFromDeviceType(DeviceType::Lower), static_cast<uint8_t>(Commands::SetPumpState), 1);
						}
						return EventResult::HANDLED;

					case Action::TurnPumpOff:
						if (devices.lower.state == DeviceState::Working) {
							sendCommand(getUIDFromDeviceType(DeviceType::Lower), static_cast<uint8_t>(Commands::SetPumpState), 0);
						}
						return EventResult::HANDLED;

					case Action::TurnLampOn:
						if (devices.upper.state == DeviceState::Working) {
							sendCommand(getUIDFromDeviceType(DeviceType::Upper), static_cast<uint8_t>(Commands::SetLampState), 1);
						}
						return EventResult::HANDLED;

					case Action::TurnLampOff:
						if (devices.upper.state == DeviceState::Working) {
							sendCommand(getUIDFromDeviceType(DeviceType::Upper), static_cast<uint8_t>(Commands::SetLampState), 0);
						}
						return EventResult::HANDLED;

					case Action::OpenDam:
						if (devices.upper.state == DeviceState::Working) {
							sendCommand(getUIDFromDeviceType(DeviceType::Upper), static_cast<uint8_t>(Commands::SetDamState), 1);
						}
						return EventResult::HANDLED;

					case Action::CloseDam:
						if (devices.upper.state == DeviceState::Working) {
							sendCommand(getUIDFromDeviceType(DeviceType::Upper), static_cast<uint8_t>(Commands::SetDamState), 0);
						}
						return EventResult::HANDLED;
				}
			} break;

			default:
				return EventResult::IGNORED;
				break;
		}

		return EventResult::IGNORED;
	}
};

#endif // INCLUDE_HYDRORS_HPP_