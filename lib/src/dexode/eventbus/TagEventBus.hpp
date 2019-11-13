//
// Created by gelldur on 30.10.2019.
//
#pragma once

#include <map>
#include <memory>
#include <string>

#include "dexode/EventBus.hpp"

namespace dexode::eventbus
{

template <class Strategy>
class TagEventBus
{
	template <typename>
	friend class dexode::eventbus::internal::ListenerAttorney;

	using EventBus_t = EventBus<Strategy>;

public:
	using ListenerAll = eventbus::Listener<TagEventBus<Strategy>>;
	using Listener = typename EventBus<Strategy>::Listener; // alias

	TagEventBus(const std::vector<std::string>& tags)
	{
		for(const auto& tag : tags)
		{
			_tagToBus.emplace(tag, std::make_shared<EventBus_t>());
		}
	}

	~TagEventBus() = default;

	TagEventBus(const TagEventBus&) = delete;
	TagEventBus(TagEventBus&&) = delete;

	TagEventBus& operator=(TagEventBus&&) = delete;
	TagEventBus& operator=(const TagEventBus&) = delete;

	template <typename Event>
	void post(const Event& event)
	{
		_allBus.post(event);
		for(auto& element : _tagToBus)
		{
			element.second->post(event);
		}
	}

	template <typename Event>
	void postpone(Event event)
	{
		_allBus.postpone(event);
		for(auto& element : _tagToBus)
		{
			element.second->postpone(event);
		}
	}

	template <typename Event>
	void post(const std::string& tag, const Event& event)
	{
		_allBus.post(event);
		_tagToBus.at(tag)->post(event);
	}

	template <typename Event>
	void postpone(const std::string& tag, Event event)
	{
		_allBus.postpone(event);
		_tagToBus.at(tag)->postpone(event);
	}

	constexpr std::size_t processAll()
	{
		return processLimit(std::numeric_limits<std::size_t>::max());
	}

	constexpr std::size_t processLimit(const std::size_t maxCountOfEvents)
	{
		return _allBus.processLimit(maxCountOfEvents);
	}

	const std::shared_ptr<EventBus<Strategy>>& get(const std::string& tag)
	{
		return _tagToBus.at(tag);
	}

private:
	EventBus_t _allBus;
	std::map<std::string, std::shared_ptr<EventBus_t>> _tagToBus;

	constexpr std::uint32_t newListenerID()
	{
		return internal::ListenerAttorney<EventBus_t>::newListenerID(_allBus);
	}

	template <class Event>
	constexpr void listen(const std::uint32_t listenerID,
						  std::function<void(const Event&)> callback)
	{
		internal::ListenerAttorney<EventBus_t>::template listen<Event>(
			_allBus, listenerID, std::move(callback));
	}

	constexpr void unlistenAll(const std::uint32_t listenerID)
	{
		internal::ListenerAttorney<EventBus_t>::unlistenAll(_allBus, listenerID);
	}

	template <typename Event>
	constexpr void unlisten(const std::uint32_t listenerID)
	{
		internal::ListenerAttorney<EventBus_t>::template unlisten<Event>(_allBus, listenerID);
	}
};

} // namespace dexode::eventbus
