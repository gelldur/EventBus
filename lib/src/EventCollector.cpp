//
// Created by Dawid Drozd aka Gelldur on 18/10/16.
//

#include <eventbus/EventCollector.h>

#include <cassert>

namespace
{

void null_deleter(Dexode::EventBus*)
{
}

}

namespace Dexode
{

EventCollector::EventCollector(const std::shared_ptr<EventBus>& bus)
		: _bus(bus)
{
	assert(_bus);
}

//Maybe ugly but hey ;) Less code and simply i can :D
EventCollector::EventCollector(EventBus* bus)
		: _bus(bus, &null_deleter)
{
}

EventCollector::EventCollector(const EventCollector& other)
		: _bus(other._bus)
{
}

EventCollector::EventCollector(EventCollector&& other)
		: _token(other._token)
		, _bus(std::move(other._bus))
{
	other._token = 0;
}

EventCollector::~EventCollector()
{
	unlistenAll();
}

EventCollector& EventCollector::operator=(const EventCollector& other)
{
	std::unique_ptr<int> vector3;
	if (this == &other)
	{
		return *this;
	}
	if (other._bus.get() != _bus.get())
	{
		unlistenAll();
		_bus = other._bus;
	}

	return *this;
}

EventCollector& EventCollector::operator=(EventCollector&& other)
{
	if (this == &other)
	{
		return *this;
	}

	unlistenAll();

	_token = other._token;
	other._token = 0;
	_bus = std::move(other._bus);

	return *this;
}

void EventCollector::unlistenAll()
{
	if (_token != 0 && _bus)
	{
		_bus->unlistenAll(_token);
	}
}

BusAttorney EventCollector::getBus() const
{
	return BusAttorney{_bus};
}

bool EventCollector::isUsing(const std::shared_ptr<EventBus>& bus) const
{
	return _bus == bus;
}

}
