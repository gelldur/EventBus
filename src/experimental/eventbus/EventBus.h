#pragma once

#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <typeinfo>
#include <list>

#if __cplusplus < 201103L
	#error This library needs at least a C++11 compliant compiler
#endif

// This version isn't transaction safe as normal EventBus

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

		if (vectorImpl->container.empty())
		{
			return;
		}

		auto it = vectorImpl->container.begin();
		while (it != vectorImpl->container.end())
		{
			vectorImpl->currentIterator = std::next(it);
			it->first(event);//Order may change eg. some unlisten during call
			it = vectorImpl->currentIterator;
		}
		vectorImpl->currentIterator = vectorImpl->container.end();
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
		using ContainerType = std::list<std::pair<CallbackType, int>>;
		ContainerType container;
		typename ContainerType::iterator currentIterator = container.end();

		//Invalidation rules: https://stackoverflow.com/questions/6438086/iterator-invalidation-rules
		virtual void removeAll() override
		{
			container.clear();
			currentIterator = container.end();
		}

		virtual void remove(const int token) override
		{
			auto removeIfPredicate = [token](const std::pair<CallbackType, int>& element)
			{
				return element.second == token;
			};
			//It is safe to remove
			if (currentIterator == container.end())
			{
				container.remove_if(removeIfPredicate);
				return;
			}

			//During iteration
			ContainerType unvisited;
			unvisited.splice(unvisited.begin(), container, currentIterator, container.end());

			container.remove_if(removeIfPredicate);//could invalidate currentIterator
			unvisited.remove_if(removeIfPredicate);

			const auto distance = unvisited.size();
			container.splice(container.end(), unvisited);

			if (container.empty())
			{
				currentIterator = container.end();
				return;
			}
			currentIterator = std::prev(container.end(), distance);
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
