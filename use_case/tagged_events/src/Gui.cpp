//
// Created by gelldur on 22.12.2019.
//
#include "Gui.hpp"

#include <iostream>

#include "event.hpp"

Gui::Gui(const std::shared_ptr<EventBus>& bus)
	: _listener{bus}
{
	_listener.listen(
		[this](const event::NewTeamMember& event) { _sackOfGold.emplace(event.memberName, 0); });

	_listener.listen([this](const event::TagEvent<event::GoldUpdate>& event) {
		auto found = _sackOfGold.find(event.tag);
		if(found != _sackOfGold.end())
		{
			found->second = event.data.goldCount;
		}
	});
}

void Gui::draw()
{
	std::cout << "-----------------------------\n";
	for(const auto& player : _sackOfGold)
	{
		std::cout << "Name:" << player.first << " - gold: " << player.second << "\n";
	}
	std::cout << "-----------------------------" << std::endl;
}
