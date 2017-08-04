//
// Created by Dawid Drozd aka Gelldur on 18/10/16.
//

#include <eventbus/EventCollector.h>

namespace
{

void null_deleter(Dexode::Notifier*)
{
}

}

namespace Dexode
{

EventCollector::EventCollector(const std::shared_ptr<Notifier>& notifier)
		: _notifier(notifier)
{
	assert(_notifier);
}

//Maybe ugly but hey ;) Less code and simply i can :D
EventCollector::EventCollector(Notifier& notifier)
		: _notifier(&notifier, &null_deleter)
{
	assert(_notifier);
}

EventCollector::EventCollector(EventCollector const& other)
		: _notifier(other._notifier)
{
}

EventCollector::EventCollector(EventCollector&& other)
		: _token(other._token)
		, _notifier(std::move(other._notifier))
{
	other._token = 0;
}

EventCollector::~EventCollector()
{
	unlistenAll();
}

EventCollector& EventCollector::operator=(EventCollector const& other)
{
	if (this == &other)
	{
		return *this;
	}
	if (other._notifier.get() != _notifier.get())
	{
		unlistenAll();
		_notifier = other._notifier;
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
	_notifier = std::move(other._notifier);

	return *this;
}

void EventCollector::unlistenAll()
{
	if (_token != 0)
	{
		_notifier->unlistenAll(_token);
	}
}

}
