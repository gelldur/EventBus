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
	explicit Listener() = default; // Dummy listener

	explicit Listener(std::shared_ptr<Bus> bus)
		: _id{bus->newListenerID()}
		, _bus{std::move(bus)}
	{}

	static Listener createNotOwning(Bus& bus)
	{
		// This isn't safe but nice for playing around
		return Listener{std::shared_ptr<Bus>{&bus, [](Bus*) {}}};
	}

	Listener(const Listener& other) = delete;

	Listener(Listener&& other)
		: _id(other._id)
		, _bus(std::move(other._bus))
	{}

	~Listener()
	{
		if(_bus != nullptr) // could be moved
		{
			unlistenAll();
		}
	}

	Listener& operator=(const Listener& other) = delete;

	Listener& operator=(Listener&& other) noexcept
	{
		if(this == &other)
		{
			return *this;
		}

		if(_bus != nullptr)
		{
			unlistenAll(); // remove previous
		}
		_id = other._id;
		_bus = std::move(other._bus);

		return *this;
	}

	template <class Event>
	void listen(std::function<void(const Event&)>&& callback)
	{
		if(_bus == nullptr)
		{
			throw std::runtime_error{"bus is null"};
		}
		_bus->template listen<Event>(_id,
									 std::forward<std::function<void(const Event&)>>(callback));
	}

	void unlistenAll()
	{
		if(_bus == nullptr)
		{
			throw std::runtime_error{"bus is null"};
		}
		_bus->unlistenAll(_id);
	}

	template <typename Event>
	void unlisten()
	{
		if(_bus == nullptr)
		{
			throw std::runtime_error{"bus is null"};
		}
		_bus->template unlisten<Event>(_id);
	}

private:
	std::uint32_t _id = 0;
	std::shared_ptr<Bus> _bus = nullptr;
};

} // namespace dexode::eventbus
