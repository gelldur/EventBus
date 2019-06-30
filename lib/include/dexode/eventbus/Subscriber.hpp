#pragma once

#include "dexode/EventBus.hpp"

namespace dexode::eventbus
{
template <class Bus>
class Subscriber
{
public:
	Subscriber(std::shared_ptr<Bus> retainBus)
		: _listener{*retainBus}
		, _bus{std::move(retainBus)}
	{}

	Subscriber(Subscriber&& other)
		: _listener(std::move(other._listener))
		, _bus(std::move(other._bus))
	{}

	~Subscriber()
	{
		unlistenAll(); // extra safety if order of members is inverse
	}

	Subscriber& operator=(Subscriber&& other)
	{
		if(this == &other)
		{
			return *this;
		}

		_listener = std::move(other._listener);
		_bus = std::move(other._bus);

		return *this;
	}

	template <class Event>
	void listen(std::function<void(const Event&)>&& callback)
	{
		_listener.template listen<Event>(std::forward<std::function<void(const Event&)>>(callback));
	}

	template <class Event>
	void listen(const std::function<void(const Event&)>& callback)
	{
		_listener.template listen<Event>(callback);
	}

	void unlistenAll()
	{
		_listener.unlistenAll();
	}

	template <typename Event>
	void unlisten()
	{
		_listener.template unlisten<Event>();
	}

	const std::shared_ptr<Bus>& getEventBus() const
	{
		return _bus;
	}

private:
	Listener<Bus> _listener;
	std::shared_ptr<Bus> _bus; // own
};
} // namespace dexode::eventbus
