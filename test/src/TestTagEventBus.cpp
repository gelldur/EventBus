//
// Created by gelldur on 30.10.2019.
//
#include <string>

#include <catch2/catch.hpp>

#include "dexode/eventbus/TagEventBus.hpp"
#include "dexode/eventbus/strategy/Protected.hpp"

using TagEventBus = dexode::eventbus::TagEventBus<dexode::eventbus::strategy::Protected>;

namespace
{

struct EventWithMessage
{
	std::string message = "no-msg";
};

} // namespace

TEST_CASE("Should notify only listeners with specific tag When using TagEventBus", "[TagEventBus]")
{
	TagEventBus bus{{"gui", "backend"}};
	auto listenerGlobal = TagEventBus::ListenerAll::createNotOwning(bus);

	int counterGlobalListener = 0;
	int counterTagGUIListener = 0;
	int counterTagBackendListener = 0;

	listenerGlobal.listen<EventWithMessage>([&](const EventWithMessage& event) {
		INFO("[Global listener] Received: EventWithMessage:" << event.message);
		++counterGlobalListener;
	});

	TagEventBus::Listener guiListener{bus.get("gui")};
	guiListener.listen<EventWithMessage>([&](const EventWithMessage& event) {
		INFO("[GUI listener] Received: EventWithMessage:" << event.message);
		++counterTagGUIListener;
	});

	TagEventBus::Listener backendListener{bus.get("backend")};
	backendListener.listen<EventWithMessage>([&](const EventWithMessage& event) {
		INFO("[Backend listener] Received: EventWithMessage:" << event.message);
		++counterTagBackendListener;
	});

	{
		bus.post(EventWithMessage{"everyone should get this message (global listeners included)"});
		REQUIRE(counterGlobalListener == 1);
		REQUIRE(counterTagGUIListener == 1);
		REQUIRE(counterTagBackendListener == 1);
	}
	{
		bus.post("gui", EventWithMessage{"gui + global should get this message"});
		REQUIRE(counterGlobalListener == 2);
		REQUIRE(counterTagGUIListener == 2);
		REQUIRE(counterTagBackendListener == 1);
	}
	{
		bus.post("backend", EventWithMessage{"backend + global should get this message"});
		REQUIRE(counterGlobalListener == 3);
		REQUIRE(counterTagGUIListener == 2);
		REQUIRE(counterTagBackendListener == 2);
	}
}
