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

class Cli : public AbstractLinearTask, public AbstractEventObserver {
	std::chrono::milliseconds nextCallTime;
	static Settings settings;
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

	void process(std::chrono::milliseconds aCurrentTime) override
	{
		if (aCurrentTime < nextCallTime) {
			return;
		}

		nextCallTime = aCurrentTime + std::chrono::milliseconds{50};

		char* line = linenoise(">> ");
		if (line == NULL) return;

		// RAII
		std::unique_ptr<char, decltype(&free)> ptr(line, &free);

		int ret;
		esp_err_t err = esp_console_run(line, &ret);
		if (err == ESP_ERR_NOT_FOUND) {
			printf("Unknown command\n");
		} else if (err != ESP_OK) {
			printf("Command execute error: %s\n", esp_err_to_name(err));
		}
	}

	static void init()
	{
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

		uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);
		uart_param_config(UART_NUM_0, &uart_config);
		uart_set_pin(UART_NUM_0, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
		uart_vfs_dev_use_driver(UART_NUM_0);

			esp_console_config_t console_config = {
				.max_cmdline_length = 256,
				.max_cmdline_args = 8,
				.heap_alloc_caps = MALLOC_CAP_SPIRAM,
				.hint_color = 32,
				.hint_bold = 0};

		esp_console_init(&console_config);
		linenoiseSetCompletionCallback(NULL);
		linenoiseHistorySetMaxLen(100);

		initCommandTable();
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

#endif