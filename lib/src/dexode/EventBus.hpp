#pragma once

#include <atomic>
#include <cassert>
#include <functional>
#include <limits>

#include "dexode/eventbus/Listener.hpp"
#include "dexode/eventbus/internal/common.h"

namespace dexode
{

template <class Strategy>
class EventBus
{
	template <typename>
	friend class dexode::eventbus::internal::ListenerAttorney;

public:
	using Listener = eventbus::Listener<EventBus<Strategy>>;

	constexpr EventBus() = default;
	~EventBus() = default;

	EventBus(const EventBus&) = delete;
	EventBus(EventBus&&) = delete;

	EventBus& operator=(EventBus&&) = delete;
	EventBus& operator=(const EventBus&) = delete;

	template <typename Event>
	constexpr void post(const Event& event)
	{
		static_assert(eventbus::internal::validateEvent<Event>(), "Invalid event");
		_base.template post<Event>(event);
	}

	template <typename Event>
	constexpr void postpone(Event event)
	{
		static_assert(eventbus::internal::validateEvent<Event>(), "Invalid event");
		_base.template postpone<Event>(std::move(event));
	}

	constexpr std::size_t processAll()
	{
		return processLimit(std::numeric_limits<std::size_t>::max());
	}

	constexpr std::size_t processLimit(const std::size_t maxCountOfEvents)
	{
		return _base.processLimit(maxCountOfEvents);
	}

	[[nodiscard]] constexpr std::size_t getPostponeEventCount() const
	{
		return _base.getQueueEventCount();
	}

	Strategy& getStrategy()
	{
		return _base;
	}

private:
	std::atomic<std::uint32_t> _lastID{0};
	Strategy _base;

	std::uint32_t newListenerID()
	{
		return ++_lastID;
	}

	template <class Event>
	constexpr void listen(const std::uint32_t listenerID,
						  std::function<void(const Event&)>&& callback)
	{
		static_assert(eventbus::internal::validateEvent<Event>(), "Invalid event");
		assert(callback && "callback should be valid"); // Check for valid object

		_base.template listen<Event>(listenerID,
									 std::forward<std::function<void(const Event&)>>(callback));
	}

	constexpr void unlistenAll(const std::uint32_t listenerID)
	{
		_base.unlistenAll(listenerID);
	}

	template <typename Event>
	constexpr void unlisten(const std::uint32_t listenerID)
	{
		static_assert(eventbus::internal::validateEvent<Event>(), "Invalid event");
		const auto eventID = eventbus::internal::event_id<Event>;
		_base.template unlisten(listenerID, eventID);
	}
};

} // namespace dexode
