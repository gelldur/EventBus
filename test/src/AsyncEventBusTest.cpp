#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include <catch2/catch.hpp>
#include <dexode/EventBus.hpp>
#include <dexode/eventbus/strategy/Protected.hpp>

using namespace std::chrono;

const auto ns2 = std::chrono::nanoseconds{2}; // GCC 7 has some issues with 2ms :/
const auto ns3 = std::chrono::nanoseconds{3};

using namespace dexode;
using ProtectedEventBus = EventBus<eventbus::strategy::Protected>;
using Listener = ProtectedEventBus::Listener;

TEST_CASE("Should consume events in synchronous way When using AsyncEventBus", "[AsyncEventBus]")
{
	struct SimpleEvent
	{
		std::thread::id id;
	};

	ProtectedEventBus bus;
	auto listener = Listener::createNotOwning(bus);

	int counter = 0;

	listener.listen<SimpleEvent>([&counter](const SimpleEvent& event) {
		std::cout << "Event from: " << event.id << std::endl;
		++counter;
	});

	std::thread worker1{[&bus]() {
		for(int i = 0; i < 10; ++i)
		{
			bus.postpone(SimpleEvent{std::this_thread::get_id()});
			std::this_thread::sleep_for(ns3);
		}
	}};
	std::thread worker2{[&bus]() {
		for(int i = 0; i < 10; ++i)
		{
			bus.postpone(SimpleEvent{std::this_thread::get_id()});
			std::this_thread::sleep_for(ns2);
		}
	}};

	REQUIRE(counter == 0);
	while(counter < 20)
	{
		static int sumOfConsumed = 0;
		REQUIRE(counter == sumOfConsumed);
		sumOfConsumed += bus.processAll();
		REQUIRE(counter == sumOfConsumed);
	}

	worker1.join();
	worker2.join();
}

TEST_CASE("Should unlisten for event When call unlisten inside Listener", "[AsyncEventBus]")
{
	struct SimpleEvent
	{
		std::thread::id id;
	};

	ProtectedEventBus bus;
	auto listener = Listener::createNotOwning(bus);

	int counter = 0;

	listener.listen<SimpleEvent>([&counter](const SimpleEvent& event) {
		std::cout << "Event from: " << event.id << std::endl;
		++counter;
		// TODO FIXME
		// bus.unlistenAll(myToken); // This doesn't mean that unlisten will be ASAP!
	});

	REQUIRE(counter == 0);
	bus.postpone(SimpleEvent{std::this_thread::get_id()});
	// This should consume (listen request), SimpleEvent, ()unlisten request
	REQUIRE(bus.processLimit(1) == 1);

	for(int i = 0; i < 10; ++i)
	{
		bus.processAll();
		bus.postpone(SimpleEvent{std::this_thread::get_id()});
		bus.processAll();
	}

	REQUIRE(counter == 1); // Should be called only once
}

TEST_CASE("Should listen for only 1 event When call unlisten inside Listener", "[AsyncEventBus]")
{
	struct SimpleEvent
	{
		std::thread::id id;
	};

	ProtectedEventBus bus;
	auto listener = Listener::createNotOwning(bus);

	int counter = 0;

	listener.listen<SimpleEvent>([&counter](const SimpleEvent& event) {
		std::cout << "Event from: " << event.id << std::endl;
		++counter;
		// TODO FIXME
		// bus.unlistenAll(myToken); // This doesn't mean that unlisten will be ASAP!
	});

	// Workers in this test are only extra stuff
	std::thread worker1{[&bus]() {
		for(int i = 0; i < 10; ++i)
		{
			bus.postpone(SimpleEvent{std::this_thread::get_id()});
			std::this_thread::sleep_for(ns3);
		}
	}};
	// Workers in this test are only extra stuff
	std::thread worker2{[&bus]() {
		for(int i = 0; i < 10; ++i)
		{
			bus.postpone(SimpleEvent{std::this_thread::get_id()});
			std::this_thread::sleep_for(ns2);
		}
	}};

	REQUIRE(counter == 0);

	for(int i = 0; i < 10; ++i)
	{
		bus.postpone(SimpleEvent{std::this_thread::get_id()});
		bus.postpone(SimpleEvent{std::this_thread::get_id()});
		bus.processAll();
	}

	REQUIRE(counter == 1); // Should be called only once
	worker1.join();
	worker2.join();
}
