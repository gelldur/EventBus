//
// Created by gelldur on 22.12.2019.
//
#pragma once

namespace event
{

struct GoldUpdate
{
	int goldCount;
};

struct NewTeamMember
{
	std::string memberName;
};

template <typename T>
struct TagEvent
{
	using Event = T;
	std::string tag;
	Event data;
};

} // namespace event
