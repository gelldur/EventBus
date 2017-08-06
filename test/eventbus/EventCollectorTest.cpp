//
// Created by Dawid Drozd aka Gelldur on 05.08.17.
//

#include <catch.hpp>

#include <eventbus/EventCollector.h>

TEST_CASE("eventbus/EventCollector sample", "Simple test for EventCollector")
{
	Dexode::EventBus bus;
	int callCount = 0;
	{
		Dexode::EventCollector listener{&bus};
		listener.listen<int>("simple", [&](int value)
		{
			REQUIRE(value == 3);
			++callCount;
		});
		bus.notify<int>("simple", 3);
		REQUIRE(callCount == 1);
	}
	bus.notify<int>("simple", 2);
	REQUIRE(callCount == 1);
}

TEST_CASE("eventbus/EventCollector unlistenAll", "EventCollector::unlistenAll")
{
	Dexode::EventBus bus;
	Dexode::EventCollector listener{&bus};

	int callCount = 0;
	listener.listen<int>("simple", [&](int value)
	{
		REQUIRE(value == 3);
		++callCount;
	});
	bus.notify<int>("simple", 3);
	listener.unlistenAll();

	bus.notify<int>("simple", 2);
	REQUIRE(callCount == 1);
}

TEST_CASE("eventbus/EventCollector reset", "EventCollector reset when we reasign")
{
	Dexode::EventBus bus;
	int callCount = 0;
	Dexode::EventCollector listener{&bus};
	Dexode::Event<int> simple{"simple"};
	listener.listen(simple, [&](int value)
	{
		REQUIRE(value == 3);
		++callCount;
	});
	bus.notify(simple, 3);
	REQUIRE(callCount == 1);
	listener = {nullptr};

	bus.notify(simple, 2);
	REQUIRE(callCount == 1);
}
