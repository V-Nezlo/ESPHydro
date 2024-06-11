/*!
@file
@brief Файл линейного планировщика - просто вызывает process у всех своих детей
@author V-Nezlo (vlladimirka@gmail.com)
@date 16.05.2024
@version 1.0
*/

#ifndef INCLUDE_LINEARSCHED_HPP_
#define INCLUDE_LINEARSCHED_HPP_

#include "TimeWrapper.hpp"
#include <chrono>
#include <vector>

class AbstractLinearTask {
public:
	virtual void process(std::chrono::milliseconds aCurrentTime) = 0;
};

class LinearSched {
public:
	LinearSched() = default;

	void registerTask(AbstractLinearTask *aTask)
	{
		const auto iter = std::find(tasks.begin(), tasks.end(), aTask);

		if (iter == tasks.end()) {
			tasks.push_back(aTask);
		}
	}

	void doTasks()
	{
		for (auto &task : tasks) {
			const auto time = TimeWrapper::milliseconds();
			task->process(time);
		}
	}

private:
	std::vector<AbstractLinearTask *> tasks;
};

#endif // INCLUDE_LINEARSCHED_HPP_