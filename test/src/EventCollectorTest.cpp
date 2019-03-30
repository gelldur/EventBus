//
// Created by Dawid Drozd aka Gelldur on 05.08.17.
//

#include <catch2/catch.hpp>

#include <eventbus/EventCollector.h>

TEST_CASE("eventbus/EventCollector sample", "Simple test for EventCollector")
{
	struct SimpleEvent
	{
		int value;
	};

	Dexode::EventBus bus;
	int callCount = 0;
	{
		Dexode::EventCollector listener {&bus};
		listener.listen<SimpleEvent>([&](const SimpleEvent& event) {
			REQUIRE(event.value == 3);
			++callCount;
		});
		bus.notify(SimpleEvent {3});
		REQUIRE(callCount == 1);
	}
	bus.notify(SimpleEvent {2});
	REQUIRE(callCount == 1);
}

TEST_CASE("eventbus/EventCollector unlistenAll", "EventCollector::unlistenAll")
{
	struct SimpleEvent
	{
		int value;
	};
	Dexode::EventBus bus;
	Dexode::EventCollector listener {&bus};

	int callCount = 0;
	listener.listen<SimpleEvent>([&](const SimpleEvent& event) {
		REQUIRE(event.value == 3);
		++callCount;
	});
	bus.notify(SimpleEvent {3});
	listener.unlistenAll();

	bus.notify(SimpleEvent {2});
	REQUIRE(callCount == 1);
}

TEST_CASE("eventbus/EventCollector reset", "EventCollector reset when we reasign")
{
	struct SimpleEvent
	{
		int value;
	};

	Dexode::EventBus bus;
	int callCount = 0;
	Dexode::EventCollector listener {&bus};
	listener.listen<SimpleEvent>([&](const SimpleEvent& event) {
		REQUIRE(event.value == 3);
		++callCount;
	});
	bus.notify(SimpleEvent {3});
	REQUIRE(callCount == 1);
	listener = {nullptr};

	bus.notify(SimpleEvent {2});
	REQUIRE(callCount == 1);
}
