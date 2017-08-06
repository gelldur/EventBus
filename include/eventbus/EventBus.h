#pragma once

#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <vector>
#include <cassert>

#include "Event.h"

#if __cplusplus < 201402L
	#error This library needs at least a C++14 compliant compiler
#endif

namespace Dexode
{

template<typename T>
struct eventbus_traits
{
	typedef T type;
};

class EventBus
{
public:
	EventBus() = default;

	~EventBus()
	{
		_callbacks.clear();
	}

	EventBus(const EventBus&) = delete;
	EventBus(EventBus&&) = delete;

	EventBus& operator=(EventBus&&) = delete;
	EventBus& operator=(const EventBus&) = delete;

	/**
	 * Register listener for notification. Returns token used for unlisten
	 *
	 * @param event
	 * @param callback - your callback to handle notification
	 * @return token used for unlisten
	 */
	template<typename ... Args>
	int listen(const Event<Args...>& event
			   , typename eventbus_traits<const std::function<void(Args...)>&>::type callback)
	{
		const int token = ++_tokener;
		listen(token, event, callback);
		return token;
	}

	/**
	 * Register listener for notification. Returns token used for unlisten
	 *
	 * @param eventName - name of your event
	 * @param callback - your callback to handle notification
	 * @return token used for unlisten
	 */
	template<typename ... Args>
	int listen(const std::string& eventName
			   , typename eventbus_traits<const std::function<void(Args...)>&>::type callback)
	{
		return listen(Dexode::Event<Args...>{eventName}, callback);
	}

	/**
	 * @param token - unique token for identification receiver. Simply pass token from @see EventBus::listen
	 * @param event - pass notification like "getNotificationXYZ()"
	 * @param callback - your callback to handle notification
	 */
	template<typename ... Args>
	void listen(const int token
				, const Event<Args...>& event
				, typename eventbus_traits<const std::function<void(Args...)>&>::type callback)
	{
		using CallbackType = std::function<void(Args...)>;
		using Vector = VectorImpl<CallbackType>;

		assert(callback && "Please set it");//Check for valid object

		std::unique_ptr<VectorInterface>& vector = _callbacks[event.getKey()];
		if (vector == nullptr)
		{
			vector.reset(new Vector{});
		}

		assert(dynamic_cast<Vector*>(vector.get()));
		Vector* vectorImpl = static_cast<Vector*>(vector.get());
		vectorImpl->container.emplace_back(std::make_pair(callback, token));
	}

	/**
	 * @param token - token from EventBus::listen
	 */
	void unlistenAll(const int token)
	{
		for (auto& element : _callbacks)
		{
			element.second->remove(token);
		}
	}

	/**
	 * @param token - token from EventBus::listen
	 * @param event - notification you wan't to unlisten. @see Notiier::listen
	 */
	template<typename EventType, typename ... Args>
	void unlisten(const int token, const EventType& event)
	{
		auto found = _callbacks.find(event.getKey());
		if (found != _callbacks.end())
		{
			found.second->remove(token);
		}
	}

	template<typename EventType, typename ... Args>
	void notify(const EventType& event, Args&& ... params)
	{
		using CallbackType = typename event_traits<EventType>::callback_type;
		using Vector = VectorImpl<CallbackType>;

		auto found = _callbacks.find(event.getKey());
		if (found == _callbacks.end())
		{
			return;// no such notifications
		}

		std::unique_ptr<VectorInterface>& vector = found->second;
		assert(dynamic_cast<Vector*>(vector.get()));
		Vector* vectorImpl = static_cast<Vector*>(vector.get());

		//Copy? TODO think about it Use 2 vectors?
		for (auto& element : vectorImpl->container)
		{
			element.first(std::forward<Args>(params)...);
		}
	}

	// We need to call this in form like: notify<int>("yes",value)
	// We can't reduce it to notify("yes",value)
	// It wouldn't be obvious which to call Event<int> or Event<int&>
	// So it can take to a lot of mistakes
	template<typename ... Args>
	void notify(const std::string& eventName, Args&& ... params)
	{
		notify(Dexode::Event<Args...>{eventName}, std::forward<Args>(params)...);
	}

private:
	struct VectorInterface
	{
		virtual ~VectorInterface() = default;

		virtual void remove(const int token) = 0;
		virtual void removeAll() = 0;
	};

	template<typename Type>
	struct VectorImpl : public VectorInterface
	{
		std::vector<std::pair<Type, int>> container;

		virtual ~VectorImpl()
		{
			removeAll();
		}

		virtual void removeAll() override
		{
			container.clear();
		}

		virtual void remove(const int token) override
		{
			auto removeFrom = std::remove_if(container.begin(), container.end()
											 , [token](const std::pair<Type, int>& element)
					{
						return element.second == token;
					});
			if (removeFrom == container.end())
			{
				return;
			}
			container.erase(removeFrom, container.end());
		}
	};

	int _tokener = 0;
	std::map<std::size_t, std::unique_ptr<VectorInterface>> _callbacks;
};

} /* namespace Dexode */
