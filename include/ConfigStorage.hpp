/*!
@file
@brief Класс-обертка над NVS
@author V-Nezlo (vlladimirka@gmail.com)
@date 17.04.2024
@version 1.0
*/

#ifndef INCLUDE_CONFIGSTORAGE_HPP_
#define INCLUDE_CONFIGSTORAGE_HPP_

#include "EventBus.hpp"
#include "NVStorage.hpp"
#include "Types.hpp"

class ConfigStorage : public AbstractEventObserver {
public:
	ConfigStorage()
	{
		NVStorage::init();
	}

	bool sync(Settings *aSettings)
	{
		return NVStorage::writeSettings(aSettings);
	}

	bool firstLoad()
	{
		Event ev;
		ev.type = EventType::SettingsUpdated;
		ev.data.settings = getDefault();

		Settings loaded;
		if (NVStorage::readSettings(&loaded)) {
			ev.data.settings = loaded;
		} else {
			ESP_LOGI("Params", "Applying defaults...");
		}

		EventBus::throwEvent(&ev, this);
		return true;
	}

	EventResult handleEvent(Event *e) override
	{
		switch(e->type) {
			case EventType::SettingsUpdated:
				sync(&e->data.settings);
				return EventResult::PASS_ON;
			default:
				return EventResult::IGNORED;
		}
	}
private:
	Settings getDefault()
	{
		Settings def;
		def.pump.enabled = false;
		def.pump.maxFloodingTime = 2000;
		def.pump.mode = PumpModes::Maintance;
		def.pump.onTime = 1000;
		def.pump.offTime = 2000;
		def.pump.swingTime = 5;

		def.lamp.enabled = true;
		def.lamp.lampOnHour = 10;
		def.lamp.lampOnMin = 0;
		def.lamp.lampOffHour = 21;
		def.lamp.lampOffMin = 0;

		def.common.alarmSoundEnabled = true;
		def.common.tapSoundEnabled = true;
		def.common.loggingEnabled = false;
		def.common.displayBrightness = 100;
		def.common.buzzerVolume = 100;

		def.modules.phSensor = false;
		def.modules.ppmSensor = false;

		return def;
	}
};

#endif // INCLUDE_CONFIGSTORAGE_HPP_