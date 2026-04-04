#include "Types.hpp"

namespace Helpers {

/// @brief Функция написана ChatGPT
/// @param currentTime
/// @param startTime
/// @param endTime
/// @return True если время находится в промежутке для включения, false - если для выключения
static inline bool isTimeForOn(const Time &currentTime, const Time &startTime, const Time &endTime)
{
	// Helper function to convert time to total seconds for easier comparison
	auto timeToSeconds = [](const Time &t) -> int
	{
		return t.hour * 3600 + t.minutes * 60 + t.seconds;
	};

	int current = timeToSeconds(currentTime);
	int start = timeToSeconds(startTime);
	int end = timeToSeconds(endTime);

	// Case 1: Same day operation (e.g., 08:00 to 18:00)
	if (start < end)
	{
		return current >= start && current < end;
	}
	// Case 2: Overnight operation (e.g., 18:00 to 08:00 next day)
	else if (start > end)
	{
		return current >= start || current < end;
	}
	// Case 3: Same time for start and end (edge case - should probably be disabled)
	else
	{
		return false;
	}
}

} // namespace Helpers