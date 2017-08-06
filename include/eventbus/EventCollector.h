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
	EventCollector(const std::shared_ptr<EventBus>& notifier);
	EventCollector(EventBus* notifier);
	EventCollector(EventCollector const& other);
	EventCollector(EventCollector&& other);

	~EventCollector();

	EventCollector& operator=(EventCollector const& other);
	EventCollector& operator=(EventCollector&& other);

	/**
	 * Register listener for notification.
	 *
	 * @param notification - pass notification like "getNotificationXYZ()"
	 * @param callback - your callback to handle notification
	 */
	template<typename ... Args>
	void listen(const Event<Args...>& notification
				, typename eventbus_traits<const std::function<void(Args...)>&>::type callback)
	{
		if (!callback || !_notifier)
		{
			return;//Skip such things
		}
		if (_token == 0)
		{
			_token = _notifier->listen(notification, callback);
		}
		else
		{
			_notifier->listen(_token, notification, callback);
		}
	}

	/**
	 * Register listener for notification. Returns token used for unlisten
	 *
	 * @param notification - name of your notification
	 * @param callback - your callback to handle notification
	 * @return token used for unlisten
	 */
	template<typename ... Args>
	void listen(const std::string& notificationName
				, typename eventbus_traits<const std::function<void(Args...)>&>::type callback)
	{
		listen(Dexode::Event<Args...>{notificationName}, callback);
	}

	void unlistenAll();

	/**
	 * @param notification - notification you wan't to unlisten. @see Notiier::listen
	 */
	template<typename NotificationType, typename ... Args>
	void unlisten(const NotificationType& notification)
	{
		if (_notifier)
		{
			_notifier->unlisten(_token, notification);
		}
	}

private:
	int _token = 0;
	std::shared_ptr<EventBus> _notifier;
};

}
