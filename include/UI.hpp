/**
 * @file lv_demo_widgets.h
 *
 */

#ifndef LV_HYDROPONIC_H
#define LV_HYDROPONIC_H

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
void uiInit(void);

void textAreasReset(uint8_t aArea);
bool textAreasApply(uint8_t aArea);
void updateMainPage(struct SystemData *aData);
void applyNewCurrentTime(struct CurrentTime *aTime);
void enterParameters(struct Settings *aParams);
struct Settings *saveParameters();

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
        case EventType::UpdateSensorData:
            // Update main page
            
            return EventResult::HANDLED;
            break;
        
        default:
            return EventResult::IGNORED;
            break;
        }
    }
};

#endif /*LV_DEMO_WIDGETS_H*/
