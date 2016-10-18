//
// Created by Dawid Drozd aka Gelldur on 18/10/16.
//

#pragma once

#include <memory>

#include "Notifier.h"

namespace Dexode
{

class EventContainer
{
public:
	EventContainer(const std::shared_ptr<Notifier>& notifier);
	EventContainer(Notifier& notifier = Notifier::getGlobal());

	~EventContainer();

	/**
	 * Register listener for notification. Returns token used to unregister
	 *
	 * @param notification - pass notification like "getNotificationXYZ()"
	 * @param callback - your callback to handle notification
	 */
	template<typename ... Args>
	void listen(const Notification<Args...>& notification
			, typename notifier_traits<const std::function<void(Args...)>&>::type callback)
	{
		if (_token == 0)
		{
			_token = _notifier->listen(notification, callback);
		}
		else
		{
			_notifier->listen(_token, notification, callback);
		}
	}

	void unlistenAll();

	/**
	 * @param notification - notification you wan't to unlisten. @see Notiier::listen
	 */
	template<typename NotificationType, typename ... Args>
	void unlisten(const NotificationType& notification)
	{
		_notifier->unlisten(_token, notification);
	}

private:
	int _token = 0;
	std::shared_ptr<Notifier> _notifier;
};

}
