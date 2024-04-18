/*********************
 *      INCLUDES
 *********************/

#include <EventBus.hpp>
#include "Types.hpp"
#include "UI.hpp"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/**********************
 *      Типы
 **********************/

typedef enum {
	DISP_SMALL,
	DISP_MEDIUM,
	DISP_LARGE,
} disp_size_t;

enum EditScrs { PumpSettingsScrNumber = 1, LampSettingsScrNumber = 2, CurrentTimeSettingsScrNumber = 3 };
enum ManualActionEnum {ManualActionPumpOn = 1, ManualActionPumpOff, ManualActionLampOn, ManualActionLampOff }; 

enum DeviceFlags {
	PumpDeviceWorking = 0x01,
	PumpDeviceWarning = 0x02,
	PumpDeviceError = 0x04,
	PumpDeviceCritical = 0x08,

	LampDeviceWorking = 0x10,
	LampDeviceWarning = 0x20,
	LampDeviceError = 0x40,

	SensorDeviceWorking = 0x80,
	SensorDeviceWarning = 0x100,
	SensorDeviceError = 0x200,

	SystemDeviceWorking = 0x400,
	SystemDeviceWarning = 0x800,
	SystemDeviceError = 0x1000
};

/**********************
 *  Всякое
 **********************/

static disp_size_t disp_size;
static const lv_font_t *font_large;
static const lv_font_t *font_normal;

static lv_style_t style_text_muted;
static lv_style_t style_title;
static lv_style_t style_icon;
static lv_style_t style_bullet;

/**********************
 * Переменные
 **********************/

// Обьекты и указатели
// Экраны
lv_obj_t *mainPage;
lv_obj_t *settingsPage;
lv_obj_t *loadingScreen;
// Доп экраны
lv_obj_t *pumpSettingsScr;
lv_obj_t *lampSettingsScr;
lv_obj_t *curTimeSettingsScr;
// Стили
// Два стиля для главного экрана
lv_style_t style_light;
lv_style_t style_dark;
// Стили для меню делать не будем
lv_style_t style_warning;
lv_style_t style_error;
lv_style_t style_good;
// Темы
lv_theme_t *mainTheme;
// Клавиатура
lv_obj_t *keyboardPanel;
lv_obj_t *keyboardPanelInputTA;
lv_obj_t *pumpKeyboard;
lv_obj_t *lampKeyboard;
lv_obj_t *timeKeyboard;

// Панели
lv_obj_t *panel1;
lv_obj_t *panel2;
lv_obj_t *panel3;
// Панель 1
lv_obj_t *waterLevelPanel;
lv_obj_t *waterLevelLabel;
lv_obj_t *waterMeter;
lv_meter_scale_t *scale;
lv_meter_indicator_t *indic;
lv_obj_t *currentModePanel;
lv_obj_t *currentModeLabel;
// Панель 2
lv_obj_t *currentTimePanel;
lv_obj_t *mainPageTime;
lv_obj_t *mainPagePH;
lv_obj_t *mainPagePPM;
lv_obj_t *mainPageWaterTemp;
lv_obj_t *mainPageWaterLevel;
// Панель 3
lv_obj_t *pumpStatusPanel;
lv_obj_t *lampStatusPanel;
lv_obj_t *sensorsStatusPanel;
lv_obj_t *systemStatusPanel;
// Настройки
lv_obj_t *menu;
lv_color_t bg_color;
lv_obj_t *cont;
lv_obj_t *section;
lv_obj_t *root_page;
lv_obj_t *subPumpPage;
lv_obj_t *subLampPage;
lv_obj_t *subCommonPage;
lv_obj_t *subManualPage;
lv_obj_t *subAboutPage;
lv_obj_t *subExitPage;
// Всякие штуки для настроек
lv_obj_t *pumpEnableButton;
lv_obj_t *pumpOnTa;
lv_obj_t *pumpOffTa;
lv_obj_t *pumpOnCornerText;
lv_obj_t *pumpOffCornerText;
lv_obj_t *pumpTypeDD;
lv_obj_t *pumpSwingTimeBase;
lv_obj_t *pumpSwingTimeSlider;
lv_obj_t *pumpSwingTimeText;
// Настройки лампы
lv_obj_t *lampEnableButton;
// Включение
lv_obj_t *lampOnHourTa;
lv_obj_t *lampOnMinTa;
lv_obj_t *lampOnCornerText;
// Выключение
lv_obj_t *lampOffHourTa;
lv_obj_t *lampOffMinTa;
lv_obj_t *lampOffCornerText;
// Настройки текущего времени
lv_obj_t *setTimeHourTa;
lv_obj_t *setTimeMinTa;
lv_obj_t *setTimeSecTa;
lv_obj_t *setTimeButton;
lv_obj_t *settingsPageTime;
// Общие настройки
lv_obj_t *alarmSoundEnableButton;
lv_obj_t *tapSountEnableButton;
// Штуки для сервиса
lv_obj_t *loggingTextarea;
lv_obj_t *loggingSwitch;
// Энумератор для колбеков
uint8_t editScrSelectorPumpSetttins = PumpSettingsScrNumber;
uint8_t editScrSelectorLampSettings = LampSettingsScrNumber;
uint8_t editScrSelectorSetTime = CurrentTimeSettingsScrNumber;
// Энумератор для панели сервиса
uint8_t manualActionPumpOnEnum = ManualActionPumpOn;
uint8_t manualActionPumpOffEnum = ManualActionPumpOff;
uint8_t manualActionLampOnEnum = ManualActionLampOn;
uint8_t manualActionLampOffEnum = ManualActionLampOff;
// Обьекты для текстового вывода версий и статусов
lv_obj_t *aboutVersionFiller;
lv_obj_t *aboutWifiPresentFiller;
lv_obj_t *aboutMqttPresentFiller;

uint8_t editScrFormattedHourEnum = 1;
uint8_t editScrFormattedMinSecEnum = 2;
uint8_t exitWithSaveButtonCallbackData = 1;
uint8_t exitWithoutSaveButtonCallbackData = 2;
lv_obj_t *activeMessageBox;
// Временный буффер для перевода всяких штук в другие штуки

struct Settings params;
Settings globalSettings;

void sendParametersToEventBus(Settings *aSettings)
{
    Event ev;
    ev.type = EventType::SettingsUpdated;
    ev.data.settings = *aSettings;
    EventBus::throwEvent(&ev);
}

void sendActionCommandToEventBus(Action aAction)
{
    Event ev;
    ev.type = EventType::ActionRequest;
    ev.data.action = aAction;
    EventBus::throwEvent(&ev);
}

void sendNewTimeToEventBus(uint8_t hour, uint8_t min, uint8_t sec)
{
    Event ev;
    ev.type = EventType::SetCurrentTime;
    ev.data.time.currentHour = hour;
    ev.data.time.currentMinutes = min;
    ev.data.time.currentSeconds = sec;
    EventBus::throwEvent(&ev);
}

#ifdef __cplusplus
extern "C" {
#endif

void uiInit(bool aDarkTheme)
{
	// Выбираем размер экрана, у нас будет 320х480
	if (LV_HOR_RES <= 320)
		disp_size = DISP_SMALL;
	else if (LV_HOR_RES < 720)
		disp_size = DISP_MEDIUM;
	else
		disp_size = DISP_LARGE;

	// Выбираем шрифты
	font_large = LV_FONT_DEFAULT;
	font_normal = LV_FONT_DEFAULT;

	if (aDarkTheme) {
		mainTheme = lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_GREEN), lv_palette_main(LV_PALETTE_GREEN),
			LV_THEME_DEFAULT_TRANSITION_TIME, font_large);
	} else {
		mainTheme = lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_LIGHT_GREEN),
			lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, font_large);
	}

	// Стили
	lv_style_init(&style_text_muted);
	lv_style_set_text_opa(&style_text_muted, LV_OPA_50);

	lv_style_init(&style_title);
	lv_style_set_text_font(&style_title, font_large);

	lv_style_init(&style_icon);
	lv_style_set_text_color(&style_icon, lv_theme_get_color_primary(NULL));
	lv_style_set_text_font(&style_icon, font_large);

	lv_style_init(&style_bullet);
	lv_style_set_border_width(&style_bullet, 0);
	lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);

	lv_obj_set_style_text_font(lv_scr_act(), font_large, 0);

	style_initialize();

	// Создаем два экрана, один главный, другой - с настройками
	mainPage = lv_obj_create(NULL);
	settingsPage = lv_obj_create(NULL);

	// Доп экраны для раздельных настроек
	pumpSettingsScr = lv_obj_create(NULL);
	lampSettingsScr = lv_obj_create(NULL);
	curTimeSettingsScr = lv_obj_create(NULL);

	main_page_create(mainPage);
	menu_create(settingsPage);
	keyboard_create();
	createAdditionalPanels();

	// Test filling
	struct SystemData data;
	data.deviceFlags = PumpDeviceWorking | LampDeviceWorking | SensorDeviceWorking | SystemDeviceWorking;
	data.phd10 = 82;
	data.ppm = 1200;
	data.waterLevelProcents = 78;
	data.waterTempd10 = 234;
	updateMainPage(&data);

	params.pump.enabled = true;
	params.pump.onTime = 123;
	params.pump.offTime = 456;
	params.pump.type = 1;
	params.pump.swingTime = 6;
	params.lamp.enabled = false;
	params.lamp.lampOnHour = 9;
	params.lamp.lampOnMin = 12;
	params.lamp.lampOffHour = 21;
	params.lamp.lampOffMin = 8;
	params.common.alarmSoundEnabled = true;
	params.common.tapSoundEnabled = false;
	enterParameters(&params);

	CurrentTime time;
	time.currentHour = 14;
	time.currentMinutes = 55;
	time.currentSeconds = 32;

	lv_scr_load(mainPage);
	applyNewCurrentTime(&time);
}

