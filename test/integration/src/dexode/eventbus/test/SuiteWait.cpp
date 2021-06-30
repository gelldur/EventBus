//
// Created by gelldur on 12.03.2020.
//
#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include <catch2/catch.hpp>

#include "dexode/EventBus.hpp"
#include "dexode/eventbus/perk/PerkEventBus.hpp"
#include "dexode/eventbus/perk/WaitPerk.hpp"

using namespace std::chrono_literals;

namespace
{
struct EventTest
{
	std::string data;
	std::chrono::steady_clock::time_point created = std::chrono::steady_clock::now();
};
} // namespace

TEST_CASE("Should not be processed with unnecessary delay", "[concurrent][EventBus]")
{
	auto bus = std::make_shared<dexode::eventbus::perk::PerkEventBus>();
	bus->addPerk(std::make_unique<dexode::eventbus::perk::WaitPerk>())
		.registerPostPostpone(&dexode::eventbus::perk::WaitPerk::onPostponeEvent);

	auto* waitPerk = bus->getPerk<dexode::eventbus::perk::WaitPerk>();
	REQUIRE(waitPerk != nullptr);

	dexode::eventbus::perk::PerkEventBus::Listener listener{bus};
	listener.listen([bus](const EventTest& event) {
		const auto eventAge = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now() - event.created);
		CHECK(eventAge.count() < (5ms).count());
		std::cout << "Event:" << event.data << " old: " << eventAge.count() << "ms" << std::endl;
		std::this_thread::sleep_for(2ms); // Some heavy work when processing event
		bus->postpone(EventTest{"other"});
		std::this_thread::sleep_for(3ms); // Some heavy work when processing event
	});

	std::atomic<bool> isWorking = true;

	std::vector<std::thread> producers;
	// Worker which will send event every 500 ms
	producers.emplace_back([&bus, &isWorking]() {
		while(isWorking)
		{
			bus->postpone(EventTest{"producer1"});
			std::this_thread::sleep_for(500ms);
		}
	});

	for(int i = 0; i < 20;)
	{
		auto start = std::chrono::steady_clock::now();
		if(waitPerk->waitFor(2000ms))
		{
			const auto sleepTime = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now() - start);

			std::cout << "[SUCCESS] I was sleeping for: " << sleepTime.count() << " ms i:" << i
					  << std::endl;
			i += bus->process();
		}
		else
		{
			const auto sleepTime = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now() - start);
			CHECK(sleepTime.count() < (5ms).count());
			// No events waiting for us
			std::cout << "I was sleeping for: " << sleepTime.count() << " ms" << std::endl;
		}
	}

	isWorking = false;
	for(auto& producer : producers)
	{
		producer.join();
	}
}

TEST_CASE("Should wait for event being scheduled", "[concurrent][EventBus]")
{
	auto bus = std::make_shared<dexode::eventbus::perk::PerkEventBus>();
	bus->addPerk(std::make_unique<dexode::eventbus::perk::WaitPerk>())
		.registerPostPostpone(&dexode::eventbus::perk::WaitPerk::onPostponeEvent);

	auto* waitPerk = bus->getPerk<dexode::eventbus::perk::WaitPerk>();
	REQUIRE(waitPerk != nullptr);

	dexode::eventbus::perk::PerkEventBus::Listener listener{bus};
	listener.listen([bus](const EventTest& event) {
		const auto eventAge = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now() - event.created);
		CHECK(eventAge.count() < (5ms).count());
		std::cout << "Event:" << event.data << " old: " << eventAge.count() << "ms" << std::endl;
	});

	std::atomic<bool> isWorking = true;

	std::vector<std::thread> producers;
	producers.emplace_back([&bus, &isWorking]() {
		while(isWorking)
		{
			std::this_thread::sleep_for(10ms);
			bus->postpone(EventTest{"producer1"});
		}
	});

	for(int i = 0; i < 20;)
	{
		auto start = std::chrono::steady_clock::now();
		if(waitPerk->waitFor(40ms))
		{
			const auto sleepTime = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now() - start);
			CHECK(sleepTime.count() >= (9ms).count());

			std::cout << "[SUCCESS] I was sleeping for: " << sleepTime.count() << " ms i:" << i
					  << std::endl;
			i += bus->process();
		}
		else
		{
			const auto sleepTime = std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now() - start);
			CHECK(sleepTime < 5ms);
			// No events waiting for us
			std::cout << "I was sleeping for: " << sleepTime.count() << " ms" << std::endl;
		}
	}

	isWorking = false;
	for(auto& producer : producers)
	{
		producer.join();
	}
}
