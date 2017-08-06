//
// Created by Dawid Drozd aka Gelldur on 18/10/16.
//

#pragma once

#include <memory>

#include "EventBus.h"

namespace Dexode
{

class EventCollector
{
public:
	EventCollector(const std::shared_ptr<EventBus>& bus);
	EventCollector(EventBus* bus);
	EventCollector(EventCollector const& other);
	EventCollector(EventCollector&& other);

	~EventCollector();

	EventCollector& operator=(EventCollector const& other);
	EventCollector& operator=(EventCollector&& other);

	/**
	 * Register listener for event.
	 *
	 * @param event - you want to listen for
	 * @param callback - your callback to handle event
	 */
	template<typename ... Args>
	void listen(const Event<Args...>& event
				, typename eventbus_traits<const std::function<void(Args...)>&>::type callback)
	{
		if (!callback || !_bus)
		{
			return;//Skip such things
		}
		if (_token == 0)
		{
			_token = _bus->listen(event, callback);
		}
		else
		{
			_bus->listen(_token, event, callback);
		}
	}

	/**
	 * Register listener for notification. Returns token used for unlisten
	 *
	 * @param notification - name of your event
	 * @param callback - your callback to handle event
	 * @return token used for unlisten
	 */
	template<typename ... Args>
	void listen(const std::string& eventName
				, typename eventbus_traits<const std::function<void(Args...)>&>::type callback)
	{
		listen(Dexode::Event<Args...>{eventName}, callback);
	}

	void unlistenAll();

	/**
	 * @param event - notification you wan't to unlisten. @see Notiier::listen
	 */
	template<typename EventType, typename ... Args>
	void unlisten(const EventType& event)
	{
		if (_bus)
		{
			_bus->unlisten(_token, event);
		}
	}

private:
	int _token = 0;
	std::shared_ptr<EventBus> _bus;
};

}
