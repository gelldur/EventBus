//
// Created by Dawid Drozd aka Gelldur on 05.08.17.
//

#include <catch.hpp>

#include <eventbus/EventCollector.h>

namespace
{

MAKE_NOTIFICATION(SimpleNotification, int);

MAKE_NOTIFICATION(RefNotification, int &);

}

TEST_CASE("eventbus/EventCollector sample", "Simple test for EventCollector")
{
	Dexode::Notifier bus;
	int callCount = 0;
	{
		Dexode::EventCollector listener{bus};
		listener.listen(getNotificationSimpleNotification(), [&](int value)
		{
			REQUIRE(value == 3);
			++callCount;
		});
		bus.notify(getNotificationSimpleNotification(), 3);
		REQUIRE(callCount == 1);
	}
	bus.notify(getNotificationSimpleNotification(), 2);
	REQUIRE(callCount == 1);
}

TEST_CASE("eventbus/EventCollector unlistenAll", "EventCollector::unlistenAll")
{
	Dexode::Notifier bus;
	Dexode::EventCollector listener{bus};

	int callCount = 0;
	listener.listen(getNotificationSimpleNotification(), [&](int value)
	{
		REQUIRE(value == 3);
		++callCount;
	});
	bus.notify(getNotificationSimpleNotification(), 3);
	listener.unlistenAll();

	bus.notify(getNotificationSimpleNotification(), 2);
	REQUIRE(callCount == 1);
}

TEST_CASE("eventbus/EventCollector reset", "EventCollector reset when we reasign")
{
	Dexode::Notifier bus;
	int callCount = 0;
	Dexode::EventCollector listener{bus};
	listener.listen(getNotificationSimpleNotification(), [&](int value)
	{
		REQUIRE(value == 3);
		++callCount;
	});
	bus.notify(getNotificationSimpleNotification(), 3);
	REQUIRE(callCount == 1);
	listener = {};

	bus.notify(getNotificationSimpleNotification(), 2);
	REQUIRE(callCount == 1);
}
