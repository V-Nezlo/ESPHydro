/*!
@file
@brief ООП обертка для серийника
@author V-Nezlo (vlladimirka@gmail.com)
@date 17.04.2024
@version 1.0
*/

#include "MutexLock.hpp"
#include "SerialWrapper.hpp"
#include <esp_log.h>

SerialWrapper::SerialWrapper(int8_t aPort, size_t aTxBufSize, size_t aRxBufSize, int aTxPin, int aRxPin, Gpio &aLatch) :
	port{static_cast<uart_port_t>(aPort)},
	config{
		115200,
		UART_DATA_8_BITS,
		UART_PARITY_DISABLE,
		UART_STOP_BITS_1,
		UART_HW_FLOWCTRL_DISABLE,
		122,
		UART_SCLK_DEFAULT,
		{0,0}
	},
	latch{aLatch},
	mutex{xSemaphoreCreateMutex()}
{
	ESP_ERROR_CHECK(uart_param_config(port, &config));
	ESP_ERROR_CHECK(uart_set_pin(port, aTxPin, aRxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
	ESP_ERROR_CHECK(uart_driver_install(port, aRxBufSize, aTxBufSize, 10, &queue, 0));
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

	ESP_LOGV("SW", "%.*s", readed, aBuffer);

	ESP_ERROR_CHECK(uart_flush(port));
	return readed;
}

ssize_t SerialWrapper::write(const uint8_t *aData, size_t aLength)
{
	MutexLock lock(mutex);

	ESP_LOGV("SW", "%.*s", aLength, aData);
	latch.set();
	const auto result = uart_write_bytes(port, aData, aLength);
	ESP_ERROR_CHECK(uart_wait_tx_done(port, 1000'000));
	latch.reset();

	return result;
}