/**********************
 * Обработчики событий
 **********************/

void manualActionEvent(lv_event_t *aEvent)
{
	const uint8_t *data = reinterpret_cast<uint8_t *>(lv_event_get_user_data(aEvent));
	const uint8_t action = *data;

	if (action == manualActionPumpOnEnum) {
		sendActionCommandToEventBus(Action::TurnPumpOn);
	} else if (action == manualActionPumpOffEnum) {
		sendActionCommandToEventBus(Action::TurnPumpOff);
	} else if (action == manualActionLampOnEnum) {
		sendActionCommandToEventBus(Action::TurnLampOn);
	} else if (action == manualActionLampOffEnum) {
		sendActionCommandToEventBus(Action::TurnLampOff);
	} else {
		return;
	}
}

void customTextAreaEvent(lv_event_t *aEvent)
{
	uint8_t *data = reinterpret_cast<uint8_t *>(lv_event_get_user_data(aEvent));

	switch (*data) {
		case PumpSettingsScrNumber:
			lv_scr_load(pumpSettingsScr);
			break;
		case LampSettingsScrNumber:
			lv_scr_load(lampSettingsScr);
			break;
		case CurrentTimeSettingsScrNumber:
			lv_scr_load(curTimeSettingsScr);
			break;
	}
}

void settingsButtonEvent(lv_event_t *e)
{
	lv_scr_load(settingsPage);
	// Откроет первую вкладку при повторных открытиях меню настройки
	lv_event_send(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0), 0), LV_EVENT_CLICKED, NULL);
}

void msgBoxCallback(lv_event_t *aEvent)
{
	lv_obj_t *obj = lv_event_get_target(aEvent);
	lv_msgbox_close(obj);
}

void pumpSwingTimeEvent(lv_event_t *aEvent)
{
	uint16_t currentValue = lv_slider_get_value(pumpSwingTimeSlider);
	char text[7];
	sprintf(text, "%us", currentValue);
	lv_label_set_text(pumpSwingTimeText, text);
}

void pumpTypeEventHandler(lv_event_t *aEvent)
{
	lv_obj_t *obj = lv_event_get_target(aEvent);
	uint16_t type = lv_dropdown_get_selected(obj);

	switch (type) {
		case 0: // Normal
			lv_obj_add_flag(pumpSwingTimeBase, LV_OBJ_FLAG_HIDDEN);
			break;
		case 1: // Swing
			lv_obj_clear_flag(pumpSwingTimeBase, LV_OBJ_FLAG_HIDDEN);
			break;
		case 2: // Maintance
			lv_obj_add_flag(pumpSwingTimeBase, LV_OBJ_FLAG_HIDDEN);
			break;
		case 3: // Drip
			lv_obj_add_flag(pumpSwingTimeBase, LV_OBJ_FLAG_HIDDEN);
			break;
		default:
			break;
	}
}

void formattedAreaCommonCallback(lv_event_t *aEvent)
{
	lv_obj_t *ta = lv_event_get_target(aEvent);
	uint8_t *editType = reinterpret_cast<uint8_t *>(lv_event_get_user_data(aEvent));

	const char *currentText = lv_textarea_get_text(ta);
	uint8_t representedValue = atoi(currentText);

	// Hour formatter
	if (*editType == 1) {
		if (representedValue > 23) {
			lv_textarea_set_text(ta, "23");
		}
	} else if (*editType == 2) {
		if (representedValue > 59) {
			lv_textarea_set_text(ta, "59");
		}
	}
}

void textAreaCommonCallback(lv_event_t *aEvent)
{
	lv_event_code_t code = lv_event_get_code(aEvent);
	lv_obj_t *ta = lv_event_get_target(aEvent);
	uint8_t *editedSpec = reinterpret_cast<uint8_t *>(lv_event_get_user_data(aEvent));

	lv_obj_t *keyboard = pumpKeyboard;
	if (*editedSpec == EditScrs::PumpSettingsScrNumber) {
		keyboard = pumpKeyboard;
	} else if (*editedSpec == EditScrs::LampSettingsScrNumber) {
		keyboard = lampKeyboard;
	} else if (*editedSpec == EditScrs::CurrentTimeSettingsScrNumber) {
		keyboard = timeKeyboard;
	} else {
		return;
	}

	if (code == LV_EVENT_CLICKED) {
		if (lv_indev_get_type(lv_indev_get_act()) != LV_INDEV_TYPE_KEYPAD) {
			lv_keyboard_set_textarea(keyboard, ta);
			lv_obj_set_style_max_height(keyboard, LV_HOR_RES * 2 / 3, 0);
			lv_obj_update_layout(lv_scr_act()); /*Be sure the sizes are recalculated*/
			lv_obj_set_height(lv_scr_act(), LV_VER_RES - lv_obj_get_height(keyboard));
			lv_obj_scroll_to_view_recursive(ta, LV_ANIM_OFF);
		}
	} else if (code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
		lv_obj_set_height(lv_scr_act(), LV_VER_RES);
		lv_obj_clear_state(ta, LV_STATE_FOCUSED);
		lv_indev_reset(NULL, ta); /*To forget the last clicked object to make it focusable again*/
		// Optional

		// Тут мы сбрасываем данные и возвращаемся
		if (code == LV_EVENT_CANCEL) {
			textAreasReset(*editedSpec);
			lv_scr_load(settingsPage);
		}

		// Тут мы пытаемся применить введенные данные если они валидны
		if (code == LV_EVENT_READY) {
			if (textAreasApply(*editedSpec)) {
				lv_scr_load(settingsPage);
			}
		}
	}
}

void exitButtonEventHandler(lv_event_t *aEvent)
{
	uint8_t *operation = reinterpret_cast<uint8_t *>(lv_event_get_user_data(aEvent));

	if (*operation == 1) {

		auto newSettings = saveParameters();
		sendParametersToEventBus(newSettings);

		// Update pump mode
		switch (lv_dropdown_get_selected(pumpTypeDD)) {
			case 0: // Normal
				lv_label_set_text(currentModeLabel, "EBB-FLOW");
				lv_obj_center(currentModeLabel);
				lv_obj_add_flag(pumpSwingTimeBase, LV_OBJ_FLAG_HIDDEN);
				break;
			case 1: // Swing
				lv_label_set_text(currentModeLabel, "EBB-SWING");
				lv_obj_center(currentModeLabel);
				lv_obj_clear_flag(pumpSwingTimeBase, LV_OBJ_FLAG_HIDDEN);
				break;
			case 2: // Maintance
				lv_label_set_text(currentModeLabel, "MAINTANCE");
				lv_obj_center(currentModeLabel);
				lv_obj_add_flag(pumpSwingTimeBase, LV_OBJ_FLAG_HIDDEN);
				break;
			case 3: // Drip
				lv_label_set_text(currentModeLabel, "DRIP");
				lv_obj_center(currentModeLabel);
				lv_obj_add_flag(pumpSwingTimeBase, LV_OBJ_FLAG_HIDDEN);
			default:
				break;
		}

		activeMessageBox = lv_msgbox_create(NULL, "Parameters applied", "Tap to continue", NULL, false);
	} else {
		enterParameters(&params);
		activeMessageBox = lv_msgbox_create(NULL, "Parameters were not applied", "Tap to continue", NULL, false);
	}

	lv_obj_align(activeMessageBox, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_event_cb(activeMessageBox, msgBoxCallback, LV_EVENT_CLICKED, NULL);

	lv_event_send(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0), 0), LV_EVENT_CLICKED, NULL);
	lv_menu_set_page(menu, NULL);

	lv_scr_load(mainPage);
}

void setTimeButtonEventHandler(lv_event_t *aEvent)
{
	CurrentTime time;
	time.currentHour = atoi(lv_textarea_get_text(setTimeHourTa));
	time.currentMinutes = atoi(lv_textarea_get_text(setTimeMinTa));
	time.currentSeconds = atoi(lv_textarea_get_text(setTimeSecTa));
	(void)time;
}

/********************************
 * Интерфейсы для взаимодействия
 ********************************/

void textAreasReset(uint8_t aArea)
{
	switch (aArea) {
		case PumpSettingsScrNumber:
			lv_textarea_set_text(pumpOnTa, "");
			lv_textarea_set_text(pumpOffTa, "");
			break;
		case LampSettingsScrNumber:
			lv_textarea_set_text(lampOnHourTa, "");
			lv_textarea_set_text(lampOnMinTa, "");
			lv_textarea_set_text(lampOffHourTa, "");
			lv_textarea_set_text(lampOffMinTa, "");
			break;
		case CurrentTimeSettingsScrNumber:
			lv_textarea_set_text(setTimeHourTa, "");
			lv_textarea_set_text(setTimeMinTa, "");
			lv_textarea_set_text(setTimeSecTa, "");
			break;
	}
}

