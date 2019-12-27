//
// Created by gelldur on 22.12.2019.
//
#include "Character.hpp"

#include "event.hpp"

Character::Character(std::shared_ptr<EventBus> bus)
	: _bus{std::move(bus)}
{
	(void)_iq; // probably something should use it ;)
}

void Character::pickGold(int goldCount)
{
	// We could store character name as member. Sure this isn't complex example rather simple one.
	// Imagine few levels of composition ;)
	_sackGold += goldCount;
	_bus->postpone(event::GoldUpdate{_sackGold});
}

void Character::damage(int amount)
{
	_health -= amount;
}
