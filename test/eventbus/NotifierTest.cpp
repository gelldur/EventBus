//
// Created by Dawid Drozd aka Gelldur on 05.08.17.
//

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <eventbus/EventBus.h>

TEST_CASE("eventbus/Simple test", "Simple test")
{
	Dexode::EventBus bus;
	Dexode::Event<int> simpleNotification{"simple"};

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
	Dexode::Event<int> simpleNotification{"simple"};
	Dexode::Event<int&> simpleRefNotification{"simple"};
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

	Dexode::Event<int> simpleNotification{"simple"};
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

TEST_CASE("eventbus/EventBus type conversion", "Check for type conversion")
{
	class Example
	{
	public:
		Example(Dexode::EventBus& bus)
		{
			Dexode::Event<int> event{"event1"};
			_token = bus.listen(event, std::bind(&Example::onEvent1, this, std::placeholders::_1));
			// Using bind we can convert types
			bus.listen(_token, event, std::bind(&Example::onEvent2, this, std::placeholders::_1));
		}

		int calledEvent1 = 0;

		void onEvent1(int value)
		{
			++calledEvent1;
		}

		int calledEvent2 = 0;

		void onEvent2(bool value)
		{
			++calledEvent2;
		}

	private:
		int _token;
	};

	//EventCollector sample
	Dexode::EventBus bus;
	Example ex{bus};

	REQUIRE(ex.calledEvent1 == 0);
	REQUIRE(ex.calledEvent2 == 0);
	bus.notify<int>("event1", 2);
	REQUIRE(ex.calledEvent1 == 1);
	REQUIRE(ex.calledEvent2 == 1);//Lookout for std::bind!
}

TEST_CASE("eventbus/Different notification", "Valid check notification")
{
	Dexode::EventBus bus;
	Dexode::Event<int> notification{"simple1"};
	Dexode::Event<int> notification2{"simple2"};

	bool called1 = false;
	bool called2 = false;

	bus.listen(notification, [&called1](int value)
	{
		called1 = true;
		REQUIRE(value == 1);
	});

	bus.listen(notification2, [&called2](int value)
	{
		called2 = true;
		REQUIRE(value == 2);
	});

	REQUIRE(called1 == false);

	bus.notify(notification, 1);
	
	REQUIRE(called1 == true);
	REQUIRE(called2 == false);
	called1 = false;

	bus.notify(notification2, 2);

	REQUIRE(called1 == false);
	REQUIRE(called2 == true);
}