bool textAreasApply(uint8_t aArea)
{
	switch (aArea) {
		case PumpSettingsScrNumber: {
			const char *taTextOn = lv_textarea_get_text(pumpOnTa);
			const char *taTextOff = lv_textarea_get_text(pumpOffTa);

			const uint32_t onTime = atoi(taTextOn);
			const uint32_t offTime = atoi(taTextOff);

			if (onTime != 0 && offTime != 0) {
				globalSettings.pump.onTime = onTime;
				globalSettings.pump.offTime = offTime;

				lv_label_set_text(pumpOnCornerText, taTextOn);
				lv_label_set_text(pumpOffCornerText, taTextOff);
				return true;
			} else {
				return false;
			}
			break;
		}

		case LampSettingsScrNumber: {
			const char *taTextOnHour = lv_textarea_get_text(lampOnHourTa);
			const char *taTextOnMin = lv_textarea_get_text(lampOnMinTa);
			const char *taTextOffHour = lv_textarea_get_text(lampOffHourTa);
			const char *taTextOffMin = lv_textarea_get_text(lampOffMinTa);

			char newData[13];
			sprintf(newData, "%02u:%02u:00", (uint8_t)atoi(taTextOnHour), (uint8_t)atoi(taTextOnMin));
			lv_label_set_text(lampOnCornerText, newData);
			sprintf(newData, "%02u:%02u:00", (uint8_t)atoi(taTextOffHour), (uint8_t)atoi(taTextOffMin));
			lv_label_set_text(lampOffCornerText, newData);

			return true;
			break;
		}
		case CurrentTimeSettingsScrNumber: {
			const char *taTextTimeHour = lv_textarea_get_text(setTimeHourTa);
			const char *taTextTimeMin = lv_textarea_get_text(setTimeMinTa);
			const char *taTextTimeSec = lv_textarea_get_text(setTimeSecTa);

			const uint8_t hour = atoi(taTextTimeHour);
			const uint8_t min = atoi(taTextTimeMin);
			const uint8_t sec = atoi(taTextTimeSec);

			sendNewTimeToEventBus(hour, min, sec);
			break;
		}
		default:
			break;
	}

	return true;
}

void updateMainPage(struct SystemData *aData)
{
	// Сначала пробежимся по флагам

	if (aData->deviceFlags & PumpDeviceWorking) {
		lv_obj_add_style(pumpStatusPanel, &style_good, 0);
	} else if (aData->deviceFlags & PumpDeviceWarning) {
		lv_obj_add_style(pumpStatusPanel, &style_warning, 0);
	} else if (aData->deviceFlags & PumpDeviceError) {
		lv_obj_add_style(pumpStatusPanel, &style_error, 0);
	} else {
		// Stype - critical
	}

	if (aData->deviceFlags & LampDeviceWorking) {
		lv_obj_add_style(lampStatusPanel, &style_good, 0);
	} else if (aData->deviceFlags & LampDeviceWarning) {
		lv_obj_add_style(lampStatusPanel, &style_warning, 0);
	} else if (aData->deviceFlags & LampDeviceError) {
		lv_obj_add_style(lampStatusPanel, &style_error, 0);
	}

	if (aData->deviceFlags & SensorDeviceWorking) {
		lv_obj_add_style(sensorsStatusPanel, &style_good, 0);
	} else if (aData->deviceFlags & SensorDeviceWarning) {
		lv_obj_add_style(sensorsStatusPanel, &style_warning, 0);
	} else if (aData->deviceFlags & SensorDeviceError) {
		lv_obj_add_style(sensorsStatusPanel, &style_error, 0);
	}

	if (aData->deviceFlags & SystemDeviceWorking) {
		lv_obj_add_style(systemStatusPanel, &style_good, 0);
	} else if (aData->deviceFlags & SystemDeviceWarning) {
		lv_obj_add_style(systemStatusPanel, &style_warning, 0);
	} else if (aData->deviceFlags & SystemDeviceError) {
		lv_obj_add_style(systemStatusPanel, &style_error, 0);
	}

	char phData[5];
	sprintf(phData, "%u.%01u", aData->phd10 / 10, aData->phd10 % 10);
	lv_label_set_text(mainPagePH, phData);

	char ppmData[6];
	sprintf(ppmData, "%4u", aData->ppm);
	lv_label_set_text(mainPagePPM, ppmData);

	char tempData[5];
	sprintf(tempData, "%02u.%01u", aData->waterTempd10 / 10, aData->waterTempd10 % 10);
	lv_label_set_text(mainPageWaterTemp, tempData);

	lv_bar_set_value(mainPageWaterLevel, aData->waterLevelProcents, LV_ANIM_OFF);
}

void applyNewCurrentTime(struct CurrentTime *aTime)
{
	// Сначала проверим какой из экранов активен
	lv_obj_t *currentScreen = lv_scr_act();

	if (currentScreen == mainPage || currentScreen == settingsPage) {
		char newTime[19];
		sprintf(newTime, "%02u : %02u : %02u", aTime->currentHour, aTime->currentMinutes, aTime->currentSeconds);

		if (currentScreen == mainPage) {
			lv_label_set_text(mainPageTime, newTime);
		} else if (currentScreen == settingsPage) {
			lv_label_set_text(settingsPageTime, newTime);
		}
	}
}

void enterParameters(struct Settings *aParams)
{
	// Сначала насос
	if (aParams->pump.enabled) {
		lv_obj_add_state(pumpEnableButton, LV_STATE_CHECKED);
	} else {
		lv_obj_clear_state(pumpEnableButton, LV_STATE_CHECKED);
	}

	char pumpOnTimeText[6];
	char pumpOffTimeText[6];
	sprintf(pumpOnTimeText, "%4u", aParams->pump.onTime);
	sprintf(pumpOffTimeText, "%4u", aParams->pump.offTime);

	lv_label_set_text(pumpOnCornerText, pumpOnTimeText);
	lv_label_set_text(pumpOffCornerText, pumpOffTimeText);

	lv_dropdown_set_selected(pumpTypeDD, aParams->pump.type);
	// Вызываю коллбек для DD руками чтобы обновился текст на главной странице
	lv_event_send(pumpTypeDD, LV_EVENT_VALUE_CHANGED, NULL);
	lv_slider_set_value(pumpSwingTimeSlider, aParams->pump.swingTime, LV_ANIM_OFF);
	// Вызываю коллбек руками чтобы прокинуть текст
	lv_event_send(pumpSwingTimeSlider, LV_EVENT_VALUE_CHANGED, NULL);

	// Установим настройки лампы
	if (aParams->lamp.enabled) {
		lv_obj_add_state(lampEnableButton, LV_STATE_CHECKED);
	} else {
		lv_obj_clear_state(pumpEnableButton, LV_STATE_CHECKED);
	}

	char lampOnTime[13];
	char lampOffTime[13];
	sprintf(lampOnTime, "%02u:%02u:00", aParams->lamp.lampOnHour, aParams->lamp.lampOnMin);
	sprintf(lampOffTime, "%02u:%02u:00", aParams->lamp.lampOffHour, aParams->lamp.lampOffMin);

	lv_label_set_text(lampOnCornerText, lampOnTime);
	lv_label_set_text(lampOffCornerText, lampOffTime);

	// Настроим прочие настройки

	if (aParams->common.alarmSoundEnabled) {
		lv_obj_add_state(alarmSoundEnableButton, LV_STATE_CHECKED);
	} else {
		lv_obj_clear_state(alarmSoundEnableButton, LV_STATE_CHECKED);
	}

	if (aParams->common.tapSoundEnabled) {
		lv_obj_add_state(tapSountEnableButton, LV_STATE_CHECKED);
	} else {
		lv_obj_clear_state(tapSountEnableButton, LV_STATE_CHECKED);
	}

	if (aParams->common.loggingEnabled) {
		lv_obj_add_state(loggingSwitch, LV_STATE_CHECKED);
	} else {
		lv_obj_clear_state(loggingSwitch, LV_STATE_CHECKED);
	}
}

struct Settings *saveParameters()
{
	globalSettings.pump.enabled = lv_obj_has_state(pumpEnableButton, LV_STATE_CHECKED);
	globalSettings.pump.onTime = atoi(lv_label_get_text(pumpOnCornerText));
	globalSettings.pump.offTime = atoi(lv_label_get_text(pumpOffCornerText));
	globalSettings.pump.type = lv_dropdown_get_selected(pumpTypeDD);
	globalSettings.pump.swingTime = lv_slider_get_value(pumpSwingTimeSlider);

	const char *lampOnText = lv_label_get_text(lampOnCornerText);
	const char *lampOffText = lv_label_get_text(lampOffCornerText);

	char lampOnHourText[2];
	char lampOnMinText[2];
	char lampOffHourText[2];
	char lampOffMinText[2];

