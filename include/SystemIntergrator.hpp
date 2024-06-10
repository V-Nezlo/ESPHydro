/*!
@file
@brief Интегратор флагов системы, обьеденяющий их для GUI
@author V-Nezlo (vlladimirka@gmail.com)
@date 16.05.2024
@version 1.0
*/

#ifndef INCLUDE_SYSTEMINTERGRATOR_HPP_
#define INCLUDE_SYSTEMINTERGRATOR_HPP_

#include "LinearSched.hpp"
#include "EventBus.hpp"
#include <chrono>

class SystemIntegrator : public AbstractEventObserver, public AbstractLinearTask {
public:
	SystemIntegrator();

	void process(std::chrono::milliseconds aCurrentTime) override;
	EventResult handleEvent(Event *e) override;
private:
	uint8_t systemFlagStorage;
	std::chrono::milliseconds lastCheckTime;
	bool updated;
	BuzzerSignal signalNeeds;
};

#endif // INCLUDE_SYSTEMINTERGRATOR_HPP_