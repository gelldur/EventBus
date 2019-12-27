//
// Created by gelldur on 23.12.2019.
//
#pragma once

namespace dexode::eventbus::perk
{

enum class Flag : int
{
	nop,
	postpone_cancel,
	postpone_continue,
};

class Perk
{
public:
	virtual ~Perk() = default;
};

} // namespace dexode::eventbus::perk
