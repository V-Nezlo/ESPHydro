#ifndef SERIALWRAPPER_HPP
#define SERIALWRAPPER_HPP

#include <iostream>
#include <cstddef>
#include <cstdint>
#include <string.h>
#include <vector>
#include <driver/uart.h>

class SerialWrapper {
public:
	SerialWrapper(uart_port_t aPort, size_t aTxBufSize, size_t aRxBufSize, int aTxPin, int aRxPin) :
        port{aPort},
        config{
            .baud_rate = 115200,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
            .rx_flow_ctrl_thresh = 122
        }
	{
        ESP_ERROR_CHECK(uart_param_config(aPort, &config));
        ESP_ERROR_CHECK(uart_set_pin(aPort, aTxPin, aRxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
        ESP_ERROR_CHECK(uart_driver_install(aPort, aRxBufSize, aTxBufSize, 10, &queue, 0));
	}

    // Interfaces
	size_t bytesAvaillable()
	{
        size_t availlableDataSize{0};
		uart_get_buffered_data_len(port, &availlableDataSize);
        return availlableDataSize;
	}

	size_t read(uint8_t *aBuffer, size_t aLength)
	{
        size_t availlableDataSize{0};
		uart_get_buffered_data_len(port, &availlableDataSize);
        uart_read_bytes(port, aBuffer, availlableDataSize, 100);
        uart_flush(port);
        return availlableDataSize;
	}

	ssize_t write(const uint8_t *aData, size_t aLength)
	{
        return uart_write_bytes(port, aData, aLength);
	}

private:
    const uart_port_t port;
	const uart_config_t config;
    QueueHandle_t queue;
};

#endif // SERIALWRAPPER_HPP
