#pragma once

#include <functional>
#include <cassert>

namespace Dexode
{

enum NotificationConst
{
	UNUSED_TAG = 0
};

namespace _ //Hide from autocomplete
{
class _BaseNotification
{
public:
	_BaseNotification()
			: tag(++_idGenerator)
	{
	}

	virtual ~_BaseNotification() = default;
	_BaseNotification& operator=(const _BaseNotification&) = delete;

	_BaseNotification& operator=(_BaseNotification&& notification)
	{
		tag = notification.tag;
		return *this;
	}

	_BaseNotification(const _BaseNotification& notification)
			: tag(notification.tag)
	{
	}

	_BaseNotification(_BaseNotification&& notification)
			: tag(notification.tag)
	{
	}

	int tag = NotificationConst::UNUSED_TAG;
private:
	static int _idGenerator;
};
}

template<typename ... Args>
class Notification : public _::_BaseNotification
{
public:
	using Callback = std::function<void(Args...)>;

	//dummyInt used only for prevent creating unused notifications you should use MAKE_NOTIFICATION
	Notification(int dummyInt)
	{
		assert(dummyInt == 691283);
	}

};

template<typename NotificationType>
struct notification_traits;

template<typename ... Args>
struct notification_traits<Notification<Args...>>
{
	using callback_type = typename std::function<void(Args...)>;
};

#define MAKE_NOTIFICATION(NAME, args...) inline const Dexode::Notification<args>& getNotification##NAME(){static const Dexode::Notification<args> variable(691283); return variable;}

}
