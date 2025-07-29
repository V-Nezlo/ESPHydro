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
	std::string inputBuffer;

public:
	Cli() : nextCallTime(std::chrono::milliseconds{0})
	{
		inputBuffer.reserve(64);
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
		char buffer[256];
		esp_task_wdt_add(NULL);

		std::string *inBuffer = reinterpret_cast<std::string *>(pvParameters);

		while(true) {
			int len = uart_read_bytes(UART_NUM_0, buffer, sizeof(buffer) - 1, 10 / portTICK_PERIOD_MS);

			if (len > 0) {
				buffer[len] = '\0';

				for (int i = 0; i < len; i++) {
					char c = buffer[i];

					if (c == '\r' || c == '\n') {
						if (!inBuffer->empty()) {
							processCommand(inBuffer->c_str());
							inBuffer->clear();
							printf(">> ");
							fflush(stdout);
						}
					} else if (c == '\b' || c == 127) {
						if (!inBuffer->empty()) {
							inBuffer->pop_back();
							printf("\b \b");
							fflush(stdout);
						}
					} else if (c >= 32 && c <= 126) {
						*inBuffer += c;
						printf("%c", c);
						fflush(stdout);
					}
				}
			}

			esp_task_wdt_reset();
			vTaskDelay(50 / portTICK_PERIOD_MS);
		}
	}

	static void processCommand(const char* line)
	{
		if (line == nullptr || strlen(line) == 0) {
			return;
		}

		int ret;
		esp_err_t err = esp_console_run(line, &ret);
		if (err == ESP_ERR_NOT_FOUND) {
			printf("\r\n");
			printf("Unknown command\n");
		} else if (err != ESP_OK) {
			printf("\r\n");
			printf("Command execute error: %s\n", esp_err_to_name(err));
		}
	}

	static void init()
	{
		initPeriph();
		initLib();

		esp_console_register_help_command();
		initCommandTable();
	}

	void start()
	{
		xTaskCreatePinnedToCore(task, "CommandLine", 12 * 1024, &inputBuffer, 5, &taskHandle, 0);

		printf("CLI ready!\r\n");
		printf(">> ");
		fflush(stdout);
	}

	static void initPeriph()
	{
		fflush(stdout);
		fsync(fileno(stdout));

		uart_vfs_dev_port_set_rx_line_endings(UART_NUM_0, ESP_LINE_ENDINGS_CR);
		uart_vfs_dev_port_set_tx_line_endings(UART_NUM_0, ESP_LINE_ENDINGS_CRLF);

		const uart_config_t uart_config = {
			115200,
			UART_DATA_8_BITS,
			UART_PARITY_DISABLE,
			UART_STOP_BITS_1,
			UART_HW_FLOWCTRL_DISABLE,
			122,
			UART_SCLK_REF_TICK,
			{0, 0}};

		ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, 512, 512, 0, NULL, 0));
		ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));

		setvbuf(stdin, NULL, _IONBF, 0);
	}

	static void initLib()
	{
		esp_console_config_t console_config = {
			.max_cmdline_length = 128,
			.max_cmdline_args = 8,
			.heap_alloc_caps = MALLOC_CAP_DEFAULT,
			.hint_color = 32,
			.hint_bold = 0};

		ESP_ERROR_CHECK(esp_console_init(&console_config));
	}
private:
	// Функции
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
		vTaskDelete(taskHandle);
		esp_restart();
		return ESP_OK;
	}

	static int ph_sensor_state(int argc, char **argv)
	{
		if (argc < 2) {
			printf("Usage: ph_sensor_state <bool>\n");
			return ESP_ERR_INVALID_ARG;
		}

		int value = atoi(argv[1]);
		bool state = static_cast<bool>(value);

		Event ev;
		ev.type = EventType::SettingsUpdated;
		ev.data.settings = settings;
		ev.data.settings.modules.phSensor = state;
		EventBus::throwEvent(&ev);
		return ESP_OK;
	}

	static int ppm_sensor_state(int argc, char **argv)
	{
		if (argc < 2) {
			printf("Usage: ppm_sensor_state <bool>\n");
			return ESP_ERR_INVALID_ARG;
		}

		int value = atoi(argv[1]);
		bool state = static_cast<bool>(value);

		Event ev;
		ev.type = EventType::SettingsUpdated;
		ev.data.settings = settings;
		ev.data.settings.modules.ppmSensor = state;
		EventBus::throwEvent(&ev);
		return ESP_OK;
	}

	static void initCommandTable()
	{
		static const esp_console_cmd_t cmd_table[] = {
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
			},
			{
				"ph_sensor_state",
				"Set PH sensor state",
				"<bool>",
				&ph_sensor_state,
				NULL,
				NULL,
				NULL
			},
			{
				"ppm_sensor_state",
				"Set PPM sensor state",
				"<bool>",
				&ppm_sensor_state,
				NULL,
				NULL,
				NULL
			}
		};

		for (auto &cmd : cmd_table) {
			ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));
		}
	}
};

Settings Cli::settings;
TaskHandle_t Cli::taskHandle;

#endif