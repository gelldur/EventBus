#pragma once

#include <cstdint>
#include <functional>
#include <memory>

#include "dexode/eventbus/internal/ListenerAttorney.hpp"
#include "dexode/eventbus/internal/event_id.hpp"
#include "dexode/eventbus/internal/listener_traits.hpp"

namespace dexode::eventbus
{

template <class Bus>
class Listener
{
public:
	explicit Listener() = default; // Dummy listener

	explicit Listener(std::shared_ptr<Bus> bus)
		: _id{internal::ListenerAttorney<Bus>::newListenerID(*bus)}
		, _bus{std::move(bus)}
	{}

	static Listener createNotOwning(Bus& bus)
	{
		// This isn't safe but nice for playing around
		return Listener{std::shared_ptr<Bus>{&bus, [](Bus*) {}}};
	}

	Listener(const Listener& other) = delete;

	Listener(Listener&& other) noexcept
		: _id(other._id) // we don't have to reset listener ID as _bus is moved and we won't call
						 // unlistenAll
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
		// we don't have reset listener ID as bus is moved and we won't call unlistenAll
		_id = other._id;
		_bus = std::move(other._bus);

		return *this;
	}

	template <class Event, typename _ = void>
	constexpr void listen(std::function<void(const Event&)>&& callback)
	{
		static_assert(internal::validateEvent<Event>(), "Invalid event");
		listenToCallback<Event>(std::forward<std::function<void(const Event&)>>(callback));
	}

	template <class EventCallback, typename Event = internal::first_argument<EventCallback>>
	constexpr void listen(EventCallback&& callback)
	{
		static_assert(std::is_const_v<std::remove_reference_t<Event>>, "Event should be const");
		static_assert(std::is_reference_v<Event>, "Event should be const & (reference)");
		using PureEvent = std::remove_const_t<std::remove_reference_t<Event>>;
		static_assert(internal::validateEvent<PureEvent>(), "Invalid event");

		listenToCallback<PureEvent>(std::forward<EventCallback>(callback));
	}

	template <class Event>
	void listenToCallback(std::function<void(const Event&)>&& callback)
	{
		static_assert(internal::validateEvent<Event>(), "Invalid event");
		if(_bus == nullptr)
		{
			throw std::runtime_error{"bus is null"};
		}

		internal::ListenerAttorney<Bus>::template listen<Event>(
			*_bus, _id, std::forward<std::function<void(const Event&)>>(callback));
	}

	template <class Event>
	void listenToCallback(const std::function<void(const Event&)>& callback)
	{
		static_assert(internal::validateEvent<Event>(), "Invalid event");
		if(_bus == nullptr)
		{
			throw std::runtime_error{"bus is null"};
		}
		internal::ListenerAttorney<Bus>::template listen<Event>(
			*_bus, _id, std::function<void(const Event&)>{callback});
	}

	void unlistenAll()
	{
		if(_bus == nullptr)
		{
			throw std::runtime_error{"bus is null"};
		}
		internal::ListenerAttorney<Bus>::unlistenAll(*_bus, _id);
	}

	template <typename Event>
	void unlisten()
	{
		static_assert(internal::validateEvent<Event>(), "Invalid event");
		if(_bus == nullptr)
		{
			throw std::runtime_error{"bus is null"};
		}
		internal::ListenerAttorney<Bus>::unlisten(*_bus, _id, internal::event_id<Event>());
	}

private:
	std::uint32_t _id = 0;
	std::shared_ptr<Bus> _bus = nullptr;
};

} // namespace dexode::eventbus
