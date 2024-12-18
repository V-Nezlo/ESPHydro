/*!
@file
@brief ООП обертка для серийника
@author V-Nezlo (vlladimirka@gmail.com)
@date 17.04.2024
@version 1.0
*/

#include "SerialWrapper.hpp"
#include <esp_log.h>

SerialWrapper::SerialWrapper(uart_port_t aPort, size_t aTxBufSize, size_t aRxBufSize, int aTxPin, int aRxPin, Gpio &aLatch) :
	port{aPort},
	config{
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.rx_flow_ctrl_thresh = 122,
		.source_clk = static_cast<uart_sclk_t>(4)
	},
	latch{aLatch},
	mutex{xSemaphoreCreateMutex()}
{
	ESP_ERROR_CHECK(uart_param_config(aPort, &config));
	ESP_ERROR_CHECK(uart_set_pin(aPort, aTxPin, aRxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
	ESP_ERROR_CHECK(uart_driver_install(aPort, aRxBufSize, aTxBufSize, 10, &queue, 0));
}

// Interfaces
size_t SerialWrapper::bytesAvaillable()
{
	size_t availlableDataSize{0};
	ESP_ERROR_CHECK(uart_get_buffered_data_len(port, &availlableDataSize));
	return availlableDataSize;
}

size_t SerialWrapper::read(uint8_t *aBuffer, size_t aLength)
{
	size_t available{0};
	ESP_ERROR_CHECK(uart_get_buffered_data_len(port, &available));
	const ssize_t readed = uart_read_bytes(port, aBuffer, available, 100);
	if (available != readed) {
		ESP_LOGE("Serial", "Serial read op not completely ");
	}

	ESP_LOGV("READING", "%.*s", readed, aBuffer);

	ESP_ERROR_CHECK(uart_flush(port));
	return readed;
}

ssize_t SerialWrapper::write(const uint8_t *aData, size_t aLength)
{
	xSemaphoreTake(mutex, portMAX_DELAY);

	ESP_LOGV("WRITING", "%.*s", aLength, aData);
	latch.set();
	const auto result = uart_write_bytes(port, aData, aLength);
	ESP_ERROR_CHECK(uart_wait_tx_done(port, 1000'000));
	latch.reset();

	xSemaphoreGive(mutex);

	return result;
}
