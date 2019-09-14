#pragma once

#include <deque>
#include <map>
#include <memory>
#include <vector>

#include "dexode/EventBus.hpp"
#include "eventbus/internal/TransactionCallbackVector.h"

namespace dexode::eventbus::strategy
{

class Transaction
{
public:
	Transaction() = default;
	~Transaction() = default;

	Transaction(const Transaction&) = delete;
	Transaction(Transaction&&) = delete;

	Transaction& operator=(Transaction&&) = delete;
	Transaction& operator=(const Transaction&) = delete;

	template <typename Event>
	void post(const Event& event)
	{
		using Vector = Dexode::Internal::TransactionCallbackVector<Event>;
		auto found = _callbacks.find(Dexode::Internal::event_id<Event>());
		if(found == _callbacks.end())
		{
			return; // no such notifications
		}

		std::unique_ptr<Dexode::Internal::CallbackVector>& vector = found->second;
		assert(dynamic_cast<Vector*>(vector.get()));
		auto* vectorImpl = static_cast<Vector*>(vector.get());

		vectorImpl->beginTransaction();
		for(const auto& element : vectorImpl->container)
		{
			element.second(event);
		}
		vectorImpl->commitTransaction();
	}

	template <typename Event>
	void postpone(Event&& event)
	{
		_eventQueue.push_back([this, event = std::forward<Event>(event)]() { post<Event>(event); });
	}

	std::size_t processLimit(const std::size_t maxCountOfEvents)
	{
		std::size_t processed = 0;
		while(processed < maxCountOfEvents && not _eventQueue.empty())
		{
			auto asyncCallback = std::move(_eventQueue.front());
			_eventQueue.pop_front();
			// Needs to be done in this way. Think about recursion in this case...
			asyncCallback();
			++processed;
		}
		return processed;
	}

	[[nodiscard]] std::size_t getQueueEventCount() const noexcept
	{
		return _eventQueue.size();
	}

	template <class Event>
	void listen(const std::uint32_t listenerID, std::function<void(const Event&)>&& callback)
	{
		using Vector = Dexode::Internal::TransactionCallbackVector<Event>;

		std::unique_ptr<Dexode::Internal::CallbackVector>& vector =
			_callbacks[Dexode::Internal::event_id<Event>()];
		if(vector == nullptr)
		{
			vector = std::make_unique<Vector>();
		}
		assert(dynamic_cast<Vector*>(vector.get()));
		auto* vectorImpl = static_cast<Vector*>(vector.get());
		vectorImpl->add(listenerID, std::forward<std::function<void(const Event&)>>(callback));
	}

	void unlistenAll(const std::uint32_t listenerID)
	{
		for(auto& element : _callbacks)
		{
			element.second->remove(listenerID);
		}
	}

	template <typename Event>
	void unlisten(const std::uint32_t listenerID)
	{
		static_assert(Dexode::Internal::validateEvent<Event>(), "Invalid event");

		auto found = _callbacks.find(Dexode::Internal::event_id<Event>());
		if(found != _callbacks.end())
		{
			found->second->remove(listenerID);
		}
	}

private:
	std::map<Dexode::Internal::event_id_t, std::unique_ptr<Dexode::Internal::CallbackVector>>
		_callbacks;
	std::deque<std::function<void()>> _eventQueue;
};

} // namespace dexode::eventbus::strategy
