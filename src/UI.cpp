/*********************
 *      INCLUDES
 *********************/

#include "UITextConsts.hpp"
#include "EventBus.hpp"
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

enum ActuatorStatus {Disabled = 0, Present = 1, Activated = 2};

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
lv_style_t style_menu_panel;
lv_style_t style_menu_subpanel;
lv_style_t style_menu_base;

lv_style_t styleImageHolder;

lv_style_t styleActuator;
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
lv_obj_t *currentModeLabel;
lv_obj_t *hydroTypeImage;

struct {
	lv_obj_t *pump;
	lv_obj_t *lamp;
	lv_obj_t *topLev;
	lv_obj_t *dam;
	lv_obj_t *aux;

	ActuatorStatus pumpStatus{ActuatorStatus::Disabled};
	ActuatorStatus lampStatus{ActuatorStatus::Disabled};
	ActuatorStatus topLevStatus{ActuatorStatus::Disabled};
	ActuatorStatus damStatus{ActuatorStatus::Disabled};
	ActuatorStatus auxStatus{ActuatorStatus::Disabled};

	ActuatorStatus getStatusByActuatorPtr(lv_obj_t *aPtr)
	{
		if (aPtr == pump) {
			return pumpStatus;
		} else if (aPtr == lamp) {
			return lampStatus;
		} else if (aPtr == topLev) {
			return topLevStatus;
		} else if (aPtr == dam) {
			return damStatus;
		} else if (aPtr == aux) {
			return auxStatus;
		} else {
			return ActuatorStatus::Disabled;
		}
	}

	void setActuatorStatus(lv_obj_t *aPtr, ActuatorStatus aNewStatus)
	{
		if (aPtr == pump) {
			pumpStatus = aNewStatus;
		} else if (aPtr == lamp) {
			lampStatus = aNewStatus;
		} else if (aPtr == topLev) {
			topLevStatus = aNewStatus;
		} else if (aPtr == dam) {
			damStatus = aNewStatus;
		} else if (aPtr == aux) {
			auxStatus = aNewStatus;
		} else {
			return;
		}
	}

} actuators;

// Панель 2
lv_obj_t *currentTimePanel;
lv_obj_t *mainPageTime;
lv_obj_t *mainPagePH;
lv_obj_t *mainPagePPM;
lv_obj_t *waterLevelLabel;
lv_obj_t *mainPageWaterTemp;
lv_obj_t *mainPageWaterLev;

// Панель 3
lv_obj_t *lowerStatusPanel;
lv_obj_t *auxStatusPanel;
lv_obj_t *upperStatusPanel;
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
lv_obj_t *sendNewTimeButton;
lv_obj_t *settingsPageTime;
// Общие настройки
lv_obj_t *alarmSoundEnableButton;
lv_obj_t *tapSountEnableButton;
lv_obj_t *brightnessSlider;
// Штуки для сервиса
lv_obj_t *lowerCalibButton;
lv_obj_t *loggingTextarea;
lv_obj_t *loggingSwitch;
// Були для активации расширенного отображения
bool isLowerPresent{false};
bool isUpperPresent{false};
bool isAuxPresent{false};
bool isSystemPresent{false};

uint8_t lowerFlags{0};
uint8_t upperFlags{0};
uint8_t systemFlags{0};
uint8_t auxFlags{0};

// Флаг того что система перешла в рабочий режим
bool initialized = false;
// Обьекты для передачи в event handlers
uint8_t kFormattingHourEnumerator = 1;
uint8_t kFormattingMinSecEnumerator = 2;
uint8_t kExitWithoutSaveButtonEnumerator = 1;
uint8_t kExitWithSaveButtonEnumerator = 2;

// Обьекты для текстового вывода версий и статусов
lv_obj_t *aboutVersionFiller;
lv_obj_t *aboutWifiPresentFiller;
lv_obj_t *aboutMqttPresentFiller;
// Статические буфферы для текста
char phLabelText[5] = "?.?";
char ppmLabelText[6] = "????";
char tempLabelText[5] = "??.?";
char waterLevelLabelText[6] = "??";
char currentTimeLabelText[19] = "?? : ?? : ??";

char settingsPumpOnTimeText[6] = "0";
char settingsPumpOffTimeText[6] = "0";
char settingsPumpSwingTimeText[7] = "0s ";

char settingsLampOnTime[13] = "00:00:00";
char settingsLampOffTime[13] = "00:00:00";

// Соединил четыре окна в одно через замену label'ов
lv_obj_t *deviceDescInfoPanel;

lv_obj_t *deviceDescPanelLabel1;
lv_obj_t *deviceDescPanelLabel2;
lv_obj_t *deviceDescPanelLabel3;
lv_obj_t *deviceDescPanelLabel4;
lv_obj_t *deviceDescPanelLabel5;
lv_obj_t *deviceDescPanelLabel6;

lv_obj_t *deviceDescPanelLed1;
lv_obj_t *deviceDescPanelLed2;
lv_obj_t *deviceDescPanelLed3;
lv_obj_t *deviceDescPanelLed4;
lv_obj_t *deviceDescPanelLed5;
lv_obj_t *deviceDescPanelLed6;

char deviceDescPanelString1[16] = " ";
char deviceDescPanelString2[16] = " ";
char deviceDescPanelString3[16] = " ";
char deviceDescPanelString4[16] = " ";
char deviceDescPanelString5[16] = " ";
char deviceDescPanelString6[16] = " ";

lv_obj_t *activeMessageBox;
// Временный буффер для перевода всяких штук в другие штуки
Settings currentSettings;
// Используемые цвета
static const lv_color_t kMainBGColor = lv_color_hex(0x5F88C0);
static const lv_color_t kLightGreyColor = lv_color_hex(0xD9D9D9);
static const lv_color_t kGreyColor = lv_color_hex(0x8097BA);
static const lv_color_t kYellowColor = lv_color_hex(0xC7D740);
static const lv_color_t kRedColor = lv_color_hex(0xAF4460);
static const lv_color_t kGreenColor = lv_color_hex(0x30C460);
static const lv_color_t kBlueColor = lv_color_hex(0x4FB2D1);
// Картинки типов гидропоники
LV_IMG_DECLARE(Drip);
LV_IMG_DECLARE(EbbDam);
LV_IMG_DECLARE(EbbNormal);
LV_IMG_DECLARE(EbbSwing);
// Картинки актуаторовы
LV_IMG_DECLARE(PumpActuator);
LV_IMG_DECLARE(LampActuator);
LV_IMG_DECLARE(DamActuator);
LV_IMG_DECLARE(FloatActuator);

// Обсервер UI для EventBus
AbstractEventObserver *observer;

void sendParametersToEventBus(Settings * aSettings)
{
	Event ev;
	ev.type = EventType::SettingsUpdated;
	ev.data.settings = *aSettings;
	EventBus::throwEvent(&ev, observer);
}

void sendActionCommandToEventBus(Action aAction)
{
	Event ev;
	ev.type = EventType::ActionRequest;
	ev.data.action = aAction;
	EventBus::throwEvent(&ev, observer);
}

void sendNewTimeToEventBus(Time aTime)
{
	Event ev;
	ev.type = EventType::SetCurrentTime;
	ev.data.time = aTime;
	EventBus::throwEvent(&ev, observer);
}

void sendNewBrightnessToEventBus(uint8_t aDuty)
{
	Event ev;
	ev.type = EventType::NewBrightness;
	ev.data.brightness = aDuty;
	EventBus::throwEvent(&ev, observer);
}

void writeToLoggingPanel(const char *aData, int aSize)
{
	if (getLoggingState() && loggingTextarea != NULL) {
		lv_textarea_add_text(loggingTextarea, aData);
	}
}

void sendTapSoundToEventBus()
{
	Event ev;
	ev.type = EventType::BuzzerSignal;
	ev.data.buzSignal = BuzzerSignal::Short;
	EventBus::throwEvent(&ev, observer);
}

#ifdef __cplusplus
extern "C" {
#endif

void uiInit(UiEventObserver *aObserver)
{
	observer = aObserver;
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
	mainTheme = lv_theme_default_init(NULL, kMainBGColor, kBlueColor, false, font_large);

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

	styleInitialize();

	// Создаем два экрана, один главный, другой - с настройками
	mainPage = lv_obj_create(NULL);
	lv_obj_add_style(mainPage, &style_menu_panel, 0);
	settingsPage = lv_obj_create(NULL);

	// Доп экраны для раздельных настроек
	pumpSettingsScr = lv_obj_create(NULL);
	lampSettingsScr = lv_obj_create(NULL);
	curTimeSettingsScr = lv_obj_create(NULL);
	loadingScreen = lv_obj_create(NULL);

	loadingScreenCreate(loadingScreen);
	mainPageCreate(mainPage);
	menuCreate(settingsPage);
	keyboardCreate();
	createAdditionalPanels();

	lv_scr_load(loadingScreen);
}

void displayMainPage()
{
	// createDescribedWindow(mainPage);
	lv_scr_load(mainPage);
	initialized = true;
}

/**********************
 * Обработчики событий
 **********************/

void customTextAreaEvent(lv_event_t *aEvent)
{
	lv_obj_t *screen = static_cast<lv_obj_t *>(lv_event_get_user_data(aEvent));
	if (screen == pumpSettingsScr) {
		lv_scr_load(pumpSettingsScr);
	} else if (screen == lampSettingsScr) {
		lv_scr_load(lampSettingsScr);
	} else if (screen == curTimeSettingsScr) {
		lv_scr_load(curTimeSettingsScr);
	} else {
		return;
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
	sprintf(settingsPumpSwingTimeText, "%us", currentValue);
	lv_label_set_text_static(pumpSwingTimeText, NULL);
}

void pumpTypeEventHandler(lv_event_t *aEvent)
{
	lv_obj_t *obj = lv_event_get_target(aEvent);
	const uint16_t typeNum = lv_dropdown_get_selected(obj);
	const PumpModes mode = static_cast<PumpModes>(typeNum);

	if (pumpSwingTimeBase != NULL) {
		if (mode == PumpModes::EBBSwing) {
			lv_obj_clear_flag(pumpSwingTimeBase, LV_OBJ_FLAG_HIDDEN);
		} else {
			lv_obj_add_flag(pumpSwingTimeBase, LV_OBJ_FLAG_HIDDEN);
		}
	}

	if (lowerCalibButton != NULL) {
		if (mode == PumpModes::Maintance) {
			lv_obj_clear_state(lowerCalibButton, LV_STATE_DISABLED);
		} else {
			lv_obj_add_state(lowerCalibButton, LV_STATE_DISABLED);
		}
	}
}

void formattedAreaCommonCallback(lv_event_t *aEvent)
{
	lv_obj_t *ta = lv_event_get_target(aEvent);
	uint8_t *editType = static_cast<uint8_t *>(lv_event_get_user_data(aEvent));

	const char *currentText = lv_textarea_get_text(ta);
	uint8_t representedValue = atoi(currentText);

	// Hour formatter
	if (*editType == kFormattingHourEnumerator) {
		if (representedValue > 23) {
			lv_textarea_set_text(ta, "23");
		}
	} else if (*editType == kFormattingMinSecEnumerator) {
		if (representedValue > 59) {
			lv_textarea_set_text(ta, "59");
		}
	} else {
		return;
	}
}

void textAreaCommonCallback(lv_event_t *aEvent)
{
	lv_event_code_t code = lv_event_get_code(aEvent);
	lv_obj_t *ta = lv_event_get_target(aEvent);
	lv_obj_t *screen = static_cast<lv_obj_t *>(lv_event_get_user_data(aEvent));

	lv_obj_t *keyboard = pumpKeyboard;
	if (screen == pumpSettingsScr) {
		keyboard = pumpKeyboard;
	} else if (screen == lampSettingsScr) {
		keyboard = lampKeyboard;
	} else if (screen == curTimeSettingsScr) {
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
			textAreasReset(screen);
			lv_scr_load(settingsPage);
		}

		// Тут мы пытаемся применить введенные данные если они валидны
		if (code == LV_EVENT_READY) {
			if (textAreasApply(screen)) {
				lv_scr_load(settingsPage);
			}
		}
	}
}

void exitButtonEventHandler(lv_event_t *aEvent)
{
	uint8_t *operation = static_cast<uint8_t *>(lv_event_get_user_data(aEvent));

	if (*operation == kExitWithSaveButtonEnumerator) {
		updateMainPagePumpTypeLabel();
		auto newSettings = saveParameters();
		sendParametersToEventBus(newSettings);
	} else if (*operation == kExitWithoutSaveButtonEnumerator) {
		enterParameters(&currentSettings);
	}

	lv_event_send(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0), 0), LV_EVENT_CLICKED, NULL);
	lv_menu_set_page(menu, NULL);

	lv_scr_load(mainPage);
}