	memcpy(lampOnHourText, lampOnText, 2);
	memcpy(lampOnMinText, &lampOnText[2], 2);
	memcpy(lampOffHourText, lampOffText, 2);
	memcpy(lampOffMinText, &lampOffText[2], 2);

	globalSettings.lamp.enabled = lv_obj_has_state(lampEnableButton, LV_STATE_CHECKED);
	globalSettings.lamp.lampOnHour = atoi(lampOnHourText);
	globalSettings.lamp.lampOnMin = atoi(lampOnMinText);
	globalSettings.lamp.lampOffHour = atoi(lampOffHourText);
	globalSettings.lamp.lampOffMin = atoi(lampOffMinText);

	globalSettings.common.alarmSoundEnabled = lv_obj_has_state(alarmSoundEnableButton, LV_STATE_CHECKED);
	globalSettings.common.tapSoundEnabled = lv_obj_has_state(tapSountEnableButton, LV_STATE_CHECKED);
	globalSettings.common.loggingEnabled = lv_obj_has_state(loggingSwitch, LV_STATE_CHECKED);

	return &globalSettings;
}

bool getLoggingState()
{
	return lv_obj_has_state(loggingSwitch, LV_STATE_CHECKED);
}

/**********************
 * Создание окон
 **********************/

void loading_screen_create(lv_obj_t *parent)
{ }

void keyboard_create()
{
	pumpKeyboard = lv_keyboard_create(pumpSettingsScr);
	lv_keyboard_set_mode(pumpKeyboard, LV_KEYBOARD_MODE_NUMBER);
	lv_obj_set_size(pumpKeyboard, lv_disp_get_hor_res(NULL), 230);

	lampKeyboard = lv_keyboard_create(lampSettingsScr);
	lv_keyboard_set_mode(lampKeyboard, LV_KEYBOARD_MODE_NUMBER);
	lv_obj_set_size(lampKeyboard, lv_disp_get_hor_res(NULL), 230);

	timeKeyboard = lv_keyboard_create(curTimeSettingsScr);
	lv_keyboard_set_mode(timeKeyboard, LV_KEYBOARD_MODE_NUMBER);
	lv_obj_set_size(timeKeyboard, lv_disp_get_hor_res(NULL), 230);
}

void style_initialize()
{
	// Настройка стилей - основной стиль
	lv_style_init(&style_light);
	lv_style_set_bg_color(&style_light, lv_palette_main(LV_PALETTE_LIGHT_GREEN));
	lv_style_set_border_color(&style_light, lv_palette_lighten(LV_PALETTE_LIGHT_GREEN, 3));
	lv_style_set_border_width(&style_light, 0);
	lv_style_set_radius(&style_light, 0);
	lv_style_set_shadow_width(&style_light, 10);
	lv_style_set_shadow_ofs_y(&style_light, 5);
	lv_style_set_shadow_opa(&style_light, LV_OPA_50);
	lv_style_set_text_color(&style_light, lv_color_white());

	// Стиль - warning
	lv_style_init(&style_warning);
	lv_style_set_bg_color(&style_warning, lv_palette_main(LV_PALETTE_YELLOW));
	lv_style_set_border_color(&style_warning, lv_palette_darken(LV_PALETTE_YELLOW, 3));
	lv_style_set_text_color(&style_warning, lv_palette_darken(LV_PALETTE_YELLOW, 4));
	lv_style_set_border_width(&style_warning, 1);
	lv_style_set_radius(&style_warning, 4);

	// Стиль - error
	lv_style_init(&style_error);
	lv_style_set_bg_color(&style_error, lv_palette_main(LV_PALETTE_RED));
	lv_style_set_border_color(&style_error, lv_palette_darken(LV_PALETTE_RED, 3));
	lv_style_set_text_color(&style_error, lv_palette_darken(LV_PALETTE_RED, 4));
	lv_style_set_border_width(&style_error, 1);
	lv_style_set_radius(&style_error, 4);

	// Стиль - good
	lv_style_init(&style_good);
	lv_style_set_bg_color(&style_good, lv_palette_main(LV_PALETTE_GREEN));
	lv_style_set_border_color(&style_good, lv_palette_darken(LV_PALETTE_GREEN, 3));
	lv_style_set_text_color(&style_good, lv_palette_darken(LV_PALETTE_GREEN, 4));
	lv_style_set_border_width(&style_good, 1);
	lv_style_set_radius(&style_good, 4);
}

