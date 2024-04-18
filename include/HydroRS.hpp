/*!
@file
@brief Клиентская сторона RsHandler
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#ifndef INCLUDE_HYDRORS_HPP_
#define INCLUDE_HYDRORS_HPP_

#include <Lib/RsHandler.hpp>
#include <EventBus.hpp>

template<class Interface, typename Crc, size_t ParserSize>
class HydroRS : public RS::RsHandler<Interface, Crc, ParserSize>, public AbstractEventObserver {
	using BaseType = RS::RsHandler<Interface, Crc, ParserSize>;

	enum class Recipient {
		Upper,
		Lower
	};

	struct PendingInfo {
		bool status{false};
		Recipient node{Recipient::Upper};
		uint8_t returnCode{0};
		std::chrono::seconds requestTime;
	} pending;

public:
	HydroRS(Interface &aInterface, uint8_t aNodeUID):
		BaseType{aInterface, aNodeUID}
	{

	}

	void process()
	{
		switch(pending.node) {
			case Recipient::Lower:
				BaseType::sendRequest(1, 1, sizeof(int));
				break;
			default:
				break;
		}

	}

	// UtilitaryRS interface
	uint8_t handleCommand(uint8_t aCommand, uint8_t aArgument) override
	{
		return 0;
	}

	void handleAck(uint8_t aTranceiverUID, uint8_t aReturnCode) override
	{
		
	}

	uint8_t handleAnswer(uint8_t aRequest, const uint8_t *aData, uint8_t aLength) override
	{
		return 0;
	}

	bool processRequest(uint8_t aTransmitUID, uint8_t aRequest, uint8_t aRequestedDataSize) override
	{
		return true;
	}

	void sendCommand(uint8_t aReceiverUID, uint8_t aCommand, uint8_t aArgument)
	{
		return BaseType::sendCommand(aReceiverUID, aCommand, aArgument);
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
				break;
		}

		return EventResult::IGNORED;
	}
};

#endif