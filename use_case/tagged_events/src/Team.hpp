//
// Created by gelldur on 22.12.2019.
//
#pragma once

#include <string>
#include <vector>

#include "Character.hpp"

class Team
{
public:
	Team(std::shared_ptr<EventBus> bus);
	void addPlayer(const std::string& name);

	Character& getMember(const std::string& name);

private:
	std::vector<std::string> _names;
	std::vector<Character> _squad;

	std::shared_ptr<EventBus> _bus;
};
