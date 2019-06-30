#pragma once

#include <cstdint>
#include <functional>
#include <memory>

namespace dexode::eventbus
{

template <class Bus>
class Listener
{
public:
	Listener(Bus& bus)
		: _id{bus.newListenerID()}
		, _bus{&bus}
	{}

	Listener(const Listener& other) = delete;

	Listener(Listener&& other)
		: _id(other._id)
		, _bus(other._bus)
	{
		other._id = 0;
		other._bus = nullptr;
	}

	~Listener()
	{
		if(_bus != nullptr) // could be moved
		{
			unlistenAll();
		}
	}

	Listener& operator=(const Listener& other) = delete;

	Listener& operator=(Listener&& other)
	{
		if(this == &other)
		{
			return *this;
		}

		unlistenAll();

		_id = other._id;
		other._id = 0;
		_bus = other._bus;
		other._bus = nullptr;

		return *this;
	}

	template <class Event>
	void listen(std::function<void(const Event&)>&& callback)
	{
		_bus->template listen<Event>(_id,
									 std::forward<std::function<void(const Event&)>>(callback));
	}

	template <class Event>
	void listen(const std::function<void(const Event&)>& callback)
	{
		_bus->template listen<Event>(_id, callback);
	}

	void unlistenAll()
	{

		_bus->unlistenAll(_id);
	}

	template <typename Event>
	void unlisten()
	{
		_bus->template unlisten<Event>(_id);
	}

private:
	std::uint32_t _id = 0;
	Bus* _bus;
};

} // namespace dexode::eventbus
