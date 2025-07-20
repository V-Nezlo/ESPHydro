/*!
@file
@brief Обработчик насоса
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#ifndef INCLUDE_PUMPCONTROLLER_HPP_
#define INCLUDE_PUMPCONTROLLER_HPP_

#include "ConfigStorage.hpp"
#include "EventBus.hpp"
#include "LinearSched.hpp"
#include "TimeWrapper.hpp"
#include "Types.hpp"
#include "Options.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <chrono>

class PumpController : public AbstractEventObserver, public AbstractLinearTask {
	enum class PumpState : uint8_t {
		PumpOn,
		PumpOff
	};

	enum class SwingState : uint8_t {
		SwingOn,
		SwingOff
	};

	enum class DamTankState : uint8_t {
		DamUnlocked,
		DamLocked
	};

	PumpModes mode;

	// Телеметрируемые переменные
	PumpState pumpState;
	DamTankState damTankState;
	uint8_t currentWaterLevel;
	bool upperState;

	SwingState swingState;

	std::chrono::milliseconds lastActionTime;
	std::chrono::milliseconds lastSwingTime;
	std::chrono::milliseconds waterFillingTimer;
	std::chrono::milliseconds lastChecksTime;
	bool fillingCheckEn;

	bool enabled;
	std::chrono::seconds pumpOnTime;
	std::chrono::seconds pumpOffTime;
	std::chrono::seconds swingTime;
	std::chrono::seconds maxFloodingTime;

	// Пояснение: некоторые поля этого класса являются разделяемым ресурсом
	// Поэтому, пока ресуры класса модифицируются UI необходимо отключать работу контроллера насоса
	SemaphoreHandle_t mutex;

public:
	PumpController();
	EventResult handleEvent(Event *e) override;
	void process(std::chrono::milliseconds aCurrentTime) override;

private:
	void updateMode(PumpModes aNewMode);
	void setPumpState(PumpState aState);
	void setDamState(DamTankState aState);

	void sendCommandToPump(bool aNewPumpState);
	void sendCommandToDam(bool aNewDamState);

	/// @brief Функция проверки разрешения работы насоса
	/// @return true если включение насоса разрешено
	bool permitForAction() const;

	/// @brief EBB режим, вкл выкл насоса по времени и проверки на флудинг
	void processEBBNormalMode(std::chrono::milliseconds aCurrentTime);

	/// @brief Расширенный EBB режим, вкл выкл насоса по времени и отработка "качелей"
	void processEBBSwingMode(std::chrono::milliseconds aCurrentTime);

	/// @brief Самый продвинутый режим, качели реализованы через "дамбу"
	void processEBBDumMode(std::chrono::milliseconds aCurrentTime);

	/// @brief Самый простой режим, просто вкл-выкл насоса по времени
	void processDripMode(std::chrono::milliseconds aCurrentTime);

	/// @brief
	void processMaintanceMode();
};

#endif // INCLUDE_PUMPCONTROLLER_HPP_
