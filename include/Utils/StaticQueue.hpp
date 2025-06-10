/*!
@file
@brief Очередь на стеке
@author V-Nezlo (vlladimirka@gmail.com)
@date 05.06.2025
@version 1.0
*/

#ifndef INCLUDE_UTILS_STATIC_QUEUE_HPP_
#define INCLUDE_UTILS_STATIC_QUEUE_HPP_

#include <cstddef>
#include <array>

template<typename T, size_t Size>
class StaticQueue {
public:
	StaticQueue() : head(0), tail(0), count(0) {}

	bool push(const T& item) {
		if (count >= Size) {
			return false;
		}

		buffer[tail] = item;
		tail = (tail + 1) % Size;
		++count;
		return true;
	}

	bool pop(T& item) {
		if (count == 0) {
			return false;
		} 
		
		item = buffer[head];
		head = (head + 1) % Size;
		--count;
		return true;
	}

	bool isEmpty() const {
		return count == 0;
	}

	bool isFull() const {
		return count >= Size;
	}

	size_t size() const {
		return count;
	}

	void clear() {
		head = tail = count = 0;
	}

	bool contains(const T& item) const 
	{
		for (size_t i = 0; i < count; ++i) {
			if (buffer[(head + i) % Size] == item) {
				return true;
			}
		}

		return false;
	}

	T front() const 
	{
		return buffer[head];
	}

private:
	std::array<T, Size> buffer;
	size_t head;
	size_t tail;
	size_t count;
};

#endif // INCLUDE_UTILS_STATIC_QUEUE_HPP_