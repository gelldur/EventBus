//
// Created by gelldur on 26.11.2019.
//
#include "EventBus.hpp"

#include <iterator>

namespace dexode
{

std::size_t EventBus::processLimit(const std::size_t limit)
{
	std::size_t processCount{0};
	std::lock_guard writeGuardProcess{_mutexProcess}; // Only one process at the time

	std::vector<std::unique_ptr<eventbus::stream::EventStream>> eventStreams;
	{
		std::lock_guard writeGuard{_mutexStreams};
		std::swap(eventStreams, _eventStreams); // move data FROM member
	}

	// Now if any setStream would be called it doesn't conflict without our process call
	for(auto& eventStream : eventStreams)
	{
		const auto runProcessCount = eventStream->process(limit);
		processCount += runProcessCount;
		if(processCount >= limit)
		{
			break;
		}
	}

	{
		std::lock_guard writeGuard{_mutexStreams};
		if(!_eventStreams.empty())
		{
			// If anything was added then we need to add those elements
			std::move(_eventStreams.begin(), _eventStreams.end(), std::back_inserter(eventStreams));
		}
		std::swap(eventStreams, _eventStreams); // move data TO member

		// Check do we need remove something
		if(_eventStreams.size() != _eventToStream.size())
		{
			auto removeFrom = std::remove_if(
				_eventStreams.begin(), _eventStreams.end(), [this](const auto& eventStream) {
					for(const auto& element : _eventToStream)
					{
						// Don't remove if we point to the same place (is it UB ?)
						if(element.second == eventStream.get())
						{
							return false;
						}
					}
					return true;
				});
			assert(removeFrom != _eventStreams.end());
			_eventStreams.erase(removeFrom, _eventStreams.end());
		}
	}

	return processCount;
}

eventbus::stream::EventStream* EventBus::findStream(
	const eventbus::internal::event_id_t eventID) const
{
	std::shared_lock readGuard{_mutexStreams};
	return findStreamUnsafe(eventID);
}

void EventBus::unlistenAll(const std::uint32_t listenerID)
{
	std::shared_lock readGuard{_mutexStreams};
	for(const auto& eventStream : _eventToStream)
	{
		eventStream.second->removeListener(listenerID);
	}
}

eventbus::stream::EventStream* EventBus::findStreamUnsafe(
	const eventbus::internal::event_id_t eventID) const
{
	auto lookup = _eventToStream.find(eventID);
	return lookup != _eventToStream.end() ? lookup->second : nullptr;
}

eventbus::stream::EventStream* EventBus::obtainStream(
	const eventbus::internal::event_id_t eventID,
	eventbus::CreateStreamCallback createStreamCallback)
{
	std::lock_guard writeGuard{_mutexStreams};
	auto* found = findStreamUnsafe(eventID);
	if(found != nullptr)
	{
		return found;
	}
	else
	{
		auto stream = createStreamCallback();
		_eventStreams.push_back(std::move(stream));
		_eventToStream[eventID] = _eventStreams.back().get();
		return _eventStreams.back().get();
	}
}

bool EventBus::postponeEvent(eventbus::PostponeHelper& postponeCall)
{
	auto* eventStream = obtainStream(postponeCall.eventID, postponeCall.createStreamCallback);
	eventStream->postpone(std::move(postponeCall.event));
	return true;
}

eventbus::stream::EventStream* EventBus::listen(const std::uint32_t,
												const eventbus::internal::event_id_t eventID,
												eventbus::CreateStreamCallback createStreamCallback)
{
	auto* eventStream = obtainStream(eventID, createStreamCallback);
	return eventStream;
}

void EventBus::unlisten(const std::uint32_t listenerID,
						const eventbus::internal::event_id_t eventID)
{
	eventbus::stream::EventStream* eventStream = findStream(eventID);
	if(eventStream != nullptr)
	{
		eventStream->removeListener(listenerID);
	}
}

} // namespace dexode
