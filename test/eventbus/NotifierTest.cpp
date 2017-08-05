//
// Created by Dawid Drozd aka Gelldur on 05.08.17.
//

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <eventbus/Notifier.h>

namespace
{

MAKE_NOTIFICATION(SimpleNotification, int);

MAKE_NOTIFICATION(RefNotification, int &);

}

TEST_CASE("eventbus/Simple test", "Simple test")
{
	Dexode::Notifier bus;
	const auto token = bus.listen(getNotificationSimpleNotification(), [](int value)
	{
		REQUIRE(value == 3);
	});

	bus.notify(getNotificationSimpleNotification(), 3);
	bus.unlistenAll(token);
	bus.notify(getNotificationSimpleNotification(), 2);

	bus.listen(getNotificationSimpleNotification(), [](int value)
	{
		REQUIRE(value == 1);
	});
	bus.notify(getNotificationSimpleNotification(), 1);
}

TEST_CASE("eventbus/Multiple listen on same token", "Listening on the same token")
{
	Dexode::Notifier bus;
	const auto token = bus.listen(getNotificationRefNotification(), [](int& value)
	{
		REQUIRE(value == 3);
		--value;
	});
	bus.listen(token, getNotificationRefNotification(), [](int& value)
	{
		REQUIRE(value == 2);
	});

	int value = 3;
	bus.notify(getNotificationRefNotification(), value);

	bus.unlistenAll(token);
	bus.notify(getNotificationSimpleNotification(), 2);
}
