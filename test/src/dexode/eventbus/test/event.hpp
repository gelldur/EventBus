//
// Created by gelldur on 24.11.2019.
//
#pragma once

namespace dexode::eventbus::test::event
{

struct T1
{};

struct T2
{};

struct Value
{
	int value{-1};
};

template <typename Event>
struct Tag
{
	Event data;
	std::string tag;
};

struct WaitPerk
{

};

} // namespace dexode::eventbus::test::event
