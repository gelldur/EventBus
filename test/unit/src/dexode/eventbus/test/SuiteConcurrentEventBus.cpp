#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include <catch2/catch.hpp>

#include "dexode/EventBus.hpp"
#include "dexode/eventbus/perk/PerkEventBus.hpp"
#include "dexode/eventbus/perk/WaitPerk.hpp"
#include "dexode/eventbus/test/event.hpp"

using namespace std::chrono_literals;

namespace dexode::eventbus::test
{
using Listener = dexode::EventBus::Listener;

struct SimpleEvent
{
	std::thread::id id;
};

constexpr auto ns2 = std::chrono::nanoseconds{2}; // GCC 7 has some issues with 2ms :/
constexpr auto ns3 = std::chrono::nanoseconds{3};

TEST_CASE("Should consume events in synchronous way When using worker threads",
		  "[concurrent][EventBus]")
{
	EventBus bus;
	auto listener = Listener::createNotOwning(bus);

	std::atomic<int> counter = 0;

	listener.listen([&counter](const SimpleEvent& event) {
		// std::cout << "Event from: " << event.id << std::endl;
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
	int sumOfConsumed = 0;
	while(counter < 20)
	{
		REQUIRE(counter == sumOfConsumed);
		sumOfConsumed += bus.process();
		REQUIRE(counter == sumOfConsumed);
	}

	worker1.join();
	worker2.join();
}

TEST_CASE("Should listen for only 1 event When call unlisten inside Listener",
		  "[concurrent][EventBus]")
{
	EventBus bus;
	auto listener = Listener::createNotOwning(bus);

	std::atomic<int> counter = 0;

	listener.listen<SimpleEvent>([&counter, &listener](const SimpleEvent& event) {
		// std::cout << "Event from: " << event.id << std::endl;
		++counter;
		listener.unlistenAll();
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

	std::thread worker3{[&bus, &counter]() {
		while(counter == 0)
		{
			bus.process();
			std::this_thread::sleep_for(ns2);
		}
		for(int i = 0; i < 10; ++i)
		{
			bus.process();
			std::this_thread::sleep_for(ns2);
		}
	}};

	for(int i = 0; i < 10; ++i)
	{
		bus.postpone(SimpleEvent{std::this_thread::get_id()});
	}

	worker1.join();
	worker2.join();
	worker3.join();

	REQUIRE(counter == 1); // Should be called only once
}

TEST_CASE("Should wait work", "[concurrent][EventBus]")
{
	dexode::eventbus::perk::PerkEventBus bus;
	bus.addPerk(std::make_unique<dexode::eventbus::perk::WaitPerk>())
		.registerPostPostpone(&dexode::eventbus::perk::WaitPerk::onPostponeEvent);

	auto* waitPerk = bus.getPerk<dexode::eventbus::perk::WaitPerk>();
	REQUIRE(waitPerk != nullptr);

	auto listener = Listener::createNotOwning(bus);
	listener.listen(
		[](const event::WaitPerk& event) { std::cout << "In WaitPerk event" << std::endl; });
	listener.listen([](const event::T1& event) { std::cout << "In T1 event" << std::endl; });

	// Worker which will send event every 10 ms
	std::atomic<bool> isWorking = true;
	std::atomic<int> produced{0};
	std::atomic<int> consumed{0};

	std::thread producer{[&bus, &isWorking, &produced]() {
		while(isWorking)
		{
			bus.postpone(event::WaitPerk{});
			bus.postpone(event::T1{});
			++produced;
			++produced;
			std::this_thread::sleep_for(5ms);
		}
	}};

	for(int i = 0; i < 20; ++i)
	{
		auto start = std::chrono::steady_clock::now();
		if(waitPerk->waitFor(20ms))
		{
			int beforeConsumed = consumed;
			consumed += bus.process();
			INFO("If events available then consumed count should change")
			CHECK(consumed >= beforeConsumed);
		}
		else
		{
			// No events waiting for us
		}

		std::cout << "I was sleeping for: "
				  << std::chrono::duration_cast<std::chrono::milliseconds>(
						 std::chrono::steady_clock::now() - start)
						 .count()
				  << " ms, consumed:" << consumed << std::endl;
	}

	isWorking = false;
	producer.join();
	REQUIRE(produced >= consumed);
}

} // namespace dexode::eventbus::test
