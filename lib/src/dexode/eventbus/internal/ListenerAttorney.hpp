//
// Created by gelldur on 30.10.2019.
//
#pragma once

//#include "dexode/EventBus.hpp"

namespace dexode::eventbus
{
template <typename>
class Listener;
} // namespace dexode::eventbus

namespace dexode::eventbus::internal
{

template <typename EventBus_t>
class ListenerAttorney
{
	template <typename>
	friend class dexode::eventbus::Listener;

private:
	static constexpr std::uint32_t newListenerID(EventBus_t& bus)
	{
		return bus.newListenerID();
	}

	template <class Event>
	static constexpr void listen(EventBus_t& bus,
								 const std::uint32_t listenerID,
								 std::function<void(const Event&)>&& callback)
	{
		bus.template listen<Event>(listenerID,
								   std::forward<std::function<void(const Event&)>>(callback));
	}

	static constexpr void unlistenAll(EventBus_t& bus, const std::uint32_t listenerID)
	{
		bus.unlistenAll(listenerID);
	}

	template <typename Event>
	static constexpr void unlisten(EventBus_t& bus, const std::uint32_t listenerID)
	{
		bus.template unlisten<Event>(listenerID);
	}
};

} // namespace dexode::eventbus::internal
