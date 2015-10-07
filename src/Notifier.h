#pragma once

#include "Notification.h"

#include <vector>
#include <functional>
#include <map>

namespace Dexode
{

class Notifier
{
public:
	Notifier()
	{
	}

	virtual ~Notifier()
	{
		for (auto&& it = _callbacks.begin(); it != _callbacks.end(); ++it)
		{
			delete it->second;
		}
	}

	Notifier& operator=(const Notifier&) = delete;

	Notifier(const Notifier&) = delete;

	Notifier& operator=(Notifier&&) = delete;

	Notifier(Notifier&&) = delete;

	static Notifier& getGlobal()
	{
		static Notifier globalNotifier;
		return globalNotifier;
	}

	template<typename ... Args>
	void listen(const int token
				, const Notification<Args...>& notification
				, const std::function<void(Args...)>& callback)
	{
		using CallbackType = std::function<void(Args...)>;

		//Check for valid object
		assert(callback && "Please set it");

		if (_callbacks[notification.tag] == nullptr)
		{
			auto pVector = new VectorImpl<CallbackType>();
			pVector->container.reserve(4);

			_callbacks[notification.tag] = pVector;
		}

		auto pVector = static_cast<VectorImpl <CallbackType>*>(_callbacks[notification.tag]);
		pVector->container.emplace_back(std::make_pair(callback, token));
	}

	void unlistenAll(const int token)
	{
		for (auto&& element : _callbacks)
		{
			element.second->remove(token);
		}
	}

	template<typename NotificationType, typename ... Args>
	void unlisten(const int token, const NotificationType& notification)
	{
		using CallbackType = typename notification_traits<NotificationType>::callback_type;
		assert(notification.tag > NotificationConst::UNUSED_TAG);

		if (_callbacks[notification.tag] == nullptr)
		{
			return;
		}
		assert(dynamic_cast<VectorImpl <CallbackType>*>(_callbacks[notification.tag]) != nullptr);
		_callbacks[notification.tag]->remove(token);
	}

	template<typename NotificationType, typename ... Args>
	void notify(const NotificationType& notification, Args&& ... params)
	{
		using CallbackType = typename notification_traits<NotificationType>::callback_type;
		assert(notification.tag > NotificationConst::UNUSED_TAG);

		if (_callbacks[notification.tag] == nullptr)
		{
			return;
		}

		assert(dynamic_cast<VectorImpl <CallbackType>*>(_callbacks[notification.tag]) != nullptr);

		//Copy? TODO think about it Use 2 vectors?
		auto pVector = static_cast<VectorImpl <CallbackType>*>(_callbacks[notification.tag]);
		for (auto&& element : pVector->container)
		{
			element.first(std::forward<Args>(params)...);
		}
	}

////////////////////////////////////////////////////////////////////////////////////////////////////
private:

	struct VectorInterface
	{
		virtual ~VectorInterface()
		{
		}

		virtual void remove(const int token) = 0;
		virtual void removeAll() = 0;
	};

	template<typename Type>
	struct VectorImpl : public VectorInterface
	{
		std::vector<std::pair<Type, int>> container;

		virtual ~VectorImpl()
		{
		}

		virtual void removeAll() override
		{
			container.clear();
		}

		virtual void remove(const int token) override
		{
			for (int i = container.size() - 1; i > -1; --i)
			{
				if (container[i].second == token)
				{
					std::swap(container[i], container.back());
					container.pop_back();
				}
			}
		}
	};

	std::map<int, VectorInterface*> _callbacks;
};

} /* namespace Dexode */
