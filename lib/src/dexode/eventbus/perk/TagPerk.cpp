//
// Created by gelldur on 24.12.2019.
//
#include "TagPerk.hpp"

namespace dexode::eventbus::perk
{

Flag TagPerk::onPrePostponeEvent(PostponeHelper& postponeCall)
{
	if(auto found = _eventsToWrap.find(postponeCall.eventID); found != _eventsToWrap.end())
	{
		found->second(postponeCall.event);
		return Flag::postpone_cancel;
	}

	return Flag::postpone_continue;
}

}