void setTimeButtonEventHandler(lv_event_t *aEvent)
{
	const char *textHour = lv_textarea_get_text(setTimeHourTa);
	const char *textMin = lv_textarea_get_text(setTimeMinTa);
	const char *textSec = lv_textarea_get_text(setTimeSecTa);

	if (isPlaceholder(textHour) || isPlaceholder(textMin) || isPlaceholder(textSec)) {
		activeMessageBox = lv_msgbox_create(curTimeSettingsScr, "Error", "Fill all fields!", NULL, false);
		lv_obj_add_event_cb(activeMessageBox, msgBoxCallback, LV_EVENT_CLICKED, NULL);
		lv_obj_align(activeMessageBox, LV_ALIGN_CENTER, 0, 0);
	} else {
		Time time;
		time.hour = atoi(textHour);
		time.minutes = atoi(textMin);
		time.seconds = atoi(textSec);

		sendNewTimeToEventBus(time);
		// Возврат к меню настроек
		lv_scr_load(settingsPage);

		// Сброс текстовых полей после установки времени
		lv_textarea_set_text(setTimeHourTa, "");
		lv_textarea_set_text(setTimeMinTa, "");
		lv_textarea_set_text(setTimeSecTa, "");
	}
}

void brightnessSliderEventHandler(lv_event_t *)
{
	uint8_t newSliderValue = lv_slider_get_value(brightnessSlider);
	sendNewBrightnessToEventBus(newSliderValue);
}

void actuatorPressedEventHandler(lv_event_t *e)
{
	const lv_obj_t * target = lv_event_get_current_target(e);
	const PumpModes mode = static_cast<PumpModes>(currentSettings.pump.mode);

	if (mode != PumpModes::Maintance) {
		return;
	}
	if (target == actuators.pump && isLowerPresent) {
		if (actuators.pumpStatus == ActuatorStatus::Present) {
			sendActionCommandToEventBus(Action::TurnPumpOn);
		} else if (actuators.pumpStatus == ActuatorStatus::Activated) {
			sendActionCommandToEventBus(Action::TurnPumpOff);
		}
	} else if (target == actuators.lamp && isUpperPresent) {
		if (actuators.lampStatus == ActuatorStatus::Present) {
			sendActionCommandToEventBus(Action::TurnLampOn);
		} else if (actuators.pumpStatus == ActuatorStatus::Activated) {
			sendActionCommandToEventBus(Action::TurnLampOff);
		}
	} else if (target == actuators.dam && isUpperPresent) {
		if (actuators.damStatus == ActuatorStatus::Present) {
			sendActionCommandToEventBus(Action::OpenDam);
		} else if (actuators.pumpStatus == ActuatorStatus::Activated) {
			sendActionCommandToEventBus(Action::CloseDam);
		}
	} else if (target == actuators.aux && isAuxPresent) {
		// Nothing
	} else {
		return;
	}
}

void returnToSettingsEventHandler(lv_event_t *)
{
	lv_scr_load(settingsPage);
}

