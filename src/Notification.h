#pragma once

#include <functional>
#include <cassert>

namespace Dexode
{

enum NotificationConst
{
	UNUSED_TAG = 0
};

inline int getUniqueId()
{
	static int id = NotificationConst::UNUSED_TAG + 1;
	return ++id;
}

inline int getToken()
{
	return getUniqueId();
}

template<typename Type>
inline int getTokenForType()
{
	static int id_for_type = getUniqueId();
	return id_for_type;
}

template<typename ... Args>
class Notification
{
public:
	using Callback = std::function<void(Args...)>;

	//dummyInt used only for prevent creating unused notifications you should use MAKE_NOTIFICATION
	Notification(int dummyInt)
			: tag(getUniqueId())
	{
		assert(dummyInt == 691283);
	}

	Notification& operator=(const Notification&) = delete;

	Notification& operator=(Notification&& notification)
	{
		tag = notification.tag;
		return *this;
	}

	Notification(const Notification& notification)
			: tag(notification.tag)
	{
	}

	Notification(Notification&& notification)
			: tag(notification.tag)
	{
	}

	int tag = NotificationConst::UNUSED_TAG;
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
