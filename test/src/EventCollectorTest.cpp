//
// Created by Dawid Drozd aka Gelldur on 05.08.17.
//

#include <catch2/catch.hpp>
#include <dexode/EventBus.hpp>
#include <dexode/eventbus/strategy/Transaction.hpp>

using namespace dexode;
using TransactionEventBus = EventBus<eventbus::strategy::Transaction>;
using Listener = TransactionEventBus::Listener;

TEST_CASE("eventbus/EventCollector sample", "Simple test for EventCollector")
{
	struct SimpleEvent
	{
		int value;
	};

	TransactionEventBus bus;

	int callCount = 0;
	{
		auto listener = Listener::createNotOwning(bus);
		listener.listen<SimpleEvent>([&](const SimpleEvent& event) {
			REQUIRE(event.value == 3);
			++callCount;
		});
		bus.post(SimpleEvent{3});
		REQUIRE(callCount == 1);
	}
	bus.post(SimpleEvent{2});
	REQUIRE(callCount == 1);
}

TEST_CASE("eventbus/EventCollector unlistenAll", "EventCollector::unlistenAll")
{
	struct SimpleEvent
	{
		int value;
	};
	TransactionEventBus bus;
	auto listener = Listener::createNotOwning(bus);

	int callCount = 0;
	listener.listen<SimpleEvent>([&](const SimpleEvent& event) {
		REQUIRE(event.value == 3);
		++callCount;
	});
	bus.post(SimpleEvent{3});
	listener.unlistenAll();

	bus.post(SimpleEvent{2});
	REQUIRE(callCount == 1);
}

TEST_CASE("eventbus/EventCollector reset", "EventCollector reset when we reasign")
{
	struct SimpleEvent
	{
		int value;
	};

	TransactionEventBus bus;
	int callCount = 0;
	auto listener = Listener::createNotOwning(bus);
	listener.listen<SimpleEvent>([&](const SimpleEvent& event) {
		REQUIRE(event.value == 3);
		++callCount;
	});
	bus.post(SimpleEvent{3});
	REQUIRE(callCount == 1);
	listener = Listener{};

	bus.post(SimpleEvent{2});
	REQUIRE(callCount == 1);
}
