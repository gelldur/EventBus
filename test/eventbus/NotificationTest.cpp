//
// Created by Dawid Drozd aka Gelldur on 05.08.17.
//

#include <catch.hpp>
#include <eventbus/Event.h>

TEST_CASE("eventbus/Event same", "Notifications should be same")
{
	{
		Dexode::Event<int> one("one");
		Dexode::Event<int> two("one");
		REQUIRE(one == two);
	}
	{
		Dexode::Event<int> one("one");
		Dexode::Event<signed> two("one");
		REQUIRE(one == two);//int == signed
	}
}

TEST_CASE("eventbus/Event not same", "Notifications should different")
{
	{
		Dexode::Event<int> one("one");
		Dexode::Event<int> two("two");
		REQUIRE(one != two);
	}
	{
		Dexode::Event<int> one("one");
		Dexode::Event<int&> two("two");
		//REQUIRE(one != two); //Different types!
	}
	{
		Dexode::Event<unsigned> one("one");
		Dexode::Event<signed> two("one");
		//REQUIRE(one != two); //Different types!
	}
	{
		Dexode::Event<int, int> one("one");
		Dexode::Event<int, unsigned> two("one");
		//REQUIRE(one != two); //Different types!
	}
}

TEST_CASE("eventbus/Event should copy constructable", "Notifications should be copy constructable")
{
	Dexode::Event<int> one("one");
	Dexode::Event<int> two = one;
	REQUIRE(one == two);
}
