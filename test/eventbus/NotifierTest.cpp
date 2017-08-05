//
// Created by Dawid Drozd aka Gelldur on 05.08.17.
//

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <eventbus/EventBus.h>

TEST_CASE("eventbus/Simple test", "Simple test")
{
	Dexode::EventBus bus;
	Dexode::Notification2<int> simpleNotification{"simple"};

	const auto token = bus.listen(simpleNotification, [](int value)
	{
		REQUIRE(value == 3);
	});

	bus.notify(simpleNotification, 3);
	bus.unlistenAll(token);
	bus.notify(simpleNotification, 2);

	bus.listen(simpleNotification, [](int value)
	{
		REQUIRE(value == 1);
	});
	bus.notify(simpleNotification, 1);
}

TEST_CASE("eventbus/Multiple listen on same token", "Listening on the same token")
{
	Dexode::EventBus bus;
	Dexode::Notification2<int> simpleNotification{"simple"};
	Dexode::Notification2<int&> simpleRefNotification{"simple"};
	const auto token = bus.listen(simpleRefNotification, [](int& value)
	{
		REQUIRE(value == 3);
		--value;
	});
	bus.listen(token, simpleRefNotification, [](int& value)
	{
		REQUIRE(value == 2);
	});

	int value = 3;
	bus.notify(simpleRefNotification, value);

	bus.unlistenAll(token);
	bus.notify(simpleNotification, value);
	REQUIRE(value == 2);
	bus.notify(simpleRefNotification, value);
	REQUIRE(value == 2);
}

TEST_CASE("eventbus/EventBus listen", "Listen without notification object. Using only string")
{
	int isCalled = 0;
	Dexode::EventBus bus;
	const auto token = bus.listen<int>("simple", [&](int value)
	{
		++isCalled;
		REQUIRE(value == 3);
	});

	Dexode::Notification2<int> simpleNotification{"simple"};
	REQUIRE(isCalled == 0);
	bus.notify(simpleNotification, 3);
	REQUIRE(isCalled == 1);
	bus.unlistenAll(token);
	bus.notify(simpleNotification, 2);

	bus.listen(simpleNotification, [&](int value)
	{
		++isCalled;
		REQUIRE(value == 1);
	});
	REQUIRE(isCalled == 1);
	bus.notify(simpleNotification, 1);
	REQUIRE(isCalled == 2);
}

TEST_CASE("eventbus/EventBus listen & notify", "Listen & notify without notification object. Using only string")
{
	int isCalled = 0;
	Dexode::EventBus bus;
	const auto token = bus.listen<int>("simple", [&](int value)
	{
		++isCalled;
		REQUIRE(value == 3);
	});
	REQUIRE(isCalled == 0);
	bus.notify<int>(std::string{"simple"}, 3);
	REQUIRE(isCalled == 1);
	bus.unlistenAll(token);
	bus.notify<int>("simple", 2);
	REQUIRE(isCalled == 1);

	bus.listen<int>("simple", [&](int value)
	{
		++isCalled;
		REQUIRE(value == 1);
	});
	bus.notify<int>("simple", 1);
	REQUIRE(isCalled == 2);
}
