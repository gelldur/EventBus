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
		vectorImpl->add(token, callback);
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
		const auto typeId = getTypeId<Event>();//TODO think about constexpr
		auto found = _callbacks.find(typeId);
		if (found == _callbacks.end())
		{
			return;// no such notifications
		}

		std::unique_ptr<VectorInterface>& vector = found->second;
		assert(dynamic_cast<Vector*>(vector.get()));
		Vector* vectorImpl = static_cast<Vector*>(vector.get());

		vectorImpl->beginTransaction();
		for (const auto& element : vectorImpl->container)
		{
			element.second(event);
		}
		vectorImpl->commitTransaction();
	}

private:
	struct VectorInterface
	{
		virtual ~VectorInterface() = default;

		virtual void remove(const int token) = 0;
	};

	template<typename Event>
	struct VectorImpl : public VectorInterface
	{
		using CallbackType = std::function<void(const Event&)>;
		using ContainerElement = std::pair<int, CallbackType>;
		using ContainerType = std::vector<ContainerElement>;
		ContainerType container;
		ContainerType toAdd;
		std::vector<int> toRemove;
		int inTransaction = 0;

		virtual void remove(const int token) override
		{
			if (inTransaction > 0)
			{
				toRemove.push_back(token);
				return;
			}

			//Invalidation rules: https://stackoverflow.com/questions/6438086/iterator-invalidation-rules
			auto removeFrom = std::remove_if(container.begin(), container.end()
											 , [token](const ContainerElement& element)
					{
						return element.first == token;
					});
			if (removeFrom != container.end())
			{
				container.erase(removeFrom, container.end());
			}
		}

		void add(const int token, const CallbackType& callback)
		{
			if (inTransaction > 0)
			{
				toAdd.emplace_back(token, callback);
			}
			else
			{
				container.emplace_back(token, callback);
			}
		}

		void beginTransaction()
		{
			++inTransaction;
		}

		void commitTransaction()
		{
			--inTransaction;
			if (inTransaction > 0)
			{
				return;
			}
			inTransaction = 0;

			if (toAdd.empty() == false)
			{
				container.insert(container.end(), toAdd.begin(), toAdd.end());
				toAdd.clear();
			}
			if (toRemove.empty() == false)
			{
				for (auto token : toRemove)
				{
					remove(token);
				}
				toRemove.clear();
			}
		}
	};

	int _tokener = 0;
	std::map<std::size_t, std::unique_ptr<VectorInterface>> _callbacks;

	template<typename T>
	static std::size_t getTypeId()
	{
		//std::hash<std::string>{}(typeid(T).name() is slower
		return typeid(T).hash_code();
	}
};

} /* namespace Dexode */
