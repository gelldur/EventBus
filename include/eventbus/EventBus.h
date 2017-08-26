#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

#if __cplusplus < 201103L
	#error This library needs at least a C++11 compliant compiler
#endif

namespace Dexode
{

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
	 * Register listener for event. Returns token used for unlisten.
	 *
	 * @tparam Event - type you want to listen for
	 * @param callback - your callback to handle event
	 * @return token used for unlisten
	 */
	template<typename Event>
	int listen(const std::function<void(const Event&)>& callback)
	{
		const int token = ++_tokener;
		listen<Event>(token, callback);
		return token;
	}

	/**
	 * @tparam Event - type you want to listen for
	 * @param token - unique token for identification receiver. Simply pass token from @see EventBus::listen
	 * @param callback - your callback to handle event
	 */
	template<typename Event>
	void listen(const int token, const std::function<void(const Event&)>& callback)
	{
		using Vector = VectorImpl<Event>;

		assert(callback && "callback should be valid");//Check for valid object

		std::unique_ptr<VectorInterface>& vector = _callbacks[getTypeId<Event>()];
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
	 * @tparam Event - type you want to unlisten. @see Notiier::listen
	 * @param token - token from EventBus::listen
	 */
	template<typename Event>
	void unlisten(const int token)
	{
		auto found = _callbacks.find(getTypeId<Event>());
		if (found != _callbacks.end())
		{
			found->second->remove(token);
		}
	}

	/**
	 * Notify all listeners for event
	 *
	 * @param event your event struct
	 */
	template<typename Event>
	void notify(const Event& event)
	{
		using Vector = VectorImpl<Event>;
		//		constexpr auto typeId = getTypeId<Event>();
		const auto typeId = getTypeId<Event>();
		auto found = _callbacks.find(typeId);
		if (found == _callbacks.end())
		{
			return;// no such notifications
		}

		std::unique_ptr<VectorInterface>& vector = found->second;
		assert(dynamic_cast<Vector*>(vector.get()));
		Vector* vectorImpl = static_cast<Vector*>(vector.get());

		//TODO think about it Use 2 vectors?
		for (const auto& element : vectorImpl->container)
		{
			element.first(event);
		}
	}

private:
	struct VectorInterface
	{
		virtual ~VectorInterface() = default;

		virtual void remove(const int token) = 0;
		virtual void removeAll() = 0;
	};

	template<typename Event>
	struct VectorImpl : public VectorInterface
	{
		using CallbackType = std::function<void(const Event&)>;
		std::vector<std::pair<CallbackType, int>> container;

		virtual void removeAll() override
		{
			container.clear();
		}

		virtual void remove(const int token) override
		{
			auto removeFrom = std::remove_if(container.begin(), container.end()
											 , [token](const std::pair<CallbackType, int>& element)
					{
						return element.second == token;
					});
			if (removeFrom != container.end())
			{
				container.erase(removeFrom, container.end());
			}
		}
	};

	int _tokener = 0;
	std::map<std::size_t, std::unique_ptr<VectorInterface>> _callbacks;

	template<typename T>
	static const std::size_t getTypeId()
	{
		//std::hash<std::string>{}(typeid(T).name() is slower
		return typeid(T).hash_code();
	}
};

} /* namespace Dexode */
