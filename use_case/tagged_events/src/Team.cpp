//
// Created by gelldur on 22.12.2019.
//
#include "Team.hpp"

#include <dexode/eventbus/perk/PassPerk.hpp>
#include <dexode/eventbus/perk/PerkEventBus.hpp>
#include <dexode/eventbus/perk/TagPerk.hpp>

#include "event.hpp"

Team::Team(std::shared_ptr<EventBus> bus)
	: _bus(std::move(bus))
{}

Character& Team::getMember(const std::string& name)
{
	auto found = std::find(_names.begin(), _names.end(), name);
	if(found == _names.end())
	{
		throw std::out_of_range("No such team member: " + name);
	}
	return _squad.at(std::distance(_names.begin(), found));
}

void Team::addPlayer(const std::string& name)
{
	auto characterBus = std::make_shared<dexode::eventbus::perk::PerkEventBus>();
	{
		auto tagPerk = std::make_unique<dexode::eventbus::perk::TagPerk>(name, _bus.get());
		tagPerk->wrapTag<event::TagEvent<event::GoldUpdate>>();

		characterBus->addPerk(std::move(tagPerk))
			.registerPrePostpone(&dexode::eventbus::perk::TagPerk::onPrePostponeEvent);
	}
	characterBus->addPerk(std::make_unique<dexode::eventbus::perk::PassEverythingPerk>(_bus))
		.registerPrePostpone(&dexode::eventbus::perk::PassEverythingPerk::onPrePostponeEvent);

	_squad.emplace_back(characterBus);
	_names.push_back(name);

	_bus->postpone(event::NewTeamMember{name});
}
