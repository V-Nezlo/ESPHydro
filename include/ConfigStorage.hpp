/*!
@file
@brief Сиглтон для хранения всех параметров системы данных
@author V-Nezlo (vlladimirka@gmail.com)
@date 17.04.2024
@version 1.0
*/

#ifndef INCLUDE_CONFIGSTORAGE_HPP_
#define INCLUDE_CONFIGSTORAGE_HPP_

#include <Types.hpp>
#include <NVStorage.hpp>
#include <EventBus.hpp>

class ConfigStorage : public AbstractEventObserver {
public:
    struct NonVolatileData {
        Settings settings;
    } nvData;

    struct VolatileData {
        bool temp;
    } vData;

    static ConfigStorage *instance()
    {
        static ConfigStorage instance;
        return &instance;
    }

    bool sync(Settings *aSettings) // EventBus
    {
        // Запишем внутреннюю часть синглтона
        nvData.settings = *aSettings;
        // Отправим в память
        NVStorage::writeSettings(aSettings);
    }

    EventResult handleEvent(Event *e) override
    {
        switch(e->type) {
            case EventType::SettingsUpdated:
                sync(&e->data.settings);
                return EventResult::PASS_ON;
            default:
                break;
        }
    }

private:
    ConfigStorage()
    {
        NVStorage::init();
        NVStorage::readSettings(&nvData.settings);
    }
};

#endif