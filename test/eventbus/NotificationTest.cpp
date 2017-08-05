//
// Created by Dawid Drozd aka Gelldur on 05.08.17.
//

#include <catch.hpp>
#include <eventbus/Notification2.h>

TEST_CASE("eventbus/Notification same", "Notifications should be same")
{
	{
		Dexode::Notification2<int> one("one");
		Dexode::Notification2<int> two("one");
		REQUIRE(one == two);
	}
	{
		Dexode::Notification2<int> one("one");
		Dexode::Notification2<signed> two("one");
		REQUIRE(one == two);//int == signed
	}
}

TEST_CASE("eventbus/Notification not same", "Notifications should different")
{
	{
		Dexode::Notification2<int> one("one");
		Dexode::Notification2<int> two("two");
		REQUIRE(one != two);
	}
	{
		Dexode::Notification2<int> one("one");
		Dexode::Notification2<int&> two("two");
		//REQUIRE(one != two); //Different types!
	}
	{
		Dexode::Notification2<unsigned> one("one");
		Dexode::Notification2<signed> two("one");
		//REQUIRE(one != two); //Different types!
	}
	{
		Dexode::Notification2<int, int> one("one");
		Dexode::Notification2<int, unsigned> two("one");
		//REQUIRE(one != two); //Different types!
	}
}

TEST_CASE("eventbus/Notification should copy constructable", "Notifications should be copy constructable")
{
	Dexode::Notification2<int> one("one");
	Dexode::Notification2<int> two = one;
	REQUIRE(one == two);
}
