//
// Created by gelldur on 23.12.2019.
//
#include "PerkEventBus.hpp"

namespace dexode::eventbus::perk
{

PerkEventBus::RegisterHelper PerkEventBus::addPerk(std::shared_ptr<Perk> perk)
{
	auto* local = perk.get();
	_perks.push_back(std::move(perk));
	return RegisterHelper(this, local);
}

bool PerkEventBus::postponeEvent(PostponeHelper& postponeCall)
{
	for(const auto& onPrePostpone : _onPrePostpone)
	{
		if(onPrePostpone(postponeCall) == perk::Flag::postpone_cancel)
		{
			return false;
		}
	}
	if(EventBus::postponeEvent(postponeCall))
	{
		for(const auto& onPostPostpone : _onPostPostpone)
		{
			if(onPostPostpone(postponeCall) == perk::Flag::postpone_cancel)
			{
				break;
			}
		}
		return true;
	}
	return false;
}

} // namespace dexode::eventbus::perk