void main_page_create(lv_obj_t *parent)
{
	static const uint16_t panelW = 155;
	static const uint16_t panelH = 310;
	static const uint16_t miniPanelW = 130;

	// Панель с аркой, уровнем воды и температурой воды
	LV_IMG_DECLARE(HydroTypeExtended);
	panel1 = lv_img_create(parent);
	lv_img_set_src(panel1, &HydroTypeExtended);
	lv_obj_set_size(panel1, panelW, panelH);
	lv_obj_align_to(panel1, parent, LV_ALIGN_LEFT_MID, 5, 0);
	lv_obj_clear_flag(panel1, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
	lv_obj_add_style(panel1, &style_light, 0);

	// Панель с информацией о всех датчиках, времени
	panel2 = lv_obj_create(parent);
	lv_obj_set_size(panel2, panelW, panelH);
	lv_obj_align_to(panel2, parent, LV_ALIGN_CENTER, 0, 0);
	lv_obj_clear_flag(panel2, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
	lv_obj_add_style(panel2, &style_light, 0);

	// Панель с информацией о исполнительных устройствах и кнопка настройки
	panel3 = lv_obj_create(parent);
	lv_obj_set_size(panel3, panelW, panelH);
	lv_obj_align_to(panel3, parent, LV_ALIGN_RIGHT_MID, -4, 0);
	lv_obj_clear_flag(panel3, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
	lv_obj_add_style(panel3, &style_light, 0);

	{ // ******************************************************* ПАНЕЛЬ 1
	  // *******************************************************
		// Отображение текущего режима
		currentModePanel = lv_obj_create(panel1);
		lv_obj_set_size(currentModePanel, miniPanelW, 30);
		lv_obj_align_to(currentModePanel, panel1, LV_ALIGN_TOP_MID, 0, 20);
		lv_obj_clear_flag(currentModePanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		currentModeLabel = lv_label_create(currentModePanel);
		// Текст с режимом
		// TODO
		lv_obj_align_to(currentModeLabel, currentModePanel, LV_ALIGN_TOP_MID, 0, -10);
	}
	{ // ******************************************************* ПАНЕЛЬ 2
	  // *******************************************************
		// Панель для времени
		currentTimePanel = lv_obj_create(panel2);
		lv_obj_set_size(currentTimePanel, miniPanelW, 60);
		lv_obj_align_to(currentTimePanel, panel2, LV_ALIGN_TOP_MID, 0, -10);
		lv_obj_clear_flag(currentTimePanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		// Текст ВРЕМЯ
		lv_obj_t *currentTimeLabel = lv_label_create(currentTimePanel);
		lv_label_set_text_static(currentTimeLabel, "Time");
		lv_obj_align_to(currentTimeLabel, currentTimePanel, LV_ALIGN_TOP_MID, 0, -10);
		// Текст с текущим временем
		mainPageTime = lv_label_create(currentTimePanel);
		lv_label_set_text_static(mainPageTime, "12 : 31 : 46");
		lv_obj_align_to(mainPageTime, currentTimePanel, LV_ALIGN_BOTTOM_MID, 0, 10);
		// Панель для PH
		lv_obj_t *pHPanel = lv_obj_create(panel2);
		lv_obj_set_size(pHPanel, miniPanelW, 30);
		lv_obj_align_to(pHPanel, panel2, LV_ALIGN_TOP_MID, 0, 55);
		lv_obj_clear_flag(pHPanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		// Текст для PH
		lv_obj_t *mainPagePHLabel = lv_label_create(pHPanel);
		lv_label_set_text(mainPagePHLabel, "PH: ");
		lv_obj_align_to(mainPagePHLabel, pHPanel, LV_ALIGN_LEFT_MID, 20, 0);

		mainPagePH = lv_label_create(pHPanel);
		lv_label_set_text_static(mainPagePH, "7.5");
		lv_obj_align_to(mainPagePH, pHPanel, LV_ALIGN_RIGHT_MID, -25, 0);
		// Панель для PPM
		lv_obj_t *ppmPanel = lv_obj_create(panel2);
		lv_obj_set_size(ppmPanel, miniPanelW, 30);
		lv_obj_align_to(ppmPanel, panel2, LV_ALIGN_TOP_MID, 0, 90);
		lv_obj_clear_flag(ppmPanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		// Текст для PPM
		lv_obj_t *mainPagePPMLabel = lv_label_create(ppmPanel);
		lv_label_set_text(mainPagePPMLabel, "PPM: ");
		lv_obj_align_to(mainPagePPMLabel, ppmPanel, LV_ALIGN_LEFT_MID, 10, 0);

		mainPagePPM = lv_label_create(ppmPanel);
		lv_label_set_text_static(mainPagePPM, "1800");
		lv_obj_align_to(mainPagePPM, ppmPanel, LV_ALIGN_RIGHT_MID, -10, 0);
		// Панель для температуры воды
		lv_obj_t *waterTempPanel = lv_obj_create(panel2);
		lv_obj_set_size(waterTempPanel, miniPanelW, 30);
		lv_obj_align_to(waterTempPanel, panel2, LV_ALIGN_TOP_MID, 0, 125);
		lv_obj_clear_flag(waterTempPanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		// Текст для PPM
		lv_obj_t *mainPageWaterTempLabel = lv_label_create(waterTempPanel);
		lv_label_set_text(mainPageWaterTempLabel, "Temp: ");
		lv_obj_align_to(mainPageWaterTempLabel, waterTempPanel, LV_ALIGN_LEFT_MID, 5, 0);

		mainPageWaterTemp = lv_label_create(waterTempPanel);
		lv_label_set_text_static(mainPageWaterTemp, "18.2");
		lv_obj_align_to(mainPageWaterTemp, waterTempPanel, LV_ALIGN_RIGHT_MID, -10, 0);

		// Индикатор уровня воды
		waterLevelPanel = lv_obj_create(panel2);
		lv_obj_set_size(waterLevelPanel, miniPanelW, 60);
		lv_obj_align_to(waterLevelPanel, panel2, LV_ALIGN_BOTTOM_MID, 0, 0);
		lv_obj_clear_flag(waterLevelPanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг

		waterLevelLabel = lv_label_create(waterLevelPanel);
		lv_label_set_text_static(waterLevelLabel, "Water Level");
		lv_obj_align_to(waterLevelLabel, waterLevelPanel, LV_ALIGN_TOP_MID, 0, -10);

		static lv_style_t style_bg;
		static lv_style_t style_indic;

		lv_style_init(&style_bg);
		lv_style_set_border_color(&style_bg, lv_palette_main(LV_PALETTE_GREEN));
		lv_style_set_border_width(&style_bg, 3);
		lv_style_set_pad_all(&style_bg, 5); /*To make the indicator smaller*/
		lv_style_set_radius(&style_bg, 2);

		lv_style_init(&style_indic);
		lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
		lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_GREEN));
		lv_style_set_radius(&style_indic, 2);

		mainPageWaterLevel = lv_bar_create(waterLevelPanel);
		lv_obj_remove_style_all(mainPageWaterLevel); /*To have a clean start*/
		lv_obj_add_style(mainPageWaterLevel, &style_bg, 0);
		lv_obj_add_style(mainPageWaterLevel, &style_indic, LV_PART_INDICATOR);

		lv_obj_set_size(mainPageWaterLevel, panelW - 40, 30);
		lv_obj_center(mainPageWaterLevel);
		lv_obj_align_to(mainPageWaterLevel, waterLevelPanel, LV_ALIGN_BOTTOM_MID, 0, 15);
		lv_bar_set_value(mainPageWaterLevel, 30, LV_ANIM_OFF);
	}
	{ // ******************************************************* ПАНЕЛЬ 3
	  // *******************************************************
		// Панель для насоса с выбором цвета
		pumpStatusPanel = lv_obj_create(panel3);
		lv_obj_set_size(pumpStatusPanel, miniPanelW, 30);
		lv_obj_align_to(pumpStatusPanel, panel3, LV_ALIGN_TOP_MID, 0, -10);
		lv_obj_clear_flag(pumpStatusPanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		lv_obj_add_style(pumpStatusPanel, &style_warning, 0);
		// Текст НАСОС
		lv_obj_t *pumpStatusLabel = lv_label_create(pumpStatusPanel);
		lv_label_set_text_static(pumpStatusLabel, "PUMP");
		lv_obj_align_to(pumpStatusLabel, pumpStatusPanel, LV_ALIGN_TOP_MID, 0, -10);

		// Панель для лампы с выбором цвета
		lampStatusPanel = lv_obj_create(panel3);
		lv_obj_set_size(lampStatusPanel, miniPanelW, 30);
		lv_obj_align_to(lampStatusPanel, panel3, LV_ALIGN_TOP_MID, 0, 25);
		lv_obj_clear_flag(lampStatusPanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		lv_obj_add_style(lampStatusPanel, &style_error, 0);
		// Текст НАСОС
		lv_obj_t *lampStatusLabel = lv_label_create(lampStatusPanel);
		lv_label_set_text_static(lampStatusLabel, "LAMP");
		lv_obj_align_to(lampStatusLabel, lampStatusPanel, LV_ALIGN_TOP_MID, 0, -10);

		// Панель для сенсоров с выбором цвета
		sensorsStatusPanel = lv_obj_create(panel3);
		lv_obj_set_size(sensorsStatusPanel, miniPanelW, 30);
		lv_obj_align_to(sensorsStatusPanel, panel3, LV_ALIGN_TOP_MID, 0, 60);
		lv_obj_clear_flag(sensorsStatusPanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		lv_obj_add_style(sensorsStatusPanel, &style_good, 0);
		// Текст НАСОС
		lv_obj_t *sensorStatusLabel = lv_label_create(sensorsStatusPanel);
		lv_label_set_text_static(sensorStatusLabel, "SENSORS");
		lv_obj_align_to(sensorStatusLabel, sensorsStatusPanel, LV_ALIGN_TOP_MID, 0, -10);

		// Панель для системы с выбором цвета
		systemStatusPanel = lv_obj_create(panel3);
		lv_obj_set_size(systemStatusPanel, miniPanelW, 30);
		lv_obj_align_to(systemStatusPanel, panel3, LV_ALIGN_TOP_MID, 0, 95);
		lv_obj_clear_flag(systemStatusPanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		lv_obj_add_style(systemStatusPanel, &style_good, 0);
		// Текст НАСОС
		lv_obj_t *systemStatusLabel = lv_label_create(systemStatusPanel);
		lv_label_set_text_static(systemStatusLabel, "SYSTEM");
		lv_obj_align_to(systemStatusLabel, systemStatusPanel, LV_ALIGN_TOP_MID, 0, -10);

		// Кнопка с картинкой настроек
		LV_IMG_DECLARE(global_settings);
		lv_obj_t *settingsButton = lv_imgbtn_create(panel3);
		lv_obj_set_size(settingsButton, 128, 128);
		lv_obj_align_to(settingsButton, panel3, LV_ALIGN_BOTTOM_MID, 0, 0);
		lv_imgbtn_set_src(settingsButton, LV_IMGBTN_STATE_RELEASED, &global_settings, NULL, NULL);
		lv_obj_add_event_cb(settingsButton, &settingsButtonEvent, LV_EVENT_CLICKED, NULL);
	}
}

void createAdditionalPanels()
{
	// Панель настройки насоса
	// Панель для установки времени PumpOn
	pumpOnTa = lv_textarea_create(pumpSettingsScr);
	lv_textarea_set_accepted_chars(pumpOnTa, "0123456789");
	lv_textarea_set_max_length(pumpOnTa, 5);
	lv_textarea_set_one_line(pumpOnTa, true);
	lv_textarea_set_text(pumpOnTa, "");
	lv_textarea_set_placeholder_text(pumpOnTa, "123");
	lv_obj_set_size(pumpOnTa, 200, 40);
	lv_obj_add_event_cb(pumpOnTa, textAreaCommonCallback, LV_EVENT_ALL, &editScrSelectorPumpSetttins);

	// Панель для установки времени PumpOff
	pumpOffTa = lv_textarea_create(pumpSettingsScr);
	lv_textarea_set_accepted_chars(pumpOffTa, "0123456789");
	lv_textarea_set_max_length(pumpOffTa, 5);
	lv_textarea_set_one_line(pumpOffTa, true);
	lv_textarea_set_text(pumpOffTa, "");
	lv_textarea_set_placeholder_text(pumpOffTa, "456");
	lv_obj_set_size(pumpOffTa, 200, 40);
	lv_obj_add_event_cb(pumpOffTa, textAreaCommonCallback, LV_EVENT_ALL, &editScrSelectorPumpSetttins);

	lv_obj_t *pumpOnSetText = lv_label_create(pumpSettingsScr);
	lv_label_set_text(pumpOnSetText, "Pump on time: ");
	lv_obj_t *pumpOffSetText = lv_label_create(pumpSettingsScr);
	lv_label_set_text(pumpOffSetText, "Pump off time: ");

	lv_obj_align_to(pumpOnTa, pumpSettingsScr, LV_ALIGN_TOP_RIGHT, -120, 5);
	lv_obj_align_to(pumpOffTa, pumpSettingsScr, LV_ALIGN_TOP_RIGHT, -120, 50);
	lv_obj_align_to(pumpOnSetText, pumpSettingsScr, LV_ALIGN_TOP_LEFT, 10, 15);
	lv_obj_align_to(pumpOffSetText, pumpSettingsScr, LV_ALIGN_TOP_LEFT, 10, 60);

	// Панель с настройкой времени включения и выключения лампы
	lv_obj_t *lampSettingsOnLabel = lv_label_create(lampSettingsScr);
	lv_label_set_text(lampSettingsOnLabel, "Lamp On - HH:MM");
	lv_obj_t *lampSettingsOffLabel = lv_label_create(lampSettingsScr);
	lv_label_set_text(lampSettingsOffLabel, "Lamp Off - HH:MM");

#define CLOCK_SET_TA_WIDHT 45
#define CLOCK_SET_TA_HEIGH 45

	// Панель часов
	lampOnHourTa = lv_textarea_create(lampSettingsScr);
	lv_textarea_set_accepted_chars(lampOnHourTa, "0123456789");
	lv_textarea_set_max_length(lampOnHourTa, 2);
	lv_textarea_set_one_line(lampOnHourTa, true);
	lv_textarea_set_text(lampOnHourTa, "");
	lv_textarea_set_placeholder_text(lampOnHourTa, "00");

	lv_obj_set_size(lampOnHourTa, CLOCK_SET_TA_WIDHT, CLOCK_SET_TA_HEIGH);
	lv_obj_add_event_cb(lampOnHourTa, textAreaCommonCallback, LV_EVENT_ALL, &editScrSelectorLampSettings);
	lv_obj_add_event_cb(lampOnHourTa, formattedAreaCommonCallback, LV_EVENT_VALUE_CHANGED, &editScrFormattedHourEnum);
	// Панель минут
	lampOnMinTa = lv_textarea_create(lampSettingsScr);
	lv_textarea_set_accepted_chars(lampOnMinTa, "0123456789");
	lv_textarea_set_max_length(lampOnMinTa, 2);
	lv_textarea_set_one_line(lampOnMinTa, true);
	lv_textarea_set_text(lampOnMinTa, "");
	lv_textarea_set_placeholder_text(lampOnMinTa, "00");

	lv_obj_set_size(lampOnMinTa, CLOCK_SET_TA_WIDHT, CLOCK_SET_TA_HEIGH);
	lv_obj_add_event_cb(lampOnMinTa, textAreaCommonCallback, LV_EVENT_ALL, &editScrSelectorLampSettings);
	lv_obj_add_event_cb(lampOnMinTa, formattedAreaCommonCallback, LV_EVENT_VALUE_CHANGED, &editScrFormattedMinSecEnum);

	lampOffHourTa = lv_textarea_create(lampSettingsScr);
	lv_textarea_set_accepted_chars(lampOffHourTa, "0123456789");
	lv_textarea_set_max_length(lampOffHourTa, 2);
	lv_textarea_set_one_line(lampOffHourTa, true);
	lv_textarea_set_text(lampOffHourTa, "");
	lv_textarea_set_placeholder_text(lampOffHourTa, "00");

	lv_obj_set_size(lampOffHourTa, CLOCK_SET_TA_WIDHT, CLOCK_SET_TA_HEIGH);
	lv_obj_add_event_cb(lampOffHourTa, textAreaCommonCallback, LV_EVENT_ALL, &editScrSelectorLampSettings);
	lv_obj_add_event_cb(lampOffHourTa, formattedAreaCommonCallback, LV_EVENT_VALUE_CHANGED, &editScrFormattedHourEnum);

	lampOffMinTa = lv_textarea_create(lampSettingsScr);
	lv_textarea_set_accepted_chars(lampOffMinTa, "0123456789");
	lv_textarea_set_max_length(lampOffMinTa, 2);
	lv_textarea_set_one_line(lampOffMinTa, true);
	lv_textarea_set_text(lampOffMinTa, "");
	lv_textarea_set_placeholder_text(lampOffMinTa, "00");

	lv_obj_set_size(lampOffMinTa, CLOCK_SET_TA_WIDHT, CLOCK_SET_TA_HEIGH);
	lv_obj_add_event_cb(lampOffMinTa, textAreaCommonCallback, LV_EVENT_ALL, &editScrSelectorLampSettings);
	lv_obj_add_event_cb(lampOffMinTa, formattedAreaCommonCallback, LV_EVENT_VALUE_CHANGED, &editScrFormattedMinSecEnum);

	// Align для всех элементов
	lv_obj_align_to(lampSettingsOnLabel, lampSettingsScr, LV_ALIGN_TOP_LEFT, 10, 12);
	lv_obj_align_to(lampSettingsOffLabel, lampSettingsScr, LV_ALIGN_TOP_LEFT, 10, 57);
	lv_obj_align_to(lampOnHourTa, lampSettingsScr, LV_ALIGN_TOP_RIGHT, -280, 2);
	lv_obj_align_to(lampOffHourTa, lampSettingsScr, LV_ALIGN_TOP_RIGHT, -280, 47);
	lv_obj_align_to(lampOnMinTa, lampSettingsScr, LV_ALIGN_TOP_RIGHT, -230, 2);
	lv_obj_align_to(lampOffMinTa, lampSettingsScr, LV_ALIGN_TOP_RIGHT, -230, 47);

	// Панель установки времени
	setTimeHourTa = lv_textarea_create(curTimeSettingsScr);
	lv_textarea_set_accepted_chars(setTimeHourTa, "0123456789");
	lv_textarea_set_max_length(setTimeHourTa, 2);
	lv_textarea_set_one_line(setTimeHourTa, true);
	lv_textarea_set_text(setTimeHourTa, "");
	lv_textarea_set_placeholder_text(setTimeHourTa, "00");

	lv_obj_set_size(setTimeHourTa, CLOCK_SET_TA_WIDHT, CLOCK_SET_TA_HEIGH);
	lv_obj_add_event_cb(setTimeHourTa, textAreaCommonCallback, LV_EVENT_ALL, &editScrSelectorSetTime);
	lv_obj_add_event_cb(setTimeHourTa, formattedAreaCommonCallback, LV_EVENT_VALUE_CHANGED, &editScrFormattedHourEnum);

	setTimeMinTa = lv_textarea_create(curTimeSettingsScr);
	lv_textarea_set_accepted_chars(setTimeMinTa, "0123456789");
	lv_textarea_set_max_length(setTimeMinTa, 2);
	lv_textarea_set_one_line(setTimeMinTa, true);
	lv_textarea_set_text(setTimeMinTa, "");
	lv_textarea_set_placeholder_text(setTimeMinTa, "00");

	lv_obj_set_size(setTimeMinTa, CLOCK_SET_TA_WIDHT, CLOCK_SET_TA_HEIGH);
	lv_obj_add_event_cb(setTimeMinTa, textAreaCommonCallback, LV_EVENT_ALL, &editScrSelectorSetTime);
	lv_obj_add_event_cb(setTimeMinTa, formattedAreaCommonCallback, LV_EVENT_VALUE_CHANGED, &editScrFormattedMinSecEnum);

	setTimeSecTa = lv_textarea_create(curTimeSettingsScr);
	lv_textarea_set_accepted_chars(setTimeSecTa, "0123456789");
	lv_textarea_set_max_length(setTimeSecTa, 2);
	lv_textarea_set_one_line(setTimeSecTa, true);
	lv_textarea_set_text(setTimeSecTa, "");
	lv_textarea_set_placeholder_text(setTimeSecTa, "00");

	lv_obj_set_size(setTimeSecTa, CLOCK_SET_TA_WIDHT, CLOCK_SET_TA_HEIGH);
	lv_obj_add_event_cb(setTimeSecTa, textAreaCommonCallback, LV_EVENT_ALL, &editScrSelectorSetTime);
	lv_obj_add_event_cb(setTimeSecTa, formattedAreaCommonCallback, LV_EVENT_VALUE_CHANGED, &editScrFormattedMinSecEnum);

	setTimeButton = lv_btn_create(curTimeSettingsScr);
	lv_obj_set_size(setTimeButton, 120, 40);
	lv_obj_add_event_cb(setTimeButton, setTimeButtonEventHandler, LV_EVENT_CLICKED, NULL);
	lv_obj_t *setTimeButtonLabel = lv_label_create(setTimeButton);
	lv_obj_align_to(setTimeButtonLabel, setTimeButton, LV_ALIGN_LEFT_MID, 0, 0);
	lv_label_set_text(setTimeButtonLabel, "Send to RTC");

	// Алигним
	lv_obj_align_to(setTimeHourTa, curTimeSettingsScr, LV_ALIGN_TOP_LEFT, 20, 20);
	lv_obj_align_to(setTimeMinTa, curTimeSettingsScr, LV_ALIGN_TOP_LEFT, 70, 20);
	lv_obj_align_to(setTimeSecTa, curTimeSettingsScr, LV_ALIGN_TOP_LEFT, 120, 20);
	lv_obj_align_to(setTimeButton, curTimeSettingsScr, LV_ALIGN_TOP_RIGHT, -170, 20);
}

void menu_create(lv_obj_t *parent)
{
	menu = lv_menu_create(parent);
	bg_color = lv_obj_get_style_bg_color(menu, 0);

	if (lv_color_brightness(bg_color) > 127) {
		lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, 0), 10), 0);
	} else {
		lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, 0), 50), 0);
	}

	//Сознательно отключаю кнопку назад, поскольку это будет отдельный пункт меню
	lv_menu_set_mode_root_back_btn(menu, LV_MENU_ROOT_BACK_BTN_DISABLED);
	lv_obj_set_size(menu, lv_disp_get_hor_res(NULL), lv_disp_get_ver_res(NULL));
	lv_obj_center(menu);

	// Создаем субстраницы
	// ********************************МЕНЮ НАСОСА********************************
	subPumpPage = lv_menu_page_create(menu, NULL);
	lv_obj_set_style_pad_hor(subPumpPage, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
	lv_menu_separator_create(subPumpPage);
	section = lv_menu_section_create(subPumpPage);
	// Вкл или выкл насоса
	pumpEnableButton = create_switch(section, LV_SYMBOL_SETTINGS, "Enable", true);

	// Текст - Время включенного состояния насоса
	lv_obj_t *pumpOnBaseText = create_text(section, NULL, "Pump on time", LV_MENU_ITEM_BUILDER_VARIANT_1);
	pumpOnCornerText = lv_label_create(pumpOnBaseText);
	lv_obj_align_to(pumpOnCornerText, pumpOnBaseText, LV_TEXT_ALIGN_RIGHT, 0, 0);
	lv_label_set_text(pumpOnCornerText, "0"); // Вот тут нужно взять проинициализированные данные

	// Текст - Время выключенного состояния насоса
	lv_obj_t *pumpOffBaseText = create_text(section, NULL, "Pump off time", LV_MENU_ITEM_BUILDER_VARIANT_2);
	pumpOffCornerText = lv_label_create(pumpOffBaseText);
	lv_obj_align_to(pumpOffCornerText, pumpOffBaseText, LV_TEXT_ALIGN_RIGHT, 100, 0);
	lv_label_set_text(pumpOffCornerText, "0"); // Вот тут нужно взять проинициализированные данные

	// Кнопка для настройки времени
	lv_obj_t *pumpConfigButton = lv_btn_create(section);
	lv_obj_set_size(pumpConfigButton, 314, 35);
	lv_obj_t *pumpOnButtonLabel = lv_label_create(pumpConfigButton);
	lv_label_set_text(pumpOnButtonLabel, "Configure pump timings");
	lv_obj_align_to(pumpOnButtonLabel, pumpConfigButton, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_event_cb(pumpConfigButton, customTextAreaEvent, LV_EVENT_CLICKED, &editScrSelectorPumpSetttins);

	// Выпадающий список с типами гидропоник
	create_text(section, NULL, "Pump type", LV_MENU_ITEM_BUILDER_VARIANT_1);
	pumpTypeDD = lv_dropdown_create(section);
	lv_dropdown_set_options(pumpTypeDD,
		"Normal\n"
		"Swing\n"
		"Maintance\n"
		"Drip");
	lv_dropdown_set_dir(pumpTypeDD, LV_DIR_RIGHT);

	lv_obj_align(pumpTypeDD, LV_ALIGN_TOP_MID, 0, 20);
	lv_obj_add_event_cb(pumpTypeDD, pumpTypeEventHandler, LV_EVENT_ALL, NULL);

	// Поле - swing время (если режим не swing - неактивно)
	pumpSwingTimeBase = create_text(section, LV_SYMBOL_SETTINGS, "Swing Time", LV_MENU_ITEM_BUILDER_VARIANT_2);
	pumpSwingTimeText = lv_label_create(pumpSwingTimeBase);
	lv_obj_align(pumpSwingTimeText, LV_ALIGN_RIGHT_MID, 0, 0);
	lv_label_set_text(pumpSwingTimeText, "0s ");
	pumpSwingTimeSlider = lv_slider_create(pumpSwingTimeBase);
	lv_obj_set_flex_grow(pumpSwingTimeSlider, 2);
	lv_slider_set_range(pumpSwingTimeSlider, 0, 10);
	lv_obj_set_size(pumpSwingTimeSlider, 20, 10);
	lv_slider_set_value(pumpSwingTimeSlider, 5, LV_ANIM_OFF);
	lv_obj_add_event_cb(pumpSwingTimeSlider, pumpSwingTimeEvent, LV_EVENT_VALUE_CHANGED, NULL);

	// ********************************МЕНЮ ЛАМПЫ********************************
	// Подменю с настройками лампы
	subLampPage = lv_menu_page_create(menu, NULL);
	lv_obj_set_style_pad_hor(subLampPage, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
	lv_menu_separator_create(subLampPage);
	section = lv_menu_section_create(subLampPage);
	// Включение или выключение управления лампой
	lampEnableButton = create_switch(section, LV_SYMBOL_SETTINGS, "Enable", true);
	lv_menu_separator_create(section);

	// Текст - время включения лампы
	lv_obj_t *lampOnBaseText = create_text(section, NULL, "Lamp On Time", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lampOnCornerText = lv_label_create(lampOnBaseText);
	lv_label_set_text(lampOnCornerText, "00:00:00");
	lv_obj_align_to(lampOnCornerText, lampOnBaseText, LV_TEXT_ALIGN_RIGHT, 0, 0);

	lv_obj_t *lampOffBaseText = create_text(section, NULL, "Lamp Off Time", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lampOffCornerText = lv_label_create(lampOffBaseText);
	lv_label_set_text(lampOffCornerText, "00:00:00");
	lv_obj_align_to(lampOffCornerText, lampOffBaseText, LV_TEXT_ALIGN_RIGHT, 0, 0);

	// Поле ввода цифр для включенного состояния
	lv_obj_t *lampSettingsButton = lv_btn_create(section);
	lv_obj_set_size(lampSettingsButton, 314, 35);
	lv_obj_t *lampButtonLabel = lv_label_create(lampSettingsButton);
	lv_label_set_text(lampButtonLabel, "Configure lamp timings");
	lv_obj_align_to(lampButtonLabel, lampSettingsButton, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_event_cb(lampSettingsButton, customTextAreaEvent, LV_EVENT_CLICKED, &editScrSelectorLampSettings);

	// ******************************** МЕНЮ ОБЩИХ НАСТРОЕК **********************************
	subCommonPage = lv_menu_page_create(menu, NULL);
	lv_obj_set_style_pad_hor(subCommonPage, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
	lv_menu_separator_create(subCommonPage);
	section = lv_menu_section_create(subCommonPage);

	// Свитч для включения или выключения звука при тапе
	tapSountEnableButton = create_switch(section, LV_SYMBOL_AUDIO, "Tap sound", false);
	alarmSoundEnableButton = create_switch(section, LV_SYMBOL_AUDIO, "Alarm sound", false);
	lv_menu_separator_create(section);

	// Настройка  текущего времени
	lv_obj_t *setTimeBaseText = create_text(section, NULL, "Current time", LV_MENU_ITEM_BUILDER_VARIANT_1);
	settingsPageTime = lv_label_create(setTimeBaseText);
	lv_label_set_text(settingsPageTime, "00:00:00");
	lv_obj_align_to(settingsPageTime, setTimeBaseText, LV_TEXT_ALIGN_RIGHT, 0, 0);

	// Кнопка для отправки времени в RTC
	lv_obj_t *setTimeButton = lv_btn_create(section);
	lv_obj_set_size(setTimeButton, 314, 35);
	lv_obj_add_event_cb(setTimeButton, customTextAreaEvent, LV_EVENT_PRESSED, &editScrSelectorSetTime);
	lv_obj_align(setTimeButton, LV_ALIGN_CENTER, 0, -40);
	// Надпись на кнопке
	lv_obj_t *setTimeLabel = lv_label_create(setTimeButton);
	lv_label_set_text(setTimeLabel, "Configure current time");
	lv_obj_center(setTimeLabel);

	lv_menu_separator_create(section);
	lv_obj_t *wifiConfigureButton = lv_btn_create(section);
	lv_obj_set_size(wifiConfigureButton, 314, 35);
	lv_obj_add_state(wifiConfigureButton, LV_STATE_DISABLED);

	lv_obj_t *wifiConfigureLabel = lv_label_create(wifiConfigureButton);
	lv_label_set_text(wifiConfigureLabel, "Configure WIFI");
	lv_obj_center(wifiConfigureLabel);

	lv_menu_separator_create(section);
	lv_obj_t *mqttConfigureButton = lv_btn_create(section);
	lv_obj_set_size(mqttConfigureButton, 314, 35);
	lv_obj_add_state(mqttConfigureButton, LV_STATE_DISABLED);

	lv_obj_t *mqttConfigureLabel = lv_label_create(mqttConfigureButton);
	lv_label_set_text(mqttConfigureLabel, "Configure MQTT");
	lv_obj_center(mqttConfigureLabel);

	// ******************************** МЕНЮ ОТЛАДКИ **********************************
	subManualPage = lv_menu_page_create(menu, NULL);
	lv_obj_set_style_pad_hor(subManualPage, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
	lv_menu_separator_create(subManualPage);
	section = lv_menu_section_create(subManualPage);
	lv_obj_clear_flag(section, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг

	static const int16_t containerColumnDescriber[] = {133, 133, LV_GRID_TEMPLATE_LAST};
	static const int16_t containerRowDescriber[] = {35, 35, LV_GRID_TEMPLATE_LAST};
	lv_obj_t *buttonContainer = lv_obj_create(section);
    lv_obj_set_style_grid_column_dsc_array(buttonContainer, containerColumnDescriber, 0);
    lv_obj_set_style_grid_row_dsc_array(buttonContainer, containerRowDescriber, 0);
	lv_obj_set_size(buttonContainer, 315, 120);
	lv_obj_set_layout(buttonContainer, LV_LAYOUT_GRID);
	lv_obj_center(buttonContainer);

	lv_obj_t *manualPumpOnButton = lv_btn_create(buttonContainer);
	lv_obj_t *manualPumpOnButtonLabel = lv_label_create(manualPumpOnButton);
	lv_label_set_text(manualPumpOnButtonLabel, "Pump ON");
	lv_obj_align_to(manualPumpOnButtonLabel, manualPumpOnButton, LV_ALIGN_CENTER, 0, 0);
	lv_obj_center(manualPumpOnButtonLabel);
	lv_obj_add_event_cb(manualPumpOnButton, manualActionEvent, LV_EVENT_CLICKED, &manualActionPumpOnEnum);
	lv_obj_set_grid_cell(manualPumpOnButton, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

	lv_obj_t *manualPumpOffButton = lv_btn_create(buttonContainer);
	lv_obj_t *manualPumpOffButtonLabel = lv_label_create(manualPumpOffButton);
	lv_label_set_text(manualPumpOffButtonLabel, "Pump OFF");
	lv_obj_align_to(manualPumpOffButtonLabel, manualPumpOffButton, LV_ALIGN_CENTER, 0, 0);
	lv_obj_center(manualPumpOffButtonLabel);
	lv_obj_add_event_cb(manualPumpOffButton, manualActionEvent, LV_EVENT_CLICKED, &manualActionPumpOffEnum);
	lv_obj_set_grid_cell(manualPumpOffButton, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);

	lv_obj_t *manualLampOnButton = lv_btn_create(buttonContainer);
	lv_obj_t *manualLampOnButtonLabel = lv_label_create(manualLampOnButton);
	lv_label_set_text(manualLampOnButtonLabel, "Lamp ON");
	lv_obj_align_to(manualLampOnButtonLabel, manualLampOnButton, LV_ALIGN_CENTER, 0, 0);
	lv_obj_center(manualLampOnButtonLabel);
	lv_obj_add_event_cb(manualLampOnButton, manualActionEvent, LV_EVENT_CLICKED, &manualActionLampOnEnum);
	lv_obj_set_grid_cell(manualLampOnButton, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

	lv_obj_t *manualLampOffButton = lv_btn_create(buttonContainer);
	lv_obj_t *manualLampOffButtonLabel = lv_label_create(manualLampOffButton);
	lv_label_set_text(manualLampOffButtonLabel, "Lamp OFF");
	lv_obj_align_to(manualLampOffButtonLabel, manualLampOffButton, LV_ALIGN_CENTER, 0, 0);
	lv_obj_center(manualLampOffButtonLabel);
	lv_obj_add_event_cb(manualLampOffButton, manualActionEvent, LV_EVENT_CLICKED, &manualActionLampOffEnum);
	lv_obj_set_grid_cell(manualLampOffButton, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 1, 1);

	loggingSwitch = create_switch(section, LV_SYMBOL_WARNING, "Logging", false);
	loggingTextarea = lv_label_create(section);
	lv_obj_set_size(loggingTextarea, 315, 125);
	lv_label_set_text(loggingTextarea, "A:N=0,R=1");
	
	// ******************************** МЕНЮ ABOUT **********************************
	subAboutPage = lv_menu_page_create(menu, NULL);
	lv_obj_set_style_pad_hor(subAboutPage, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
	lv_menu_separator_create(subAboutPage);
	section = lv_menu_section_create(subAboutPage);

	lv_obj_t *versionContaiter = create_text(section, NULL, "Version:", LV_MENU_ITEM_BUILDER_VARIANT_1);
	aboutVersionFiller = lv_label_create(versionContaiter);
	lv_label_set_text(aboutVersionFiller, "2.0.2-ff523ed");
	lv_obj_align(aboutVersionFiller, LV_ALIGN_LEFT_MID, 0,0);

	lv_obj_t *wifiPresentContainer = create_text(section, NULL, "Wifi status:", LV_MENU_ITEM_BUILDER_VARIANT_1);
	aboutWifiPresentFiller = lv_label_create(wifiPresentContainer);
	lv_label_set_text(aboutWifiPresentFiller, "UNSUPPORTED");
	lv_obj_align(aboutWifiPresentFiller, LV_ALIGN_LEFT_MID, 0,0);

	lv_obj_t *mqttPresentContainer = create_text(section, NULL, "MQTT status:", LV_MENU_ITEM_BUILDER_VARIANT_1);
	aboutMqttPresentFiller = lv_label_create(mqttPresentContainer);
	lv_label_set_text(aboutMqttPresentFiller, "UNSUPPORTED");
	lv_obj_align(aboutMqttPresentFiller, LV_ALIGN_LEFT_MID, 0,0);

	// ******************************** МЕНЮ ВЫХОДА **********************************
	subExitPage = lv_menu_page_create(menu, NULL);
	lv_obj_set_style_pad_hor(subExitPage, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
	lv_menu_separator_create(subExitPage);
	section = lv_menu_section_create(subExitPage);
	// ***** КНОПКА ВЫХОДА С СОХРАНЕНИЕМ
	lv_obj_t *exitWithSaveButtonContainer = lv_menu_cont_create(section);
	lv_obj_t *exitWithSaveButton = lv_btn_create(exitWithSaveButtonContainer);
	lv_obj_set_size(exitWithSaveButton, 290, 50);
	lv_obj_align(exitWithSaveButton, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_event_cb(exitWithSaveButton, exitButtonEventHandler, LV_EVENT_PRESSED, &exitWithSaveButtonCallbackData);
	// Надпись на кнопке
	lv_obj_t *exitWithSaveLabel = lv_label_create(exitWithSaveButton);
	lv_label_set_text(exitWithSaveLabel, "Save and exit");
	lv_obj_align_to(exitWithSaveLabel, exitWithSaveButton, LV_ALIGN_CENTER, 0, 0);
	// ***** КНОПКА ВЫХОДА БЕЗ СОХРАНЕНИЯ
	lv_obj_t *exitWnoSaveButtonContainer = lv_menu_cont_create(section);
	lv_obj_t *exitWnoSaveButton = lv_btn_create(exitWnoSaveButtonContainer);
	lv_obj_set_size(exitWnoSaveButton, 290, 50);
	lv_obj_align(exitWnoSaveButton, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_event_cb(
		exitWnoSaveButton, exitButtonEventHandler, LV_EVENT_PRESSED, &exitWithoutSaveButtonCallbackData);
	// Надпись на кнопке
	lv_obj_t *exitWnoSaveLabel = lv_label_create(exitWnoSaveButton);
	lv_label_set_text(exitWnoSaveLabel, "Exit without saving");
	lv_obj_center(exitWnoSaveLabel);

	/* ************************************ОБЩЕЕ ДЛЯ СОЗДАНИЯ МЕНЮ *******************************/
	lv_obj_t *root_page = lv_menu_page_create(menu, NULL);
	lv_obj_set_style_pad_hor(root_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
	section = lv_menu_section_create(root_page);
	cont = create_text(section, LV_SYMBOL_TINT, "Pump", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, subPumpPage);
	cont = create_text(section, LV_SYMBOL_UP, "Light", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, subLampPage);
	cont = create_text(section, LV_SYMBOL_SETTINGS, "Common", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, subCommonPage);
	cont = create_text(section, LV_SYMBOL_WARNING, "Service", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, subManualPage);
	cont = create_text(section, LV_SYMBOL_LIST, "About", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, subAboutPage);

	create_text(root_page, NULL, "Others", LV_MENU_ITEM_BUILDER_VARIANT_1);
	section = lv_menu_section_create(root_page);
	cont = create_text(section, LV_SYMBOL_CLOSE, "Exit", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, subExitPage);
	lv_menu_set_sidebar_page(menu, root_page);
	lv_event_send(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0), 0), LV_EVENT_CLICKED, NULL);
}

lv_obj_t *create_text(lv_obj_t *parent, const char *icon, const char *txt, lv_menu_builder_variant_t builder_variant)
{
	lv_obj_t *obj = lv_menu_cont_create(parent);

	lv_obj_t *img = NULL;
	lv_obj_t *label = NULL;

	if (icon) {
		img = lv_img_create(obj);
		lv_img_set_src(img, icon);
	}

	if (txt) {
		label = lv_label_create(obj);
		lv_label_set_text(label, txt);
		lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
		lv_obj_set_flex_grow(label, 1);
	}

	if (builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 && icon && txt) {
		lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
		lv_obj_swap(img, label);
	}

	return obj;
}

lv_obj_t *create_slider(lv_obj_t *parent, const char *icon, const char *txt, int32_t min, int32_t max, int32_t val)
{
	lv_obj_t *obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_2);

	lv_obj_t *slider = lv_slider_create(obj);
	lv_obj_set_flex_grow(slider, 1);
	lv_slider_set_range(slider, min, max);
	lv_slider_set_value(slider, val, LV_ANIM_OFF);

	if (icon == NULL) {
		lv_obj_add_flag(slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
	}

	return obj;
}

lv_obj_t *create_switch(lv_obj_t *parent, const char *icon, const char *txt, bool chk)
{
	lv_obj_t *obj = create_text(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

	lv_obj_t *sw = lv_switch_create(obj);
	lv_obj_add_state(sw, chk ? LV_STATE_CHECKED : 0);

	return sw;
}

#ifdef __cplusplus
} /* extern "C" */
#endif