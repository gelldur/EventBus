#include <eventbus/AsyncEventBus.h>

#include <limits>

namespace Dexode
{

std::size_t AsyncEventBus::processCommandsAndGetQueuedEventsCount()
{
	std::lock_guard<std::mutex> guard {_eventMutex};
	while(_commandsQueue.empty() == false)
	{
		_commandsQueue
			.front()(); //This can't add any extra commands, because in this queue we story only listen/unlisten stuff
		_commandsQueue.pop_front();
	}
	//Yeah we want to return events count. So don't have to call getQueueEventCount
	return _eventQueue.size();
}

int AsyncEventBus::consume(int max)
{
	if(max == 0)
	{
		max = std::numeric_limits<int>::max();
	}

	int consumed = 0;

	std::function<void()> eventCommand;
	while(processCommandsAndGetQueuedEventsCount() > 0 && consumed < max) //order is important
	{
		{
			std::lock_guard<std::mutex> guard {_eventMutex};
			eventCommand = std::move(_eventQueue.front());
			_eventQueue.pop_front();
		}

		eventCommand();
		++consumed;
	}

	return consumed;
}

} // namespace Dexode
