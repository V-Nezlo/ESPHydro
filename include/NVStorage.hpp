/*!
@file
@brief Класс для хранения NV данных
@author V-Nezlo (vlladimirka@gmail.com)
@date 09.04.2024
@version 1.0
*/

#ifndef INCLUDE_NVSTORAGE_HPP_
#define INCLUDE_NVSTORAGE_HPP_

#include <Types.hpp>
#include <esp_log.h>
#include <nvs_flash.h>
#include <nvs.h>

class NVStorage {
    static constexpr char kSettingsNamespaceName[] = "Settings";
    static constexpr char kSettingsKeyName[] = "Common";

public:
    static bool init()
    {
        return init_impl();
    }

    static bool writeSettings(const Settings *aSettings)
    {
        return writeBlob(kSettingsNamespaceName, kSettingsKeyName, aSettings, sizeof(Settings));
    }

    static bool readSettings(Settings *aSettings)
    {
        size_t size = sizeof(Settings);
        return readBlob(kSettingsNamespaceName, kSettingsKeyName, aSettings, &size);
    }

private:
    static bool init_impl() 
    {
        esp_err_t err = nvs_flash_init();
        if ((err == ESP_ERR_NVS_NO_FREE_PAGES) || (err == ESP_ERR_NVS_NEW_VERSION_FOUND)) {
            ESP_LOGW("NVS", "Erasing NVS partition...");
            nvs_flash_erase();
            err = nvs_flash_init();
        };
        if (err == ESP_OK) {
            ESP_LOGI("NVS", "NVS partition initilized");
            return true;
        } else {
            ESP_LOGE("NVS", "NVS partition initialization error: %d (%s)", err, esp_err_to_name(err));
            return false;
        };
    }

    static bool openNamespace(const char* name_group, nvs_open_mode_t open_mode, nvs_handle_t *nvs_handle)
    {
        esp_err_t err = nvs_open(name_group, open_mode, nvs_handle); 
        if (err != ESP_OK) {
            if (!((err == ESP_ERR_NVS_NOT_FOUND) && (open_mode == NVS_READONLY))) {
            ESP_LOGE("NVS", "Error opening NVS namespace \"%s\": %d (%s)!", name_group, err, esp_err_to_name(err));
            };
            return false;
        };
        return true;
    }

    static bool readBlob(const char* name_group, const char* key, void *aData, size_t *aDataSize)
    {
        nvs_handle_t handle;
        esp_err_t err;
        if (openNamespace(name_group, NVS_READONLY, &handle)) {
            err = nvs_get_blob(handle, key, aData, aDataSize);

            if (err == ESP_OK) {
                ESP_LOGI("NVS", "NVS read successful");
                return true;
            } else {
                ESP_LOGE("NVS", "NVS read error: %d (%s)", err, esp_err_to_name(err));
                return false;
            };

        } else {
            return false;
        }
    }

    static bool writeBlob(const char* name_group, const char* key, const void *aData, size_t aDataSize)
    {
        nvs_handle_t handle;
        esp_err_t err;
        if (openNamespace(name_group, NVS_READWRITE, &handle)) {
            err = nvs_set_blob(handle, key, aData, aDataSize);
            
            if (err == ESP_OK) {
                ESP_LOGI("NVS", "NVS write successful");
                return true;
            } else {
                ESP_LOGE("NVS", "NVS write error: %d (%s)", err, esp_err_to_name(err));
                return false;
            };

        } else {
            return false;
        }
    }
};

#endif