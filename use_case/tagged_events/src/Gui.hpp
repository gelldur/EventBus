//
// Created by gelldur on 22.12.2019.
//
#pragma once

#include <map>
#include <string>

#include "EventBus.hpp"

class Gui
{
public:
	Gui(const std::shared_ptr<EventBus>& bus);

	void draw();

private:
	EventBus::Listener _listener;
	std::map<std::string, int> _sackOfGold;
};
