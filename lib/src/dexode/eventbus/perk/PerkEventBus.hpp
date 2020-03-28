//
// Created by gelldur on 23.12.2019.
//
#pragma once

#include <memory>
#include <vector>

#include "Perk.hpp"
#include "dexode/EventBus.hpp"

namespace dexode::eventbus::perk
{

class PerkEventBus : public EventBus
{
public:
	class RegisterHelper
	{
		friend PerkEventBus;

	public:
		template <typename Perk_t>
		RegisterHelper& registerPrePostpone(perk::Flag (Perk_t::*method)(PostponeHelper&))
		{
			_bus->_onPrePostpone.push_back(
				std::bind(method, static_cast<Perk_t*>(_perk), std::placeholders::_1));
			return *this;
		}

		template <typename Perk_t>
		RegisterHelper& registerPostPostpone(perk::Flag (Perk_t::*method)(PostponeHelper&))
		{
			_bus->_onPostPostpone.push_back(
				std::bind(method, static_cast<Perk_t*>(_perk), std::placeholders::_1));
			return *this;
		}

	private:
		PerkEventBus* _bus;
		Perk* _perk;

		RegisterHelper(PerkEventBus* bus, Perk* perk)
			: _bus(bus)
			, _perk(perk)
		{}
	};

	RegisterHelper addPerk(std::shared_ptr<Perk> perk);

	template <typename T>
	T* getPerk()
	{
		auto found =
			std::find_if(_perks.begin(), _perks.end(), [](const std::shared_ptr<Perk>& perk) {
				return dynamic_cast<T*>(perk.get()) != nullptr;
			});
		if(found != _perks.end())
		{
			return static_cast<T*>(found->get());
		}
		return nullptr;
	}

protected:
	bool postponeEvent(PostponeHelper& postponeCall) override;

private:
	std::vector<std::shared_ptr<Perk>> _perks;
	std::vector<std::function<perk::Flag(PostponeHelper&)>> _onPrePostpone;
	std::vector<std::function<perk::Flag(PostponeHelper&)>> _onPostPostpone;
};

} // namespace dexode::eventbus::perk
