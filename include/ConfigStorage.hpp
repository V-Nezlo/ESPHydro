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
		Settings loaded;
		if (NVStorage::readSettings(&loaded)) {
			Event ev;
			ev.type = EventType::SettingsUpdated;
			ev.data.settings = loaded;
			EventBus::throwEvent(&ev, this);
			return true;
		} else {
			return false;
		}
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
};

#endif // INCLUDE_CONFIGSTORAGE_HPP_