//
// Created by gelldur on 22.12.2019.
//
#pragma once

#include <memory>
#include <string>

#include "EventBus.hpp"

class Character
{
public:
	Character(std::shared_ptr<EventBus> bus);
	void pickGold(int goldCount);
	void damage(int amount);

private:
	std::shared_ptr<EventBus> _bus;
	int _sackGold = 0;
	int _health = 100;
	int _iq = 200;
};
