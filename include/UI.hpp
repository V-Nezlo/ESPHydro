/*!
@file
@brief Сишный код, отвечающий за создание графического интерфейса
@author V-Nezlo (vlladimirka@gmail.com)
@date 17.04.2024
@version 1.0
*/

#ifndef INCLUDE_UI_HPP_
#define INCLUDE_UI_HPP_

#include "Types.hpp"
#include "EventBus.hpp"
#include "MutexLock.hpp"

#include <freertos/FreeRTOS.h>
#include "freertos/semphr.h"

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t lv_menu_builder_variant_t;
class UiEventObserver;

void styleInitialize();
void menuCreate(lv_obj_t *parent);
void actuatorsCreate(lv_obj_t *parent, uint16_t aYOffset);
void mainPageCreate(lv_obj_t * parent);
void loadingScreenCreate(lv_obj_t * parent);
void keyboardCreate();

lv_obj_t *createSwitch(lv_obj_t * parent, const char * icon, const char * txt, bool chk);
lv_obj_t *createSlider(lv_obj_t * parent, const char * icon, const char * txt, int32_t min, int32_t max, int32_t val);
lv_obj_t *createText(lv_obj_t * parent, const char * icon, const char * txt, lv_menu_builder_variant_t builder_variant);
// Создает презентованные LABEL и LED
void createLed(lv_obj_t *parent, lv_obj_t *&label, lv_obj_t *&led, const char *text, uint8_t ledShiftX, uint8_t labelAlignX, uint8_t labelAlignY);

void createAdditionalPanels();
void settingsButtonEvent(lv_event_t * e);
void uiInit(UiEventObserver *aObserver);
void displayMainPage();

void textAreasReset(lv_obj_t *aScreen);
bool textAreasApply(lv_obj_t *aScreen);

void updatePanelStyleByFlags(lv_obj_t *aModulePanel, DeviceHealth aHealth);
void updateActuatorByFlags(lv_obj_t *aActuator, bool aDevicePresent, bool aActivated);

void updateLowerData(const struct LowerInternalData *aData);
void updateUpperData(struct UpperInternalData *aData);
void updateAUXData(struct AuxData *aData);

void fillDevicePlaceholders(DeviceType aDevice);

void applyNewCurrentTime(struct Time *aTime);
void enterParameters(struct Settings *aParams);
struct Settings *saveParameters();
bool getLoggingState();
void updateMainPagePumpTypeLabel();
void updateDeviceHealth(DeviceType aType, DeviceHealth aHealth);
void processTap(lv_event_t *);

enum {
	LV_MENU_ITEM_BUILDER_VARIANT_1,
	LV_MENU_ITEM_BUILDER_VARIANT_2
};

#ifdef __cplusplus
} /* extern "C" */
#endif

/// @brief Приемная сторона шины событий-данных для графического интерфейса
class UiEventObserver : public AbstractEventObserver {
public:
	EventResult handleEvent(Event *e) override
	{
		assert(mutex != nullptr);

		switch (e->type) 
		{
		case EventType::UpdateUpperData: {
			MutexLock lock(*mutex);
			updateUpperData(&e->data.upperData);
			} return EventResult::PASS_ON;

		case EventType::UpdateLowerData: {
			MutexLock lock(*mutex);
			updateLowerData(&e->data.lowerData);
			} return EventResult::PASS_ON;

		case EventType::GetCurrentTime: {
			MutexLock lock(*mutex);
			applyNewCurrentTime(&e->data.time);
			} return EventResult::PASS_ON;

		case EventType::SettingsUpdated: {
			MutexLock lock(*mutex);
			enterParameters(&e->data.settings);
			} return EventResult::PASS_ON;

		case EventType::UpdateDeviceHealth: {
			MutexLock lock(*mutex);
			updateDeviceHealth(e->data.updateHealth.type, e->data.updateHealth.health);
			} return EventResult::PASS_ON;

		default:
			return EventResult::IGNORED;
			break;
		}
	}

	void registerLVGLMutex(SemaphoreHandle_t *aMutex)
	{
		mutex = aMutex;
	}

private:
	SemaphoreHandle_t *mutex{nullptr};
};

void sendParametersToEventBus(Settings *aSettings);
void sendActionCommandToEventBus(Action aAction);
void sendNewTimeToEventBus(Time aTime);
void sendNewBrightnessToEventBus(uint8_t aDuty);
void writeToLoggingPanel(const char *aData, int aSize);
void sendTapSoundToEventBus();

static inline bool isPlaceholder(const char *aText)
{
	return (!strncmp(aText, "", 1));
}

#endif // INCLUDE_UI_HPP_
