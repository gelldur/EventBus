#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <vector>
#include <shared_mutex>

#include "dexode/eventbus/stream/EventStream.hpp"

namespace dexode::eventbus::stream
{

template <typename Event, typename CallbackReturn = void, typename... ExtraArgTypes>
class ProtectedEventStream : public EventStream
{
	using Callback = std::function<CallbackReturn(const Event&, ExtraArgTypes...)>;

public:
	void postpone(std::any event) override
	{
		auto myEvent = std::any_cast<Event>(event);
		std::lock_guard writeGuard{_mutexEvent};
		_queue.push_back(std::move(myEvent));
	}

	std::size_t process(const std::size_t limit) override
	{
		std::vector<Event> processEvents;
		{
			std::lock_guard writeGuard{_mutexEvent};
			if(limit >= _queue.size())
			{
				processEvents.reserve(_queue.size());
				std::swap(processEvents, _queue);
			}
			else
			{
				const auto countElements = std::min(limit, _queue.size());
				processEvents.reserve(countElements);
				std::move(_queue.begin(),
						  std::next(_queue.begin(), countElements),
						  std::back_inserter(processEvents));
			}
		}

		for(const auto& event : processEvents)
		{
			// At this point we need to consider transaction safety as during some notification
			// we can add/remove listeners
			_isProcessing = true;
			for(auto& callback : _callbacks)
			{
				callback(event);
			}
			_isProcessing = false;

			flushWaitingOnes();
		}

		return processEvents.size();
	}

	bool addListener(const std::uint32_t listenerID, std::any callback) override
	{
		std::lock_guard writeGuard{_mutexCallbacks};
		auto myCallback = std::any_cast<Callback>(callback);
		if(_isProcessing)
		{
			_waiting.emplace_back(listenerID, std::move(myCallback));
			return true;
		}

		return rawAddListener(listenerID, std::move(myCallback));
	}

	bool removeListener(const std::uint32_t listenerID) override
	{
		std::lock_guard writeGuard{_mutexCallbacks};
		if(_isProcessing)
		{
			_waiting.emplace_back(listenerID, Callback{});
			return true;
		}

		return rawRemoveListener(listenerID);
	}

	[[nodiscard]] bool hasEvents() const
	{
		std::shared_lock readGuard{_mutexEvent};
		return not _queue.empty();
	}

private:
	std::vector<std::uint32_t> _listenerIDs;
	std::vector<Event> _queue;
	std::vector<Callback> _callbacks;

	std::atomic<bool> _isProcessing{false};
	std::vector<std::pair<std::uint32_t, Callback>> _waiting;

	std::shared_mutex _mutexEvent;
	std::shared_mutex _mutexCallbacks;

	void flushWaitingOnes()
	{
		assert(_isProcessing == false);
		std::lock_guard writeGuard{_mutexCallbacks};
		if(_waiting.empty())
		{
			return;
		}

		for(auto&& element : _waiting)
		{
			if(element.second) // if callable it means we want to add
			{
				rawAddListener(element.first, std::move(element.second));
			}
			else // if not callable we want to remove
			{
				rawRemoveListener(element.first);
			}
		}
		_waiting.clear();
	}

	bool rawAddListener(const std::uint32_t listenerID, Callback&& callback)
	{
		auto found = std::find(_listenerIDs.begin(), _listenerIDs.end(), listenerID);
		if(found != _listenerIDs.end())
		{
			/// ###### IMPORTANT ######
			/// This exception has some reason.
			/// User should use multiple listeners instead of one. Thanks to that it makes
			/// it more clear what will happen when call unlisten<Event> with specific Event
			throw std::invalid_argument{std::string{"Already added listener for event: "} +
										typeid(Event).name()};
		}

		_callbacks.push_back(std::forward<Callback>(callback));
		_listenerIDs.push_back(listenerID);
		assert(_listenerIDs.size() == _callbacks.size());
		return true;
	}

	bool rawRemoveListener(const std::uint32_t listenerID)
	{
		auto found = std::find(_listenerIDs.begin(), _listenerIDs.end(), listenerID);
		if(found == _listenerIDs.end())
		{
			return false;
		}
		const auto index = std::distance(_listenerIDs.begin(), found);

		_listenerIDs.erase(found);
		_callbacks.erase(std::next(_callbacks.begin(), index));
		assert(_listenerIDs.size() == _callbacks.size());
		return true;
	}
};

} // namespace dexode::eventbus::stream
