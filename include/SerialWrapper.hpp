/*!
@file
@brief ООП обертка для серийника
@author V-Nezlo (vlladimirka@gmail.com)
@date 17.04.2024
@version 1.0
*/

#ifndef INCLUDE_SERIALWRAPPER_HPP_
#define INCLUDE_SERIALWRAPPER_HPP_

#include <driver/uart.h>

#include <iostream>
#include <cstddef>
#include <cstdint>
#include <string.h>
#include <vector>


class SerialWrapper {
public:
	SerialWrapper(const SerialWrapper &) = delete;
	SerialWrapper() = delete;
	SerialWrapper(uart_port_t aPort, size_t aTxBufSize, size_t aRxBufSize, int aTxPin, int aRxPin);

	// Interfaces
	size_t bytesAvaillable();
	size_t read(uint8_t *aBuffer, size_t aLength);
	ssize_t write(const uint8_t *aData, size_t aLength);

private:
	const uart_port_t port;
	const uart_config_t config;
	QueueHandle_t queue;
};

#endif // INCLUDE_SERIALWRAPPER_HPP_