void deviceDetailedInfoOpenCallback(lv_event_t *e)
{
	const lv_obj_t *target = lv_event_get_current_target(e);

	if (target == lowerStatusPanel) {
		if (!isLowerPresent) {
			return;
		}
		// Set text values
		sprintf(deviceDescPanelString1, "%s", kDevDescInfoPumpCur);
		sprintf(deviceDescPanelString2, "%s", kDevDescInfoTempSen);
		sprintf(deviceDescPanelString3, "%s", kDevDescInfoPHSens);
		sprintf(deviceDescPanelString4, "%s", kDevDescInfoPPMSens);
		sprintf(deviceDescPanelString5, "%s", kDevDescInfoWater);
		sprintf(deviceDescPanelString6, "%s", kDevDescInfoCalibration);
		// Update labels
		lv_label_set_text_static(deviceDescPanelLabel1, NULL);
		lv_label_set_text_static(deviceDescPanelLabel2, NULL);
		lv_label_set_text_static(deviceDescPanelLabel3, NULL);
		lv_label_set_text_static(deviceDescPanelLabel4, NULL);
		lv_label_set_text_static(deviceDescPanelLabel5, NULL);
		lv_label_set_text_static(deviceDescPanelLabel6, NULL);
		// Decompose flags for leds
		const lv_color_t led1Color = lowerFlags & LowerFlags::LowerPumpLowCurrentFlag || lowerFlags & LowerFlags::LowerPumpOverCurrentFlag ? kRedColor : kGreenColor;
		const lv_color_t led2Color = lowerFlags & LowerFlags::LowerTempSensorErrorFlag ? kRedColor : kGreenColor;
		const lv_color_t led3Color = lowerFlags & LowerFlags::LowerPHSensorErrorFlag ? kRedColor : kGreenColor;
		const lv_color_t led4Color = lowerFlags & LowerFlags::LowerPPMSensorErrorFlag ? kRedColor : kGreenColor;
		const lv_color_t led5Color = lowerFlags & LowerFlags::LowerNoWaterFlag ? kRedColor : kGreenColor;
		const lv_color_t led6Color = lowerFlags & LowerFlags::Calibration ? kYellowColor : kGreenColor;
		// Set colors and clear hidden flags
		lv_led_set_color(deviceDescPanelLed1, led1Color);
		lv_led_set_color(deviceDescPanelLed2, led2Color);
		lv_led_set_color(deviceDescPanelLed3, led3Color);
		lv_led_set_color(deviceDescPanelLed4, led4Color);
		lv_led_set_color(deviceDescPanelLed5, led5Color);
		lv_led_set_color(deviceDescPanelLed6, led6Color);

		lv_obj_clear_flag(deviceDescPanelLed1, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(deviceDescPanelLed2, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(deviceDescPanelLed3, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(deviceDescPanelLed4, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(deviceDescPanelLed5, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(deviceDescPanelLed6, LV_OBJ_FLAG_HIDDEN);

	} else if (target == upperStatusPanel) {
		if (!isUpperPresent) {
			return;
		}
		// Set text values
		sprintf(deviceDescPanelString1, "%s", kDevDescInfoACStat);
		sprintf(deviceDescPanelString2, "%s", kDevDescInfoFloatLevStat);
		// Update labels
		lv_label_set_text_static(deviceDescPanelLabel1, NULL);
		lv_label_set_text_static(deviceDescPanelLabel2, NULL);
		// Decompose flags for leds
		const lv_color_t led1Color = upperFlags & UpperFlags::UpperPowerError ? kRedColor : kGreenColor;
		const lv_color_t led2Color = upperFlags & UpperFlags::UpperTopWaterLevelStuck ? kRedColor : kGreenColor;
		// Set colors and clear hidden flag
		lv_led_set_color(deviceDescPanelLed1, led1Color);
		lv_led_set_color(deviceDescPanelLed2, led2Color);
		lv_obj_clear_flag(deviceDescPanelLed1, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(deviceDescPanelLed2, LV_OBJ_FLAG_HIDDEN);
	} else if (target == auxStatusPanel) {
		if (!isAuxPresent) {
			return;
		}
	} else if (target == systemStatusPanel) {
		if (!isSystemPresent) {
			return;
		}
		// Set text values
		sprintf(deviceDescPanelString1, "%s", kDevDescInfoRTCErrorText);
		sprintf(deviceDescPanelString2, "%s", kDevDescInfoInternalMemErrorText);
		sprintf(deviceDescPanelString3, "%s", kDevDescInfoRSBusErrorText);
		sprintf(deviceDescPanelString4, "%s", kDevDescInfoPumpNotOperateError);
		sprintf(deviceDescPanelString5, "%s", kDevDescInfoPumpTimingsError);
		// Update labels
		lv_label_set_text_static(deviceDescPanelLabel1, NULL);
		lv_label_set_text_static(deviceDescPanelLabel2, NULL);
		lv_label_set_text_static(deviceDescPanelLabel3, NULL);
		lv_label_set_text_static(deviceDescPanelLabel4, NULL);
		lv_label_set_text_static(deviceDescPanelLabel5, NULL);
		// Decompose flags for leds
		const lv_color_t led1Color = systemFlags & SystemErrors::SystemRTCError ? kRedColor : kGreenColor;
		const lv_color_t led2Color = systemFlags & SystemErrors::SystemInternalMemError ? kRedColor : kGreenColor;
		const lv_color_t led3Color = systemFlags & SystemErrors::SystemRSBusError ? kRedColor : kGreenColor;
		const lv_color_t led4Color = systemFlags & SystemErrors::SystemPumpNotOperate ? kRedColor : kGreenColor;
		const lv_color_t led5Color = systemFlags & SystemErrors::SystemTankNotFloodedInTime ? kRedColor : kGreenColor;
		// Set colors and clear hidden flag
		lv_led_set_color(deviceDescPanelLed1, led1Color);
		lv_led_set_color(deviceDescPanelLed2, led2Color);
		lv_led_set_color(deviceDescPanelLed3, led3Color);
		lv_led_set_color(deviceDescPanelLed4, led4Color);
		lv_led_set_color(deviceDescPanelLed5, led5Color);

		lv_obj_clear_flag(deviceDescPanelLed1, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(deviceDescPanelLed2, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(deviceDescPanelLed3, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(deviceDescPanelLed4, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(deviceDescPanelLed5, LV_OBJ_FLAG_HIDDEN);
	}

	lv_obj_clear_flag(deviceDescInfoPanel, LV_OBJ_FLAG_HIDDEN);
}

void deviceDetailedInfoCloseCallback(lv_event_t *e)
{
	// Set text values
	sprintf(deviceDescPanelString1, "%s", kDevDescInfoEmpty);
	sprintf(deviceDescPanelString2, "%s", kDevDescInfoEmpty);
	sprintf(deviceDescPanelString3, "%s", kDevDescInfoEmpty);
	sprintf(deviceDescPanelString4, "%s", kDevDescInfoEmpty);
	sprintf(deviceDescPanelString5, "%s", kDevDescInfoEmpty);

	// Update labels
	lv_label_set_text_static(deviceDescPanelLabel1, NULL);
	lv_label_set_text_static(deviceDescPanelLabel2, NULL);
	lv_label_set_text_static(deviceDescPanelLabel3, NULL);
	lv_label_set_text_static(deviceDescPanelLabel4, NULL);
	lv_label_set_text_static(deviceDescPanelLabel5, NULL);

	lv_obj_add_flag(deviceDescPanelLed1, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(deviceDescPanelLed2, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(deviceDescPanelLed3, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(deviceDescPanelLed4, LV_OBJ_FLAG_HIDDEN);
	lv_obj_add_flag(deviceDescPanelLed5, LV_OBJ_FLAG_HIDDEN);

	lv_obj_add_flag(deviceDescInfoPanel, LV_OBJ_FLAG_HIDDEN);
}

void serviceEventHandler(lv_event_t *aEv)
{
	lv_obj_t *caller = lv_event_get_current_target(aEv);
	if (caller == lowerCalibButton) {
		sendActionCommandToEventBus(Action::ECCalibSens);
	}
}

/********************************
 * Интерфейсы для взаимодействия
 ********************************/

void textAreasReset(lv_obj_t *aScreen)
{
	if (aScreen == pumpSettingsScr) {
		lv_textarea_set_text(pumpOnTa, "");
		lv_textarea_set_text(pumpOffTa, "");
	} else if (aScreen == lampSettingsScr) {
		lv_textarea_set_text(lampOnHourTa, "");
		lv_textarea_set_text(lampOnMinTa, "");
		lv_textarea_set_text(lampOffHourTa, "");
		lv_textarea_set_text(lampOffMinTa, "");
	} else if (aScreen == curTimeSettingsScr) {
		lv_textarea_set_text(setTimeHourTa, "");
		lv_textarea_set_text(setTimeMinTa, "");
		lv_textarea_set_text(setTimeSecTa, "");
	}
}

bool textAreasApply(lv_obj_t *aScreen)
{
	if (aScreen == pumpSettingsScr) {
		const char *taTextOn = lv_textarea_get_text(pumpOnTa);
		const char *taTextOff = lv_textarea_get_text(pumpOffTa);

		sprintf(settingsPumpOnTimeText, "%s", taTextOn);
		sprintf(settingsPumpOffTimeText, "%s", taTextOff);

		const uint32_t onTime = atoi(taTextOn);
		const uint32_t offTime = atoi(taTextOff);

		if (onTime != 0 && offTime != 0) {
			lv_label_set_text_static(pumpOnCornerText, NULL);
			lv_label_set_text_static(pumpOffCornerText, NULL);
			return true;
		} else {
			return false;
		}
	} else if (aScreen == lampSettingsScr) {
		const char *taTextOnHour = lv_textarea_get_text(lampOnHourTa);
		const char *taTextOnMin = lv_textarea_get_text(lampOnMinTa);
		const char *taTextOffHour = lv_textarea_get_text(lampOffHourTa);
		const char *taTextOffMin = lv_textarea_get_text(lampOffMinTa);

		sprintf(settingsLampOnTime, "%02u:%02u:00", (uint8_t)atoi(taTextOnHour), (uint8_t)atoi(taTextOnMin));
		lv_label_set_text_static(lampOnCornerText, NULL);
		sprintf(settingsLampOffTime, "%02u:%02u:00", (uint8_t)atoi(taTextOffHour), (uint8_t)atoi(taTextOffMin));
		lv_label_set_text_static(lampOffCornerText, NULL);

		return true;
	} else if (aScreen == curTimeSettingsScr) {
		return true;
	} 

	return false;
}

void updatePanelStyleByFlags(lv_obj_t *aModulePanel, DeviceHealth aHealth)
{
	if (aHealth == DeviceHealth::DeviceWorking) {
		lv_obj_set_style_bg_color(aModulePanel, kGreenColor, 0);
	} else if (aHealth == DeviceHealth::DeviceWarning) {
		lv_obj_set_style_bg_color(aModulePanel, kYellowColor, 0);
	} else if (aHealth == DeviceHealth::DeviceError) {
		lv_obj_set_style_bg_color(aModulePanel, kRedColor, 0);
	} else if (aHealth == DeviceHealth::DeviceCritical) {
		lv_obj_set_style_bg_color(aModulePanel, kRedColor, 0);
	} else if (aHealth == DeviceHealth::DeviceDisabled) {
		lv_obj_set_style_bg_color(aModulePanel, kGreyColor, 0);
	}
}

void updateActuatorByFlags(lv_obj_t *aActuator, bool aDevicePresent, bool aActivated)
{
	ActuatorStatus oldStatus = actuators.getStatusByActuatorPtr(aActuator);
	ActuatorStatus newStatus = aActivated ? ActuatorStatus::Activated : ActuatorStatus::Present;

	if (oldStatus != newStatus) {
		if (!aDevicePresent) {
			lv_obj_set_style_bg_color(aActuator, kRedColor, 0);
		} else if (aActivated) {
			lv_obj_set_style_bg_color(aActuator, kBlueColor, 0);
		} else {
			lv_obj_set_style_bg_color(aActuator, kGreenColor, 0);
		}
	}

	actuators.setActuatorStatus(aActuator, newStatus);
}

void updateSystemData(struct SystemData *aData)
{
	systemFlags = aData->flags;
}

void updateLowerData(const struct LowerInternalData *aData)
{
	updateActuatorByFlags(actuators.pump, isLowerPresent, aData->pumpState);
	lowerFlags = aData->flags;

	sprintf(ppmLabelText, "%4u", aData->ppm);
	sprintf(tempLabelText, "%02u.%01u", aData->waterTemp10 / 10, aData->waterTemp10 % 10);
	sprintf(waterLevelLabelText, "%3u %%", aData->waterLevel);
	sprintf(phLabelText, "%u.%01u", aData->ph10 / 10, aData->ph10 % 10);

	// Сигнал для обновления текстов
	lv_label_set_text_static(mainPageWaterTemp, NULL);
	lv_label_set_text_static(mainPagePPM, NULL);
	lv_label_set_text_static(mainPagePH, NULL);
	lv_label_set_text_static(mainPageWaterLev, NULL);
}

void updateAUXData(struct AuxData *aData)
{
	auxFlags = aData->flags;
}

void updateUpperData(struct UpperInternalData *aData)
{
	updateActuatorByFlags(actuators.lamp, isUpperPresent, aData->lampState);
	updateActuatorByFlags(actuators.dam, isUpperPresent, aData->damState);
	updateActuatorByFlags(actuators.topLev, isUpperPresent, aData->swingLevelState);
	upperFlags = aData->flags;
}

void applyNewCurrentTime(struct Time *aTime)
{
	// Сначала проверим какой из экранов активен
	lv_obj_t *currentScreen = lv_scr_act();

	if (currentScreen == mainPage || currentScreen == settingsPage) {
		sprintf(currentTimeLabelText, "%02u : %02u : %02u", aTime->hour, aTime->minutes, aTime->seconds);

		if (currentScreen == mainPage) {
			lv_label_set_text_static(mainPageTime, NULL);
		} else if (currentScreen == settingsPage) {
			lv_label_set_text_static(settingsPageTime, NULL);
		}
	}
}

void enterParameters(struct Settings *aParams)
{
	// Установим новые параметры в системе
	currentSettings = *aParams;
	// А потом установим все элементы в соответствующее состояние
	// Сначала насос
	if (aParams->pump.enabled) {
		lv_obj_add_state(pumpEnableButton, LV_STATE_CHECKED);
	} else {
		lv_obj_clear_state(pumpEnableButton, LV_STATE_CHECKED);
	}

	sprintf(settingsPumpOnTimeText, "%4u", aParams->pump.onTime);
	sprintf(settingsPumpOffTimeText, "%4u", aParams->pump.offTime);

	lv_label_set_text_static(pumpOnCornerText, NULL);
	lv_label_set_text_static(pumpOffCornerText, NULL);

	lv_dropdown_set_selected(pumpTypeDD, static_cast<uint8_t>(aParams->pump.mode));
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

	sprintf(settingsLampOnTime, "%02u:%02u:00", aParams->lamp.lampOnHour, aParams->lamp.lampOnMin);
	sprintf(settingsLampOffTime, "%02u:%02u:00", aParams->lamp.lampOffHour, aParams->lamp.lampOffMin);

	lv_label_set_text_static(lampOnCornerText, NULL);
	lv_label_set_text_static(lampOffCornerText, NULL);

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

	// Установим новое значение яркости
	lv_slider_set_value(brightnessSlider, aParams->common.displayBrightness, LV_ANIM_OFF);
	lv_event_send(brightnessSlider, LV_EVENT_VALUE_CHANGED, NULL);

	// В конце обновим главную страницу
	updateMainPagePumpTypeLabel();
}

struct Settings *saveParameters()
{
	currentSettings.pump.enabled = lv_obj_has_state(pumpEnableButton, LV_STATE_CHECKED);
	currentSettings.pump.onTime = atoi(lv_label_get_text(pumpOnCornerText));
	currentSettings.pump.offTime = atoi(lv_label_get_text(pumpOffCornerText));
	currentSettings.pump.mode = static_cast<PumpModes>(lv_dropdown_get_selected(pumpTypeDD));

	currentSettings.pump.swingTime = lv_slider_get_value(pumpSwingTimeSlider);

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

	currentSettings.lamp.enabled = lv_obj_has_state(lampEnableButton, LV_STATE_CHECKED);
	currentSettings.lamp.lampOnHour = atoi(lampOnHourText);
	currentSettings.lamp.lampOnMin = atoi(lampOnMinText);
	currentSettings.lamp.lampOffHour = atoi(lampOffHourText);
	currentSettings.lamp.lampOffMin = atoi(lampOffMinText);

	currentSettings.common.alarmSoundEnabled = lv_obj_has_state(alarmSoundEnableButton, LV_STATE_CHECKED);
	currentSettings.common.tapSoundEnabled = lv_obj_has_state(tapSountEnableButton, LV_STATE_CHECKED);
	currentSettings.common.loggingEnabled = lv_obj_has_state(loggingSwitch, LV_STATE_CHECKED);
	currentSettings.common.displayBrightness = lv_slider_get_value(brightnessSlider);

	return &currentSettings;
}

bool getLoggingState()
{
	return lv_obj_has_state(loggingSwitch, LV_STATE_CHECKED);
}

void updateMainPagePumpTypeLabel()
{
	// Update pump mode
	const uint8_t newModeNumber = lv_dropdown_get_selected(pumpTypeDD);
	const PumpModes newMode = static_cast<PumpModes>(newModeNumber);
	const PumpModes oldMode = currentSettings.pump.mode;

	if (initialized && (newMode == oldMode)) {
		return;
	}

	switch (newMode) {
		case PumpModes::EBBNormal: // Normal
			lv_label_set_text_static(currentModeLabel, "EBB-FLOW");
			lv_obj_center(currentModeLabel);
			lv_obj_add_flag(pumpSwingTimeBase, LV_OBJ_FLAG_HIDDEN);

			lv_img_set_src(hydroTypeImage, &EbbNormal);
			lv_obj_align(hydroTypeImage, LV_ALIGN_CENTER, 0, 0);
			break;
		case PumpModes::EBBSwing: // Swing
			lv_label_set_text_static(currentModeLabel, "EBB-SWING");
			lv_obj_center(currentModeLabel);
			lv_obj_clear_flag(pumpSwingTimeBase, LV_OBJ_FLAG_HIDDEN);

			lv_img_set_src(hydroTypeImage, &EbbSwing);
			lv_obj_align(hydroTypeImage, LV_ALIGN_CENTER, 0, 0);
			break;
		case PumpModes::Maintance: // Maintance
			lv_label_set_text_static(currentModeLabel, "MAINTANCE");
			lv_obj_center(currentModeLabel);
			lv_obj_add_flag(pumpSwingTimeBase, LV_OBJ_FLAG_HIDDEN);

			lv_img_set_src(hydroTypeImage, &EbbNormal);
			lv_obj_align(hydroTypeImage, LV_ALIGN_CENTER, 0, 0);
			break;
		case PumpModes::Dripping: // Drip
			lv_label_set_text_static(currentModeLabel, "DRIP");
			lv_obj_center(currentModeLabel);
			lv_obj_add_flag(pumpSwingTimeBase, LV_OBJ_FLAG_HIDDEN);

			lv_img_set_src(hydroTypeImage, &Drip);
			lv_obj_align(hydroTypeImage, LV_ALIGN_CENTER, 0, 0);
			break;
		case PumpModes::EBBDam: // DAM
			lv_label_set_text_static(currentModeLabel, "EBB-DAM");
			lv_obj_center(currentModeLabel);
			lv_obj_add_flag(pumpSwingTimeBase, LV_OBJ_FLAG_HIDDEN);

			lv_img_set_src(hydroTypeImage, &EbbDam);
			lv_obj_align(hydroTypeImage, LV_ALIGN_CENTER, 0, 0);
			break;
		default:
			break;
	}
}

void updateDeviceHealth(struct HealthUpdate *aUpdate)
{
	switch(aUpdate->type) {
		case DeviceType::Lower:
			updatePanelStyleByFlags(lowerStatusPanel, aUpdate->health);

			if (aUpdate->health == DeviceHealth::DeviceDisabled) {
				isLowerPresent = false;
				actuators.pumpStatus = ActuatorStatus::Present;
			} else {
				isLowerPresent = true;
			}
			break;
		case DeviceType::Upper:
			updatePanelStyleByFlags(upperStatusPanel, aUpdate->health);

			if (aUpdate->health == DeviceHealth::DeviceDisabled) {
				isUpperPresent = false;
			} else {
				isUpperPresent = true;
			}
			break;
		case DeviceType::AUX:
			updatePanelStyleByFlags(auxStatusPanel, aUpdate->health);

			if (aUpdate->health == DeviceHealth::DeviceDisabled) {
				isAuxPresent = false;
			} else {
				isAuxPresent = true;
			}
			break;
		case DeviceType::Master:
			updatePanelStyleByFlags(systemStatusPanel, aUpdate->health);

			if (aUpdate->health == DeviceHealth::DeviceDisabled) {
				isSystemPresent = false;
			} else {
				isSystemPresent = true;
			}
			break;
		default:
			break;
	}
}

void processTap(lv_event_t *e)
{
	if (currentSettings.common.tapSoundEnabled) {
		sendTapSoundToEventBus();
	}
}

void fillDevicePlaceholders(DeviceType aDevice)
{
	switch (aDevice) {
		case DeviceType::Lower:
			sprintf(phLabelText, "?.?");
			sprintf(ppmLabelText, "????");
			sprintf(tempLabelText, "??.?");
			sprintf(waterLevelLabelText, "??");

			// Сигнал для обновления текстов
			lv_label_set_text_static(mainPageWaterTemp, NULL);
			lv_label_set_text_static(mainPagePPM, NULL);
			lv_label_set_text_static(mainPagePH, NULL);
			lv_label_set_text_static(mainPageWaterLev, NULL);
			break;
		default:
			break;
	}
}

/**********************
 * Создание окон
 **********************/

void loadingScreenCreate(lv_obj_t *parent)
{
	lv_obj_t *firstString = lv_label_create(parent);
	lv_obj_t *secondString = lv_label_create(parent);
	lv_obj_t *thirdString = lv_label_create(parent);

	lv_label_set_text_static(firstString, "ESPHydro");
	lv_label_set_text_static(secondString, AUTO_VERSION);
	lv_label_set_text_static(thirdString, "Copyright: V-Nezlo (vlladimirka@gmail.com)");

	lv_obj_align_to(firstString, parent, LV_ALIGN_CENTER, 0, -20);
	lv_obj_align_to(secondString, parent, LV_ALIGN_CENTER, 0, 0);
	lv_obj_align_to(thirdString, parent, LV_ALIGN_CENTER, 0, +20);
}

void keyboardCreate()
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

void styleInitialize()
{
	// Настройка стилей - основной стиль
	lv_style_init(&style_menu_panel);
	lv_style_set_bg_color(&style_menu_panel, kMainBGColor);
	lv_style_set_border_color(&style_menu_panel, kMainBGColor);
	lv_style_set_border_width(&style_menu_panel, 0);
	lv_style_set_radius(&style_menu_panel, 0);
	lv_style_set_shadow_width(&style_menu_panel, 0);
	lv_style_set_shadow_ofs_y(&style_menu_panel, 0);
	lv_style_set_shadow_opa(&style_menu_panel, LV_OPA_20);
	lv_style_set_text_color(&style_menu_panel, lv_color_black());

	// Стили субпанелей
	lv_style_init(&style_menu_subpanel);
	lv_style_set_bg_color(&style_menu_subpanel, kLightGreyColor);
	lv_style_set_border_color(&style_menu_subpanel, kLightGreyColor);
	lv_style_set_border_width(&style_menu_subpanel, 0);
	lv_style_set_radius(&style_menu_subpanel, 3);
	lv_style_set_shadow_width(&style_menu_subpanel, 4);
	lv_style_set_shadow_ofs_y(&style_menu_subpanel, 4);
	lv_style_set_shadow_opa(&style_menu_subpanel, LV_OPA_20);
	lv_style_set_text_color(&style_menu_subpanel, lv_color_black());

	// Базовый стиль
	lv_style_init(&style_menu_base);
	lv_style_set_bg_color(&style_menu_base, kGreyColor);
	lv_style_set_border_color(&style_menu_base, kGreyColor);
	lv_style_set_border_width(&style_menu_base, 0);
	lv_style_set_radius(&style_menu_base, 3);
	lv_style_set_shadow_width(&style_menu_base, 4);
	lv_style_set_shadow_ofs_y(&style_menu_base, 4);
	lv_style_set_shadow_opa(&style_menu_base, LV_OPA_20);
	lv_style_set_text_color(&style_menu_base, lv_color_black());

	// Стиль актуаторов
	lv_style_init(&styleActuator);
	lv_style_set_bg_color(&styleActuator, kGreenColor);
	lv_style_set_border_color(&styleActuator, kGreenColor);
	lv_style_set_border_width(&styleActuator, 0);
	lv_style_set_radius(&styleActuator, 3);
	lv_style_set_shadow_width(&styleActuator, 4);
	lv_style_set_shadow_ofs_y(&styleActuator, 4);
	lv_style_set_shadow_opa(&styleActuator, LV_OPA_20);
	lv_style_set_text_color(&styleActuator, lv_color_black());

	// Стиль - хранитель картинок
	lv_style_init(&styleImageHolder);
	lv_style_set_bg_color(&styleImageHolder, kLightGreyColor);
	lv_style_set_border_color(&styleImageHolder, kLightGreyColor);
	lv_style_set_border_width(&styleImageHolder, 0);
	lv_style_set_radius(&styleImageHolder, 15);
	lv_style_set_shadow_width(&styleImageHolder, 0);
	lv_style_set_shadow_ofs_y(&styleImageHolder, 0);
	lv_style_set_shadow_opa(&styleImageHolder, LV_OPA_20);
	lv_style_set_text_color(&styleImageHolder, lv_color_black());
}

void actuatorsCreate(lv_obj_t *parent, uint16_t aYOffset)
{
	static constexpr uint8_t kPanelSize = 36;

	actuators.pump = lv_obj_create(parent);
	lv_obj_align(actuators.pump, LV_ALIGN_CENTER, 0, aYOffset);
	lv_obj_set_size(actuators.pump, kPanelSize, kPanelSize);
	lv_obj_clear_flag(actuators.pump, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_style(actuators.pump, &style_menu_base, 0);

	lv_obj_t *pumpActuatorImage = lv_img_create(actuators.pump);
	lv_img_set_src(pumpActuatorImage, &PumpActuator);
	lv_obj_center(pumpActuatorImage);

	lv_obj_add_event_cb(actuators.pump, actuatorPressedEventHandler, LV_EVENT_CLICKED, NULL);

	actuators.lamp = lv_obj_create(parent);
	lv_obj_align(actuators.lamp, LV_ALIGN_CENTER, 46, aYOffset);
	lv_obj_set_size(actuators.lamp, kPanelSize, kPanelSize);
	lv_obj_clear_flag(actuators.lamp, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_style(actuators.lamp, &style_menu_base, 0);

	lv_obj_t *lampActuatorImage = lv_img_create(actuators.lamp);
	lv_img_set_src(lampActuatorImage, &LampActuator);
	lv_obj_center(lampActuatorImage);

	lv_obj_add_event_cb(actuators.lamp, actuatorPressedEventHandler, LV_EVENT_CLICKED, NULL);

	actuators.topLev = lv_obj_create(parent);
	lv_obj_align(actuators.topLev, LV_ALIGN_CENTER, -46, aYOffset);
	lv_obj_set_size(actuators.topLev, kPanelSize, kPanelSize);
	lv_obj_clear_flag(actuators.topLev, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_style(actuators.topLev, &style_menu_base, 0);

	lv_obj_t *floatActuatorImage = lv_img_create(actuators.topLev);
	lv_img_set_src(floatActuatorImage, &FloatActuator);
	lv_obj_center(floatActuatorImage);

	lv_obj_add_event_cb(actuators.topLev, actuatorPressedEventHandler, LV_EVENT_CLICKED, NULL);

	actuators.dam = lv_obj_create(parent);
	lv_obj_align(actuators.dam, LV_ALIGN_CENTER, -46, aYOffset + 46);
	lv_obj_set_size(actuators.dam, kPanelSize, kPanelSize);
	lv_obj_clear_flag(actuators.dam, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_style(actuators.dam, &style_menu_base, 0);

	lv_obj_t *damActuatorImage = lv_img_create(actuators.dam);
	lv_img_set_src(damActuatorImage, &DamActuator);
	lv_obj_center(damActuatorImage);

	lv_obj_add_event_cb(actuators.dam, actuatorPressedEventHandler, LV_EVENT_CLICKED, NULL);

	actuators.aux = lv_obj_create(parent);
	lv_obj_align(actuators.aux, LV_ALIGN_CENTER, 24, aYOffset + 46);
	lv_obj_set_size(actuators.aux, kPanelSize * 2 + 10, kPanelSize);
	lv_obj_clear_flag(actuators.aux, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_style(actuators.aux, &style_menu_base, 0);
	lv_obj_t *actuatorAuxLabel = lv_label_create(actuators.aux);
	lv_label_set_text_static(actuatorAuxLabel, "AUX");
	lv_obj_align(actuatorAuxLabel, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_event_cb(actuators.aux, actuatorPressedEventHandler, LV_EVENT_CLICKED, NULL);
}

void mainPageCreate(lv_obj_t *parent)
{
	static const uint16_t panelW = 155;
	static const uint16_t panelH = 310;
	static const uint16_t miniPanelW = 130;

	// Панель с аркой, уровнем воды и температурой воды
	panel1 = lv_obj_create(parent);
	lv_obj_set_size(panel1, panelW + 10, panelH + 10);
	lv_obj_align_to(panel1, parent, LV_ALIGN_LEFT_MID, 0, 0);
	lv_obj_clear_flag(panel1, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
	lv_obj_add_style(panel1, &style_menu_panel, 0);

	// Панель с информацией о всех датчиках, времени
	panel2 = lv_obj_create(parent);
	lv_obj_set_size(panel2, panelW, panelH);
	lv_obj_align_to(panel2, parent, LV_ALIGN_CENTER, 0, 0);
	lv_obj_clear_flag(panel2, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
	lv_obj_add_style(panel2, &style_menu_panel, 0);

	// Панель с информацией о исполнительных устройствах и кнопка настройки
	panel3 = lv_obj_create(parent);
	lv_obj_set_size(panel3, panelW, panelH);
	lv_obj_align_to(panel3, parent, LV_ALIGN_RIGHT_MID, -4, 0);
	lv_obj_clear_flag(panel3, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
	lv_obj_add_style(panel3, &style_menu_panel, 0);

	{ // ******************************************************* ПАНЕЛЬ 1 *******************************************************
		// Отображение текущего режима
		lv_obj_t *currentModePanel = lv_obj_create(panel1);
		lv_obj_set_size(currentModePanel, miniPanelW, 30);
		lv_obj_align_to(currentModePanel, panel1, LV_ALIGN_TOP_MID, 0, -5);
		lv_obj_clear_flag(currentModePanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		lv_obj_add_style(currentModePanel, &style_menu_subpanel, 0);
		currentModeLabel = lv_label_create(currentModePanel);
		lv_obj_align_to(currentModeLabel, currentModePanel, LV_ALIGN_TOP_MID, 0, -10);

		lv_obj_t *hydroTypePanel = lv_obj_create(panel1);
		lv_obj_add_style(hydroTypePanel, &styleImageHolder, 0);
		lv_obj_set_size(hydroTypePanel, 160, 130);
		lv_obj_align_to(hydroTypePanel, panel1, LV_ALIGN_TOP_MID, -2, 170);
		lv_obj_clear_flag(hydroTypePanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг

		hydroTypeImage = lv_img_create(hydroTypePanel);
		lv_obj_clear_flag(hydroTypePanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		lv_obj_align(hydroTypeImage, LV_ALIGN_CENTER, 0, -50);
	}
	{ // ******************************************************* ПАНЕЛЬ 2 *******************************************************
		// Панель для времени
		currentTimePanel = lv_obj_create(panel2);
		lv_obj_set_size(currentTimePanel, miniPanelW, 30);
		lv_obj_align_to(currentTimePanel, panel2, LV_ALIGN_TOP_MID, 0, -10);
		lv_obj_clear_flag(currentTimePanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		lv_obj_add_style(currentTimePanel, &style_menu_subpanel, 0);

		// Текст с текущим временем
		mainPageTime = lv_label_create(currentTimePanel);
		lv_label_set_text_static(mainPageTime, currentTimeLabelText);
		lv_obj_align_to(mainPageTime, currentTimePanel, LV_ALIGN_BOTTOM_MID, 0, 10);
		// Панель для PH
		lv_obj_t *pHPanel = lv_obj_create(panel2);
		lv_obj_set_size(pHPanel, miniPanelW, 30);
		lv_obj_align_to(pHPanel, panel2, LV_ALIGN_TOP_MID, 0, 25);
		lv_obj_clear_flag(pHPanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		lv_obj_add_style(pHPanel, &style_menu_subpanel, 0);

		// Текст для PH
		lv_obj_t *mainPagePHLabel = lv_label_create(pHPanel);
		lv_label_set_text_static(mainPagePHLabel, "PH: ");
		lv_obj_align_to(mainPagePHLabel, pHPanel, LV_ALIGN_LEFT_MID, 20, 0);

		mainPagePH = lv_label_create(pHPanel);
		lv_label_set_text_static(mainPagePH, phLabelText);
		lv_obj_align_to(mainPagePH, pHPanel, LV_ALIGN_RIGHT_MID, -25, 0);
		// Панель для PPM
		lv_obj_t *ppmPanel = lv_obj_create(panel2);
		lv_obj_set_size(ppmPanel, miniPanelW, 30);
		lv_obj_align_to(ppmPanel, panel2, LV_ALIGN_TOP_MID, 0, 60);
		lv_obj_clear_flag(ppmPanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		lv_obj_add_style(ppmPanel, &style_menu_subpanel, 0);
		// Текст для PPM
		lv_obj_t *mainPagePPMLabel = lv_label_create(ppmPanel);
		lv_label_set_text_static(mainPagePPMLabel, "PPM: ");
		lv_obj_align_to(mainPagePPMLabel, ppmPanel, LV_ALIGN_LEFT_MID, 10, 0);

		mainPagePPM = lv_label_create(ppmPanel);
		lv_label_set_text_static(mainPagePPM, ppmLabelText);
		lv_obj_align_to(mainPagePPM, ppmPanel, LV_ALIGN_RIGHT_MID, -10, 0);
		// Панель для температуры воды
		lv_obj_t *waterTempPanel = lv_obj_create(panel2);
		lv_obj_set_size(waterTempPanel, miniPanelW, 30);
		lv_obj_align_to(waterTempPanel, panel2, LV_ALIGN_TOP_MID, 0, 95);
		lv_obj_clear_flag(waterTempPanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		lv_obj_add_style(waterTempPanel, &style_menu_subpanel, 0);
		// Текст для температуры воды
		lv_obj_t *mainPageWaterTempLabel = lv_label_create(waterTempPanel);
		lv_label_set_text_static(mainPageWaterTempLabel, "Temp: ");
		lv_obj_align_to(mainPageWaterTempLabel, waterTempPanel, LV_ALIGN_LEFT_MID, 5, 0);

		mainPageWaterTemp = lv_label_create(waterTempPanel);
		lv_label_set_text_static(mainPageWaterTemp, tempLabelText);
		lv_obj_align_to(mainPageWaterTemp, waterTempPanel, LV_ALIGN_RIGHT_MID, -10, 0);

		// Индикатор уровня воды
		// Панель
		lv_obj_t *waterLevelPanel = lv_obj_create(panel2);
		lv_obj_set_size(waterLevelPanel, miniPanelW, 30);
		lv_obj_align_to(waterLevelPanel, panel2, LV_ALIGN_TOP_MID, 0, 130);
		lv_obj_clear_flag(waterLevelPanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		lv_obj_add_style(waterLevelPanel, &style_menu_subpanel, 0);
		// Текст для уровня воды
		lv_obj_t *mainPageWaterLevelLabel = lv_label_create(waterLevelPanel);
		lv_label_set_text_static(mainPageWaterLevelLabel, "Water: ");
		lv_obj_align_to(mainPageWaterLevelLabel, waterLevelPanel, LV_ALIGN_LEFT_MID, 5, 0);

		mainPageWaterLev = lv_label_create(waterLevelPanel);
		lv_label_set_text_static(mainPageWaterLev, waterLevelLabelText);
		lv_obj_align_to(mainPageWaterLev, waterLevelPanel, LV_ALIGN_RIGHT_MID, -15, 0);
	}
	{ // ******************************************************* ПАНЕЛЬ 3 *******************************************************
		// Панель для насоса с выбором цвета
		lowerStatusPanel = lv_obj_create(panel3);
		lv_obj_set_size(lowerStatusPanel, miniPanelW, 30);
		lv_obj_align_to(lowerStatusPanel, panel3, LV_ALIGN_TOP_MID, 0, -10);
		lv_obj_clear_flag(lowerStatusPanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		lv_obj_add_style(lowerStatusPanel, &style_menu_base, 0);
		lv_obj_add_event_cb(lowerStatusPanel, deviceDetailedInfoOpenCallback, LV_EVENT_CLICKED, NULL);
		// Текст LOWER
		lv_obj_t *lowerStatusLabel = lv_label_create(lowerStatusPanel);
		lv_label_set_text_static(lowerStatusLabel, "LOWER");
		lv_obj_align_to(lowerStatusLabel, lowerStatusPanel, LV_ALIGN_TOP_MID, 0, -10);

		// Панель для лампы с выбором цвета
		auxStatusPanel = lv_obj_create(panel3);
		lv_obj_set_size(auxStatusPanel, miniPanelW, 30);
		lv_obj_align_to(auxStatusPanel, panel3, LV_ALIGN_TOP_MID, 0, 25);
		lv_obj_clear_flag(auxStatusPanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		lv_obj_add_style(auxStatusPanel, &style_menu_base, 0);
		lv_obj_add_event_cb(auxStatusPanel, deviceDetailedInfoOpenCallback, LV_EVENT_CLICKED, NULL);
		// Текст AUX
		lv_obj_t *auxStatusLabel = lv_label_create(auxStatusPanel);
		lv_label_set_text_static(auxStatusLabel, "AUX");
		lv_obj_align_to(auxStatusLabel, auxStatusPanel, LV_ALIGN_TOP_MID, 0, -10);

		// Панель для сенсоров с выбором цвета
		upperStatusPanel = lv_obj_create(panel3);
		lv_obj_set_size(upperStatusPanel, miniPanelW, 30);
		lv_obj_align_to(upperStatusPanel, panel3, LV_ALIGN_TOP_MID, 0, 60);
		lv_obj_clear_flag(upperStatusPanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		lv_obj_add_style(upperStatusPanel, &style_menu_base, 0);
		lv_obj_add_event_cb(upperStatusPanel, deviceDetailedInfoOpenCallback, LV_EVENT_CLICKED, NULL);
		// Текст UPPER
		lv_obj_t *upperStatusLabel = lv_label_create(upperStatusPanel);
		lv_label_set_text_static(upperStatusLabel, "UPPER");
		lv_obj_align_to(upperStatusLabel, upperStatusPanel, LV_ALIGN_TOP_MID, 0, -10);

		// Панель для системы с выбором цвета
		systemStatusPanel = lv_obj_create(panel3);
		lv_obj_set_size(systemStatusPanel, miniPanelW, 30);
		lv_obj_align_to(systemStatusPanel, panel3, LV_ALIGN_TOP_MID, 0, 95);
		lv_obj_clear_flag(systemStatusPanel, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг
		lv_obj_add_style(systemStatusPanel, &style_menu_base, 0);
		lv_obj_add_event_cb(systemStatusPanel, deviceDetailedInfoOpenCallback, LV_EVENT_CLICKED, NULL);
		// Текст SYSTEM
		lv_obj_t *systemStatusLabel = lv_label_create(systemStatusPanel);
		lv_label_set_text_static(systemStatusLabel, "SYSTEM");
		lv_obj_align_to(systemStatusLabel, systemStatusPanel, LV_ALIGN_TOP_MID, 0, -10);

		// Кнопка с картинкой настроек
		LV_IMG_DECLARE(SettingsWhite);
		lv_obj_t *settingsButton = lv_imgbtn_create(panel3);
		lv_obj_set_size(settingsButton, 125, 125);
		lv_obj_align_to(settingsButton, panel3, LV_ALIGN_BOTTOM_MID, 0, 0);
		lv_imgbtn_set_src(settingsButton, LV_IMGBTN_STATE_RELEASED, &SettingsWhite, NULL, NULL);
		lv_obj_add_event_cb(settingsButton, &settingsButtonEvent, LV_EVENT_CLICKED, NULL);
	}

	// Актуаторы
	actuatorsCreate(panel1, -90);
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
	lv_obj_add_event_cb(pumpOnTa, textAreaCommonCallback, LV_EVENT_ALL, pumpSettingsScr);
	lv_obj_add_event_cb(pumpOnTa, processTap, LV_EVENT_ALL, NULL);

	// Панель для установки времени PumpOff
	pumpOffTa = lv_textarea_create(pumpSettingsScr);
	lv_textarea_set_accepted_chars(pumpOffTa, "0123456789");
	lv_textarea_set_max_length(pumpOffTa, 5);
	lv_textarea_set_one_line(pumpOffTa, true);
	lv_textarea_set_text(pumpOffTa, "");
	lv_textarea_set_placeholder_text(pumpOffTa, "456");
	lv_obj_set_size(pumpOffTa, 200, 40);
	lv_obj_add_event_cb(pumpOffTa, textAreaCommonCallback, LV_EVENT_ALL, pumpSettingsScr);
	lv_obj_add_event_cb(pumpOffTa, processTap, LV_EVENT_ALL, NULL);

	lv_obj_t *pumpOnSetText = lv_label_create(pumpSettingsScr);
	lv_label_set_text_static(pumpOnSetText, "Pump on time: ");
	lv_obj_t *pumpOffSetText = lv_label_create(pumpSettingsScr);
	lv_label_set_text_static(pumpOffSetText, "Pump off time: ");

	lv_obj_align_to(pumpOnTa, pumpSettingsScr, LV_ALIGN_TOP_RIGHT, -120, 5);
	lv_obj_align_to(pumpOffTa, pumpSettingsScr, LV_ALIGN_TOP_RIGHT, -120, 50);
	lv_obj_align_to(pumpOnSetText, pumpSettingsScr, LV_ALIGN_TOP_LEFT, 10, 15);
	lv_obj_align_to(pumpOffSetText, pumpSettingsScr, LV_ALIGN_TOP_LEFT, 10, 60);

	// Панель с настройкой времени включения и выключения лампы
	lv_obj_t *lampSettingsOnLabel = lv_label_create(lampSettingsScr);
	lv_label_set_text_static(lampSettingsOnLabel, "Lamp On - HH:MM");
	lv_obj_t *lampSettingsOffLabel = lv_label_create(lampSettingsScr);
	lv_label_set_text_static(lampSettingsOffLabel, "Lamp Off - HH:MM");

	lv_obj_t *returnButton = lv_btn_create(pumpSettingsScr);
	lv_obj_set_size(returnButton, 70, 70);
	lv_obj_align(returnButton, LV_ALIGN_TOP_RIGHT, -12, 12);
	lv_obj_add_event_cb(returnButton, returnToSettingsEventHandler, LV_EVENT_CLICKED, NULL);
	lv_obj_t *returnButtonLabel = lv_label_create(returnButton);
	lv_label_set_text_static(returnButtonLabel, "Return");
	lv_obj_align(returnButtonLabel, LV_ALIGN_CENTER, 0, 0);

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
	lv_obj_add_event_cb(lampOnHourTa, textAreaCommonCallback, LV_EVENT_ALL, lampSettingsScr);
	lv_obj_add_event_cb(lampOnHourTa, formattedAreaCommonCallback, LV_EVENT_VALUE_CHANGED, &kFormattingHourEnumerator);
	lv_obj_add_event_cb(lampOnHourTa, processTap, LV_EVENT_ALL, NULL);
	// Панель минут
	lampOnMinTa = lv_textarea_create(lampSettingsScr);
	lv_textarea_set_accepted_chars(lampOnMinTa, "0123456789");
	lv_textarea_set_max_length(lampOnMinTa, 2);
	lv_textarea_set_one_line(lampOnMinTa, true);
	lv_textarea_set_text(lampOnMinTa, "");
	lv_textarea_set_placeholder_text(lampOnMinTa, "00");

	lv_obj_set_size(lampOnMinTa, CLOCK_SET_TA_WIDHT, CLOCK_SET_TA_HEIGH);
	lv_obj_add_event_cb(lampOnMinTa, textAreaCommonCallback, LV_EVENT_ALL, lampSettingsScr);
	lv_obj_add_event_cb(lampOnMinTa, formattedAreaCommonCallback, LV_EVENT_VALUE_CHANGED, &kFormattingMinSecEnumerator);
	lv_obj_add_event_cb(lampOnMinTa, processTap, LV_EVENT_ALL, NULL);

	lampOffHourTa = lv_textarea_create(lampSettingsScr);
	lv_textarea_set_accepted_chars(lampOffHourTa, "0123456789");
	lv_textarea_set_max_length(lampOffHourTa, 2);
	lv_textarea_set_one_line(lampOffHourTa, true);
	lv_textarea_set_text(lampOffHourTa, "");
	lv_textarea_set_placeholder_text(lampOffHourTa, "00");

	lv_obj_set_size(lampOffHourTa, CLOCK_SET_TA_WIDHT, CLOCK_SET_TA_HEIGH);
	lv_obj_add_event_cb(lampOffHourTa, textAreaCommonCallback, LV_EVENT_ALL, lampSettingsScr);
	lv_obj_add_event_cb(lampOffHourTa, formattedAreaCommonCallback, LV_EVENT_VALUE_CHANGED, &kFormattingHourEnumerator);
	lv_obj_add_event_cb(lampOffHourTa, processTap, LV_EVENT_ALL, NULL);

	lampOffMinTa = lv_textarea_create(lampSettingsScr);
	lv_textarea_set_accepted_chars(lampOffMinTa, "0123456789");
	lv_textarea_set_max_length(lampOffMinTa, 2);
	lv_textarea_set_one_line(lampOffMinTa, true);
	lv_textarea_set_text(lampOffMinTa, "");
	lv_textarea_set_placeholder_text(lampOffMinTa, "00");

	lv_obj_set_size(lampOffMinTa, CLOCK_SET_TA_WIDHT, CLOCK_SET_TA_HEIGH);
	lv_obj_add_event_cb(lampOffMinTa, textAreaCommonCallback, LV_EVENT_ALL, lampSettingsScr);
	lv_obj_add_event_cb(lampOffMinTa, formattedAreaCommonCallback, LV_EVENT_VALUE_CHANGED, &kFormattingMinSecEnumerator);
	lv_obj_add_event_cb(lampOffMinTa, processTap, LV_EVENT_ALL, NULL);

	// Align для всех элементов
	lv_obj_align_to(lampSettingsOnLabel, lampSettingsScr, LV_ALIGN_TOP_LEFT, 10, 12);
	lv_obj_align_to(lampSettingsOffLabel, lampSettingsScr, LV_ALIGN_TOP_LEFT, 10, 57);
	lv_obj_align_to(lampOnHourTa, lampSettingsScr, LV_ALIGN_TOP_RIGHT, -280, 2);
	lv_obj_align_to(lampOffHourTa, lampSettingsScr, LV_ALIGN_TOP_RIGHT, -280, 47);
	lv_obj_align_to(lampOnMinTa, lampSettingsScr, LV_ALIGN_TOP_RIGHT, -230, 2);
	lv_obj_align_to(lampOffMinTa, lampSettingsScr, LV_ALIGN_TOP_RIGHT, -230, 47);

	returnButton = lv_btn_create(lampSettingsScr);
	lv_obj_set_size(returnButton, 70, 70);
	lv_obj_align(returnButton, LV_ALIGN_TOP_RIGHT, -12, 12);
	lv_obj_add_event_cb(returnButton, returnToSettingsEventHandler, LV_EVENT_CLICKED, NULL);
	returnButtonLabel = lv_label_create(returnButton);
	lv_label_set_text_static(returnButtonLabel, "Return");
	lv_obj_align(returnButtonLabel, LV_ALIGN_CENTER, 0, 0);

	// Панель установки времени
	setTimeHourTa = lv_textarea_create(curTimeSettingsScr);
	lv_textarea_set_accepted_chars(setTimeHourTa, "0123456789");
	lv_textarea_set_max_length(setTimeHourTa, 2);
	lv_textarea_set_one_line(setTimeHourTa, true);
	lv_textarea_set_text(setTimeHourTa, "");
	lv_textarea_set_placeholder_text(setTimeHourTa, "00");

	lv_obj_set_size(setTimeHourTa, CLOCK_SET_TA_WIDHT, CLOCK_SET_TA_HEIGH);
	lv_obj_add_event_cb(setTimeHourTa, textAreaCommonCallback, LV_EVENT_ALL, curTimeSettingsScr);
	lv_obj_add_event_cb(setTimeHourTa, formattedAreaCommonCallback, LV_EVENT_VALUE_CHANGED, &kFormattingHourEnumerator);
	lv_obj_add_event_cb(setTimeHourTa, processTap, LV_EVENT_ALL, NULL);

	setTimeMinTa = lv_textarea_create(curTimeSettingsScr);
	lv_textarea_set_accepted_chars(setTimeMinTa, "0123456789");
	lv_textarea_set_max_length(setTimeMinTa, 2);
	lv_textarea_set_one_line(setTimeMinTa, true);
	lv_textarea_set_text(setTimeMinTa, "");
	lv_textarea_set_placeholder_text(setTimeMinTa, "00");

	lv_obj_set_size(setTimeMinTa, CLOCK_SET_TA_WIDHT, CLOCK_SET_TA_HEIGH);
	lv_obj_add_event_cb(setTimeMinTa, textAreaCommonCallback, LV_EVENT_ALL, curTimeSettingsScr);
	lv_obj_add_event_cb(setTimeMinTa, formattedAreaCommonCallback, LV_EVENT_VALUE_CHANGED, &kFormattingMinSecEnumerator);
	lv_obj_add_event_cb(setTimeMinTa, processTap, LV_EVENT_ALL, NULL);

	setTimeSecTa = lv_textarea_create(curTimeSettingsScr);
	lv_textarea_set_accepted_chars(setTimeSecTa, "0123456789");
	lv_textarea_set_max_length(setTimeSecTa, 2);
	lv_textarea_set_one_line(setTimeSecTa, true);
	lv_textarea_set_text(setTimeSecTa, "");
	lv_textarea_set_placeholder_text(setTimeSecTa, "00");

	lv_obj_set_size(setTimeSecTa, CLOCK_SET_TA_WIDHT, CLOCK_SET_TA_HEIGH);
	lv_obj_add_event_cb(setTimeSecTa, textAreaCommonCallback, LV_EVENT_ALL, curTimeSettingsScr);
	lv_obj_add_event_cb(setTimeSecTa, formattedAreaCommonCallback, LV_EVENT_VALUE_CHANGED, &kFormattingMinSecEnumerator);
	lv_obj_add_event_cb(setTimeSecTa, processTap, LV_EVENT_ALL, NULL);

	sendNewTimeButton = lv_btn_create(curTimeSettingsScr);
	lv_obj_set_size(sendNewTimeButton, 120, 40);
	lv_obj_add_event_cb(sendNewTimeButton, setTimeButtonEventHandler, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(sendNewTimeButton, processTap, LV_EVENT_ALL, NULL);
	lv_obj_t *setTimeButtonLabel = lv_label_create(sendNewTimeButton);
	lv_obj_align_to(setTimeButtonLabel, sendNewTimeButton, LV_ALIGN_LEFT_MID, 0, 0);
	lv_label_set_text_static(setTimeButtonLabel, "Send to RTC");

	// Алигним
	lv_obj_align_to(setTimeHourTa, curTimeSettingsScr, LV_ALIGN_TOP_LEFT, 20, 20);
	lv_obj_align_to(setTimeMinTa, curTimeSettingsScr, LV_ALIGN_TOP_LEFT, 70, 20);
	lv_obj_align_to(setTimeSecTa, curTimeSettingsScr, LV_ALIGN_TOP_LEFT, 120, 20);
	lv_obj_align_to(sendNewTimeButton, curTimeSettingsScr, LV_ALIGN_TOP_RIGHT, -170, 20);

	returnButton = lv_btn_create(curTimeSettingsScr);
	lv_obj_set_size(returnButton, 70, 70);
	lv_obj_align(returnButton, LV_ALIGN_TOP_RIGHT, -12, 12);
	lv_obj_add_event_cb(returnButton, returnToSettingsEventHandler, LV_EVENT_CLICKED, NULL);
	returnButtonLabel = lv_label_create(returnButton);
	lv_label_set_text_static(returnButtonLabel, "Return");
	lv_obj_align(returnButtonLabel, LV_ALIGN_CENTER, 0, 0);
}

void createDescribedWindow(lv_obj_t *aParent)
{
	deviceDescInfoPanel = lv_obj_create(aParent);
	lv_obj_set_size(deviceDescInfoPanel, 200, 200);
	lv_obj_align(deviceDescInfoPanel, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_flag(deviceDescInfoPanel, LV_OBJ_FLAG_HIDDEN);

	lv_obj_t *exitButton = lv_btn_create(deviceDescInfoPanel);
	lv_obj_t *exitButtonLabel = lv_label_create(exitButton);
	lv_label_set_text_static(exitButtonLabel, "Close");
	lv_obj_align(exitButton, LV_ALIGN_BOTTOM_MID, 0, 0);
	lv_obj_add_event_cb(exitButton, deviceDetailedInfoCloseCallback, LV_EVENT_CLICKED, NULL);

	createLed(deviceDescInfoPanel, deviceDescPanelLabel1, deviceDescPanelLed1, deviceDescPanelString1, 110, 0, 0);
	createLed(deviceDescInfoPanel, deviceDescPanelLabel2, deviceDescPanelLed2, deviceDescPanelString2, 110, 0, 20);
	createLed(deviceDescInfoPanel, deviceDescPanelLabel3, deviceDescPanelLed3, deviceDescPanelString3, 110, 0, 40);
	createLed(deviceDescInfoPanel, deviceDescPanelLabel4, deviceDescPanelLed4, deviceDescPanelString4, 110, 0, 60);
	createLed(deviceDescInfoPanel, deviceDescPanelLabel5, deviceDescPanelLed5, deviceDescPanelString5, 110, 0, 80);
	createLed(deviceDescInfoPanel, deviceDescPanelLabel6, deviceDescPanelLed6, deviceDescPanelString6, 110, 0, 100);
}

void menuCreate(lv_obj_t *parent)
{
	menu = lv_menu_create(parent);
	bg_color = lv_obj_get_style_bg_color(menu, 0);

	if (lv_color_brightness(bg_color) > 127) {
		lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, 0), 10), 0);
	} else {
		lv_obj_set_style_bg_color(menu, lv_color_darken(lv_obj_get_style_bg_color(menu, 0), 50), 0);
	}

	// Сознательно отключаю кнопку назад, поскольку это будет отдельный пункт меню
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
	pumpEnableButton = createSwitch(section, LV_SYMBOL_SETTINGS, "Enable", true);
	lv_obj_add_event_cb(pumpEnableButton, processTap, LV_EVENT_VALUE_CHANGED, NULL);

	// Текст - Время включенного состояния насоса
	lv_obj_t *pumpOnBaseText = createText(section, NULL, "Pump on time", LV_MENU_ITEM_BUILDER_VARIANT_1);
	pumpOnCornerText = lv_label_create(pumpOnBaseText);
	lv_label_set_text_static(pumpOnCornerText, settingsPumpOnTimeText);
	lv_obj_align_to(pumpOnCornerText, pumpOnBaseText, LV_TEXT_ALIGN_RIGHT, 0, 0);

	// Текст - Время выключенного состояния насоса
	lv_obj_t *pumpOffBaseText = createText(section, NULL, "Pump off time", LV_MENU_ITEM_BUILDER_VARIANT_2);
	pumpOffCornerText = lv_label_create(pumpOffBaseText);
	lv_label_set_text_static(pumpOffCornerText, settingsPumpOffTimeText);
	lv_obj_align_to(pumpOffCornerText, pumpOffBaseText, LV_TEXT_ALIGN_RIGHT, 100, 0);

	// Кнопка для настройки времени
	lv_obj_t *pumpConfigButton = lv_btn_create(section);
	lv_obj_set_size(pumpConfigButton, 314, 35);
	lv_obj_t *pumpOnButtonLabel = lv_label_create(pumpConfigButton);
	lv_label_set_text_static(pumpOnButtonLabel, "Configure pump timings");
	lv_obj_align_to(pumpOnButtonLabel, pumpConfigButton, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_event_cb(pumpConfigButton, customTextAreaEvent, LV_EVENT_CLICKED, pumpSettingsScr);
	lv_obj_add_event_cb(pumpConfigButton, processTap, LV_EVENT_VALUE_CHANGED, NULL);

	// Выпадающий список с типами гидропоник
	createText(section, NULL, "Hydroponic type", LV_MENU_ITEM_BUILDER_VARIANT_1);
	pumpTypeDD = lv_dropdown_create(section);
	lv_dropdown_set_options(pumpTypeDD,
		"Normal\n"
		"Swing\n"
		"Maintance\n"
		"Drip\n"
		"Dam");
	lv_dropdown_set_dir(pumpTypeDD, LV_DIR_RIGHT);

	lv_obj_align(pumpTypeDD, LV_ALIGN_TOP_MID, 0, 20);
	lv_obj_add_event_cb(pumpTypeDD, pumpTypeEventHandler, LV_EVENT_ALL, NULL);
	lv_obj_add_event_cb(pumpTypeDD, processTap, LV_EVENT_CLICKED, NULL);

	// Поле - swing время (если режим не swing - неактивно)
	pumpSwingTimeBase = createText(section, LV_SYMBOL_SETTINGS, "Swing Time", LV_MENU_ITEM_BUILDER_VARIANT_2);
	pumpSwingTimeText = lv_label_create(pumpSwingTimeBase);
	lv_label_set_text_static(pumpSwingTimeText, settingsPumpSwingTimeText);
	lv_obj_align(pumpSwingTimeText, LV_ALIGN_RIGHT_MID, 0, 0);

	pumpSwingTimeSlider = lv_slider_create(pumpSwingTimeBase);
	lv_obj_set_flex_grow(pumpSwingTimeSlider, 2);
	lv_slider_set_range(pumpSwingTimeSlider, 0, 10);
	lv_obj_set_size(pumpSwingTimeSlider, 20, 10);
	lv_slider_set_value(pumpSwingTimeSlider, 5, LV_ANIM_OFF);
	lv_obj_add_event_cb(pumpSwingTimeSlider, pumpSwingTimeEvent, LV_EVENT_VALUE_CHANGED, NULL);
	lv_obj_add_event_cb(pumpSwingTimeSlider, processTap, LV_EVENT_CLICKED, NULL);

	// ********************************МЕНЮ ЛАМПЫ********************************
	// Подменю с настройками лампы
	subLampPage = lv_menu_page_create(menu, NULL);
	lv_obj_set_style_pad_hor(subLampPage, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
	lv_menu_separator_create(subLampPage);
	section = lv_menu_section_create(subLampPage);
	// Включение или выключение управления лампой
	lampEnableButton = createSwitch(section, LV_SYMBOL_SETTINGS, "Enable", true);
	lv_obj_add_event_cb(lampEnableButton, processTap, LV_EVENT_VALUE_CHANGED, NULL);
	lv_menu_separator_create(section);

	// Текст - время включения лампы
	lv_obj_t *lampOnBaseText = createText(section, NULL, "Lamp On Time", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lampOnCornerText = lv_label_create(lampOnBaseText);
	lv_label_set_text_static(lampOnCornerText, settingsLampOnTime);
	lv_obj_align_to(lampOnCornerText, lampOnBaseText, LV_TEXT_ALIGN_RIGHT, 0, 0);

	lv_obj_t *lampOffBaseText = createText(section, NULL, "Lamp Off Time", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lampOffCornerText = lv_label_create(lampOffBaseText);
	lv_label_set_text_static(lampOffCornerText, settingsLampOffTime);
	lv_obj_align_to(lampOffCornerText, lampOffBaseText, LV_TEXT_ALIGN_RIGHT, 0, 0);

	// Поле ввода цифр для включенного состояния
	lv_obj_t *lampSettingsButton = lv_btn_create(section);
	lv_obj_set_size(lampSettingsButton, 314, 35);
	lv_obj_t *lampButtonLabel = lv_label_create(lampSettingsButton);
	lv_label_set_text_static(lampButtonLabel, "Configure lamp timings");
	lv_obj_align_to(lampButtonLabel, lampSettingsButton, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_event_cb(lampSettingsButton, customTextAreaEvent, LV_EVENT_CLICKED, lampSettingsScr);
	lv_obj_add_event_cb(lampSettingsButton, processTap, LV_EVENT_CLICKED, NULL);

	// ******************************** МЕНЮ ОБЩИХ НАСТРОЕК **********************************
	subCommonPage = lv_menu_page_create(menu, NULL);
	lv_obj_set_style_pad_hor(subCommonPage, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
	lv_menu_separator_create(subCommonPage);
	section = lv_menu_section_create(subCommonPage);

	// Свитч для включения или выключения звука при тапе
	tapSountEnableButton = createSwitch(section, LV_SYMBOL_AUDIO, "Tap sound", false);
	alarmSoundEnableButton = createSwitch(section, LV_SYMBOL_AUDIO, "Alarm sound", false);
	lv_obj_add_event_cb(tapSountEnableButton, processTap, LV_EVENT_VALUE_CHANGED, NULL); // ?
	lv_obj_add_event_cb(alarmSoundEnableButton, processTap, LV_EVENT_VALUE_CHANGED, NULL);

	brightnessSlider = createSlider(section, NULL, "Display brightness", 30, 255, 50);
	lv_obj_add_event_cb(brightnessSlider, brightnessSliderEventHandler, LV_EVENT_VALUE_CHANGED, NULL);
	lv_obj_add_event_cb(brightnessSlider, processTap, LV_EVENT_VALUE_CHANGED, NULL);
	lv_menu_separator_create(section);

	// Настройка  текущего времени
	lv_obj_t *setTimeBaseText = createText(section, NULL, "Current time", LV_MENU_ITEM_BUILDER_VARIANT_1);
	settingsPageTime = lv_label_create(setTimeBaseText);
	lv_label_set_text_static(settingsPageTime, currentTimeLabelText);
	lv_obj_align_to(settingsPageTime, setTimeBaseText, LV_TEXT_ALIGN_RIGHT, 0, 0);

	// Кнопка для отправки времени в RTC
	lv_obj_t *setTimeButton = lv_btn_create(section);
	lv_obj_set_size(setTimeButton, 314, 35);
	lv_obj_add_event_cb(setTimeButton, customTextAreaEvent, LV_EVENT_PRESSED, curTimeSettingsScr);
	lv_obj_add_event_cb(setTimeButton, processTap, LV_EVENT_PRESSED, NULL);
	lv_obj_align(setTimeButton, LV_ALIGN_CENTER, 0, -40);
	// Надпись на кнопке
	lv_obj_t *setTimeLabel = lv_label_create(setTimeButton);
	lv_label_set_text_static(setTimeLabel, "Configure current time");
	lv_obj_center(setTimeLabel);

	lv_menu_separator_create(section);

	lv_obj_t *mqttConfigureButton = lv_btn_create(section);
	lv_obj_set_size(mqttConfigureButton, 314, 35);
	lv_obj_add_state(mqttConfigureButton, LV_STATE_DISABLED);

	lv_obj_t *mqttConfigureLabel = lv_label_create(mqttConfigureButton);
	lv_label_set_text_static(mqttConfigureLabel, "Configure connections");
	lv_obj_center(mqttConfigureLabel);

	// ******************************** МЕНЮ СЕРВИСА **********************************
	subManualPage = lv_menu_page_create(menu, NULL);
	lv_obj_set_style_pad_hor(subManualPage, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
	lv_menu_separator_create(subManualPage);
	section = lv_menu_section_create(subManualPage);
	lv_obj_clear_flag(section, LV_OBJ_FLAG_SCROLLABLE); // Отключаем скроллинг

	// Команда калибровки ловера
	lowerCalibButton = lv_btn_create(section);
	lv_obj_set_size(lowerCalibButton, 314, 35);
	lv_obj_t *lowerCalibButtonLabel = lv_label_create(lowerCalibButton);
	lv_label_set_text_static(lowerCalibButtonLabel, "Calibrate LOWER");
	lv_obj_align_to(lowerCalibButtonLabel, lowerCalibButton, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_event_cb(lowerCalibButton, serviceEventHandler, LV_EVENT_CLICKED, NULL);
	lv_obj_add_event_cb(lowerCalibButton, processTap, LV_EVENT_CLICKED, NULL);

	// Включение - выключение логгирования
	loggingSwitch = createSwitch(section, LV_SYMBOL_WARNING, "Logging", false);
	lv_obj_add_event_cb(loggingSwitch, processTap, LV_EVENT_VALUE_CHANGED, NULL);
	loggingTextarea = lv_textarea_create(section);
	lv_obj_set_size(loggingTextarea, 315, 150);
	lv_textarea_set_text(loggingTextarea, "");

	// ******************************** МЕНЮ ABOUT **********************************
	subAboutPage = lv_menu_page_create(menu, NULL);
	lv_obj_set_style_pad_hor(subAboutPage, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
	lv_menu_separator_create(subAboutPage);
	section = lv_menu_section_create(subAboutPage);

	lv_obj_t *versionContaiter = createText(section, NULL, "Version:", LV_MENU_ITEM_BUILDER_VARIANT_1);
	aboutVersionFiller = lv_label_create(versionContaiter);
	lv_label_set_text_static(aboutVersionFiller, AUTO_VERSION);
	lv_obj_align(aboutVersionFiller, LV_ALIGN_LEFT_MID, 0,0);

	lv_obj_t *wifiPresentContainer = createText(section, NULL, "Wifi status:", LV_MENU_ITEM_BUILDER_VARIANT_1);
	aboutWifiPresentFiller = lv_label_create(wifiPresentContainer);
	lv_label_set_text_static(aboutWifiPresentFiller, "UNSUPPORTED");
	lv_obj_align(aboutWifiPresentFiller, LV_ALIGN_LEFT_MID, 0,0);

	lv_obj_t *mqttPresentContainer = createText(section, NULL, "MQTT status:", LV_MENU_ITEM_BUILDER_VARIANT_1);
	aboutMqttPresentFiller = lv_label_create(mqttPresentContainer);
	lv_label_set_text_static(aboutMqttPresentFiller, "UNSUPPORTED");
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
	lv_obj_add_event_cb(exitWithSaveButton, exitButtonEventHandler, LV_EVENT_PRESSED, &kExitWithSaveButtonEnumerator);
	lv_obj_add_event_cb(exitWithSaveButton, processTap, LV_EVENT_PRESSED, NULL);
	// Надпись на кнопке
	lv_obj_t *exitWithSaveLabel = lv_label_create(exitWithSaveButton);
	lv_label_set_text_static(exitWithSaveLabel, "Save and exit");
	lv_obj_align_to(exitWithSaveLabel, exitWithSaveButton, LV_ALIGN_CENTER, 0, 0);
	// ***** КНОПКА ВЫХОДА БЕЗ СОХРАНЕНИЯ
	lv_obj_t *exitWnoSaveButtonContainer = lv_menu_cont_create(section);
	lv_obj_t *exitWnoSaveButton = lv_btn_create(exitWnoSaveButtonContainer);
	lv_obj_set_size(exitWnoSaveButton, 290, 50);
	lv_obj_align(exitWnoSaveButton, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_event_cb(
		exitWnoSaveButton, exitButtonEventHandler, LV_EVENT_PRESSED, &kExitWithoutSaveButtonEnumerator);
	lv_obj_add_event_cb(exitWnoSaveButton, processTap, LV_EVENT_PRESSED, NULL);
	// Надпись на кнопке
	lv_obj_t *exitWnoSaveLabel = lv_label_create(exitWnoSaveButton);
	lv_label_set_text_static(exitWnoSaveLabel, "Exit without saving");
	lv_obj_center(exitWnoSaveLabel);

	/* ************************************ОБЩЕЕ ДЛЯ СОЗДАНИЯ МЕНЮ *******************************/
	lv_obj_t *root_page = lv_menu_page_create(menu, NULL);
	lv_obj_set_style_pad_hor(root_page, lv_obj_get_style_pad_left(lv_menu_get_main_header(menu), 0), 0);
	section = lv_menu_section_create(root_page);
	//	Колбек для нажатий по панели слева, работает через эвент Draw
	lv_obj_add_event_cb(section, processTap, LV_EVENT_DRAW_POST_BEGIN, NULL);

	cont = createText(section, LV_SYMBOL_TINT, "Pump", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, subPumpPage);
	cont = createText(section, LV_SYMBOL_UP, "Light", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, subLampPage);
	cont = createText(section, LV_SYMBOL_SETTINGS, "Common", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, subCommonPage);
	cont = createText(section, LV_SYMBOL_WARNING, "Service", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, subManualPage);
	cont = createText(section, LV_SYMBOL_LIST, "About", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, subAboutPage);

	createText(root_page, NULL, "Others", LV_MENU_ITEM_BUILDER_VARIANT_1);
	section = lv_menu_section_create(root_page);
	cont = createText(section, LV_SYMBOL_CLOSE, "Exit", LV_MENU_ITEM_BUILDER_VARIANT_1);
	lv_menu_set_load_page_event(menu, cont, subExitPage);
	lv_menu_set_sidebar_page(menu, root_page);
	lv_event_send(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0), 0), LV_EVENT_CLICKED, NULL);
}

lv_obj_t *createText(lv_obj_t *parent, const char *icon, const char *txt, lv_menu_builder_variant_t builder_variant)
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
		lv_label_set_text_static(label, txt);
		lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
		lv_obj_set_flex_grow(label, 1);
	}

	if (builder_variant == LV_MENU_ITEM_BUILDER_VARIANT_2 && icon && txt) {
		lv_obj_add_flag(img, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
		lv_obj_swap(img, label);
	}

	return obj;
}

lv_obj_t *createSlider(lv_obj_t *parent, const char *icon, const char *txt, int32_t min, int32_t max, int32_t val)
{
	lv_obj_t *obj = createText(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_2);

	lv_obj_t *slider = lv_slider_create(obj);
	lv_obj_set_flex_grow(slider, 1);
	lv_slider_set_range(slider, min, max);
	lv_slider_set_value(slider, val, LV_ANIM_OFF);

	if (icon == NULL) {
		lv_obj_add_flag(slider, LV_OBJ_FLAG_FLEX_IN_NEW_TRACK);
	}

	return slider;
}

lv_obj_t *createSwitch(lv_obj_t *parent, const char *icon, const char *txt, bool chk)
{
	lv_obj_t *obj = createText(parent, icon, txt, LV_MENU_ITEM_BUILDER_VARIANT_1);

	lv_obj_t *sw = lv_switch_create(obj);
	lv_obj_add_state(sw, chk ? LV_STATE_CHECKED : 0);

	return sw;
}

void createLed(lv_obj_t *parent, lv_obj_t *&label, lv_obj_t *&led, const char *text, uint8_t ledShiftX, uint8_t labelAlignX, uint8_t labelAlignY)
{
	label = lv_label_create(parent);
	lv_label_set_text_static(label, text);
	lv_obj_set_size(label, 130, 20);
	lv_obj_align(label, LV_ALIGN_TOP_MID, labelAlignX, labelAlignY);
	led = lv_led_create(label);
	lv_obj_set_size(led, 10, 10);
	lv_led_set_brightness(led, 200);
	lv_obj_align(led, LV_ALIGN_OUT_RIGHT_MID, 0 + ledShiftX, 3);
	lv_obj_add_flag(led, LV_OBJ_FLAG_HIDDEN);
}

#ifdef __cplusplus
} /* extern "C" */
#endif