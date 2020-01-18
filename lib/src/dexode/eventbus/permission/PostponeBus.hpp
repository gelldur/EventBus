//
// Created by gelldur on 24.12.2019.
//
#pragma once

#include <cassert>
#include <memory>

#include "dexode/eventbus/Bus.hpp"

namespace dexode::eventbus::permission
{
/**
 * Intention of this helper is to hide API of other bus but allow only to postpone events
 * So no:
 * - listening
 * - processing
 */
class PostponeBus
{
public:
	PostponeBus(std::shared_ptr<Bus> hideBus)
		: _hideBus{std::move(hideBus)}
	{}

	template <typename Event>
	constexpr bool postpone(Event event)
	{
		assert(_hideBus != nullptr);
		return _hideBus->postpone(event);
	}

private:
	std::shared_ptr<Bus> _hideBus;
};

} // namespace dexode::eventbus::permission
