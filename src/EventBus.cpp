#include "EventBus.hpp"
#include <vector>

std::vector<AbstractEventObserver *> EventBus::observers;
std::queue<std::pair<Event, AbstractEventObserver *>> EventBus::queue;