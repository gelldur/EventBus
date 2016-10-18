//
// Created by Dawid Drozd aka Gelldur on 18/10/16.
//

#include "EventContainer.h"

namespace Dexode
{

EventContainer::EventContainer(const std::shared_ptr<Notifier>& notifier)
		: _notifier(notifier)
{
	assert(_notifier);
}

void null_deleter(Notifier*)
{
}

//Maybe ugly but hey ;) Less code and simply i can :D
EventContainer::EventContainer(Notifier& notifier)
		: _notifier(&notifier, &null_deleter)
{
	assert(_notifier);
}

EventContainer::~EventContainer()
{
	unlistenAll();
}

void EventContainer::unlistenAll()
{
	if (_token != 0)
	{
		_notifier->unlistenAll(_token);
	}
}

}
