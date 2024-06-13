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
#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t lv_menu_builder_variant_t;

void style_initialize();
void menu_create(lv_obj_t *parent);
void actuatorsCreate(lv_obj_t *parent, uint16_t aYOffset);
void main_page_create(lv_obj_t * parent);
void loading_screen_create(lv_obj_t * parent);
void keyboard_create();

lv_obj_t * create_switch(lv_obj_t * parent, const char * icon, const char * txt, bool chk);
lv_obj_t * create_slider(lv_obj_t * parent, const char * icon, const char * txt, int32_t min, int32_t max, int32_t val);
lv_obj_t * create_text(lv_obj_t * parent, const char * icon, const char * txt, lv_menu_builder_variant_t builder_variant);

void createAdditionalPanels();
void settingsButtonEvent(lv_event_t * e);
void uiInit(bool aDarkTheme);

void textAreasReset(uint8_t aArea);
bool textAreasApply(uint8_t aArea);

void updatePanelStyleByFlags(lv_obj_t *aModulePanel, DeviceHealth aHealth);
void updateActuatorByFlags(lv_obj_t *aActuator, bool aDevicePresent, bool aActivated);

void updateSystemData(struct SystemData *aData);
void updateLowerData(struct LowerInternalData *aData);
void updateUpperData(struct UpperInternalData *aData);
void updateAUXData(struct AuxData *aData);

void applyNewCurrentTime(struct Time *aTime);
void enterParameters(struct Settings *aParams);
struct Settings *saveParameters();
bool getLoggingState();
void updateMainPagePumpTypeLabel();
void updateDeviceHealth(struct HealthUpdate *aUpdate);

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
		switch (e->type) 
		{
		case EventType::UpdateUpperData:
			updateUpperData(&e->data.upperData);
			return EventResult::PASS_ON;

		case EventType::UpdateLowerData:
			updateLowerData(&e->data.lowerData);
			return EventResult::PASS_ON;

		case EventType::UpdateSystemData:
			updateSystemData(&e->data.systemData);
			return EventResult::PASS_ON;

		case EventType::GetCurrentTime:
			applyNewCurrentTime(&e->data.time);
			return EventResult::PASS_ON;

		case EventType::SettingsFirstLoad:
			enterParameters(&e->data.settings);
			return EventResult::PASS_ON;

		case EventType::HealthUpdated:
			updateDeviceHealth(&e->data.healthUpdate);
			return EventResult::PASS_ON;

		default:
			return EventResult::IGNORED;
			break;
		}
	}
};

void sendParametersToEventBus(Settings *aSettings);
void sendActionCommandToEventBus(Action aAction);
void sendNewTimeToEventBus(uint8_t hour, uint8_t min, uint8_t sec);
void sendNewBrightnessToEventBus(uint8_t aDuty);

#endif // INCLUDE_UI_HPP_
