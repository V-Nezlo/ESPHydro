/*!
@file
@brief Сишный код, отвечающий за создание графического интерфейса
@author V-Nezlo (vlladimirka@gmail.com)
@date 17.04.2024
@version 1.0
*/


#ifndef INCLUDE_UI_HPP_
#define INCLUDE_UI_HPP_

#include <lvgl.h>
#include "Types.hpp"
#include "EventBus.hpp"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t lv_menu_builder_variant_t;

void style_initialize();
void menu_create(lv_obj_t *parent);
void main_page_create(lv_obj_t * parent);
void loading_screen_create(lv_obj_t * parent);
void keyboard_create();

lv_obj_t * create_switch(lv_obj_t * parent, const char * icon, const char * txt, bool chk);
lv_obj_t * create_slider(lv_obj_t * parent, const char * icon, const char * txt, int32_t min, int32_t max, int32_t val);
lv_obj_t * create_text(lv_obj_t * parent, const char * icon, const char * txt, lv_menu_builder_variant_t builder_variant);

void create_monitor_tab(lv_obj_t *aParent);
void create_pump_tab(lv_obj_t *aParent);
void create_lamp_tab(lv_obj_t *aParent);
void create_misc_tab(lv_obj_t *aParent);

void createAdditionalPanels();
void settingsButtonEvent(lv_event_t * e);
void uiInit(bool aDarkTheme);

void textAreasReset(uint8_t aArea);
bool textAreasApply(uint8_t aArea);
void updateMainPage(struct SystemData *aData);
void applyNewCurrentTime(struct CurrentTime *aTime);
void enterParameters(struct Settings *aParams);
struct Settings *saveParameters();
bool getLoggingState();
void updateMainPagePumpTypeLabel();

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
            // Update main page
            return EventResult::PASS_ON;
            break;
        case EventType::GetCurrentTime:
            applyNewCurrentTime(&e->data.time);
            return EventResult::PASS_ON;
        case EventType::SettingsFirstLoad:
            enterParameters(&e->data.settings);
            return EventResult::PASS_ON;
            break;
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

#endif /*INCLUDE_UI_HPP_*/
