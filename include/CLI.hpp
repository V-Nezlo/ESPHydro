#ifndef INCLUDE_CLI_HPP_
#define INCLUDE_CLI_HPP_

#include "EventBus.hpp"
#include "LinearSched.hpp"
#include <stdio.h>
#include <stdlib.h>
#include "esp_system.h"
#include "esp_console.h"
#include "linenoise/linenoise.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "driver/uart_vfs.h"
#include <esp_task_wdt.h>

class Cli : public AbstractEventObserver {
	std::chrono::milliseconds nextCallTime;
	static Settings settings;
	static TaskHandle_t taskHandle;
public:
	Cli() : nextCallTime(std::chrono::milliseconds{0}) 
	{

	}

	EventResult handleEvent(Event *e) override
	{
		switch (e->type) {
			case EventType::SettingsUpdated:
				settings = e->data.settings;
				return EventResult::PASS_ON;
			default:
				return EventResult::IGNORED;
		}
	}

	static void task(void *pvParameters)
	{
		while(true) {
			char* line = linenoise(">> ");
			if (line == NULL) continue;

			// RAII
			std::unique_ptr<char, decltype(&free)> ptr(line, &free);

			int ret;
			esp_err_t err = esp_console_run(line, &ret);
			if (err == ESP_ERR_NOT_FOUND) {
				printf("Unknown command\n");
			} else if (err != ESP_OK) {
				printf("Command execute error: %s\n", esp_err_to_name(err));
			}

			vTaskDelay(50 / portTICK_PERIOD_MS);
		}
	}

	static void init()
	{
		fflush(stdout);
		fsync(fileno(stdout));
		setvbuf(stdin, NULL, _IONBF, 0);

		const uart_config_t uart_config = {
			115200,
			UART_DATA_8_BITS,
			UART_PARITY_DISABLE,
			UART_STOP_BITS_1,
			UART_HW_FLOWCTRL_DISABLE,
			122,
			static_cast<uart_sclk_t>(4),
			{0, 0}
			};

		uart_driver_install(UART_NUM_0, 148, 148, 0, NULL, 0);
		uart_param_config(UART_NUM_0, &uart_config);
		uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
		uart_vfs_dev_use_driver(UART_NUM_0);

		uart_vfs_dev_port_set_rx_line_endings(UART_NUM_0, ESP_LINE_ENDINGS_CR);
		uart_vfs_dev_port_set_tx_line_endings(UART_NUM_0, ESP_LINE_ENDINGS_CRLF);
		linenoiseSetMultiLine(1);
		linenoiseSetCompletionCallback(&esp_console_get_completion);
		linenoiseSetHintsCallback((linenoiseHintsCallback *) &esp_console_get_hint);
		linenoiseSetMaxLineLen(128);  // как в console_config
		linenoiseAllowEmpty(false);

		if (linenoiseProbe()) {
			linenoiseSetDumbMode(1);
		}
			esp_console_config_t console_config = {
				.max_cmdline_length = 128,
				.max_cmdline_args = 8,
				.heap_alloc_caps = MALLOC_CAP_SPIRAM,
				.hint_color = 32,
				.hint_bold = 0};

		esp_console_init(&console_config);
		linenoiseSetCompletionCallback(NULL);

		initCommandTable();
		xTaskCreate(task, "CommandLine", 8 * 1024, nullptr, 5, &taskHandle);
	}
private:

	static int setFilTimerFunc(int argc, char **argv)
	{
		if (argc < 2) {
			printf("Usage: set_ph <secs>\n");
			return ESP_ERR_INVALID_ARG;
		}
		int secs = atoi(argv[1]);
		if (secs < 0) {
			return ESP_ERR_INVALID_ARG;
		}

		Event ev;
		ev.type = EventType::SettingsUpdated;
		ev.data.settings = settings;
		ev.data.settings.pump.maxFloodingTime = secs;
		EventBus::throwEvent(&ev);
		return ESP_OK;
	}

	static int reboot_func(int argc, char **argv)
	{
		vTaskDelete(taskHandle);  // Убить текущую задачу
		esp_restart();
		return ESP_OK;
	}

	static void initCommandTable()
	{
		esp_console_cmd_t cmd_table[] = {
			{
				"set_filtimer",
				"Set water filling timer",
				"<secs>",
				&setFilTimerFunc,
				NULL,
				NULL,
				NULL
			},
			{
				"reboot",
				"Reboot system",
				NULL,
				&reboot_func,
				NULL,
				NULL,
				NULL
			}};

		for (auto &cmd : cmd_table) {
			esp_console_cmd_register(&cmd);
		}
	}
};

Settings Cli::settings;
TaskHandle_t Cli::taskHandle;

#endif