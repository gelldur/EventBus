//
// Created by Dawid Drozd aka Gelldur on 05.08.17.
//

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <eventbus/EventBus.h>

TEST_CASE("eventbus/Simple test", "Simple test")
{
	Dexode::EventBus bus;
	struct SimpleEvent
	{
		int value;
	};

	const auto token =
		bus.listen<SimpleEvent>([](const SimpleEvent& event) { REQUIRE(event.value == 3); });

	bus.notify(SimpleEvent {3});
	bus.unlistenAll(token);
	bus.notify(SimpleEvent {2});

	bus.listen<SimpleEvent>([](const SimpleEvent& event) { REQUIRE(event.value == 1); });
	bus.notify(SimpleEvent {1});
}

TEST_CASE("eventbus/Simple test2", "Simple test")
{
	Dexode::EventBus bus;
	struct SimpleEvent
	{
		int value;
	};

	const auto token =
		bus.listen<SimpleEvent>([](const SimpleEvent& event) { REQUIRE(event.value == 3); });

	bus.notify<SimpleEvent>({3});
	bus.unlistenAll(token);
	bus.notify(SimpleEvent {2});

	bus.listen<SimpleEvent>([](const SimpleEvent& event) { REQUIRE(event.value == 1); });
	bus.notify(SimpleEvent {1});
}

TEST_CASE("eventbus/EventBus listen & notify",
		  "Listen & notify without notification object. Using only string")
{
	int isCalled = 0;
	Dexode::EventBus bus;
	struct SimpleEvent
	{
		int value;
	};

	const auto token = bus.listen<SimpleEvent>([&](const SimpleEvent& event) {
		++isCalled;
		REQUIRE(event.value == 3);
	});
	REQUIRE(isCalled == 0);
	bus.notify(SimpleEvent {3});
	REQUIRE(isCalled == 1);
	bus.unlistenAll(token);
	bus.notify(SimpleEvent {2});
	REQUIRE(isCalled == 1);

	bus.listen<SimpleEvent>([&](const SimpleEvent& event) {
		++isCalled;
		REQUIRE(event.value == 1);
	});
	bus.notify(SimpleEvent {1});
	REQUIRE(isCalled == 2);
}

TEST_CASE("eventbus/Different notification", "Valid check notification")
{
	Dexode::EventBus bus;
	struct SimpleEvent1
	{
		int value;
	};
	struct SimpleEvent2
	{
		int value;
	};

	bool called1 = false;
	bool called2 = false;

	bus.listen<SimpleEvent1>([&called1](const SimpleEvent1& event) {
		called1 = true;
		REQUIRE(event.value == 1);
	});

	bus.listen<SimpleEvent2>([&called2](const SimpleEvent2& event) {
		called2 = true;
		REQUIRE(event.value == 2);
	});

	REQUIRE(called1 == false);

	bus.notify(SimpleEvent1 {1});

	REQUIRE(called1 == true);
	REQUIRE(called2 == false);
	called1 = false;

	bus.notify(SimpleEvent2 {2});

	REQUIRE(called1 == false);
	REQUIRE(called2 == true);
}

namespace Scope1
{
struct SimpleEvent
{
	int value;
};
} // namespace Scope1
namespace Scope2
{
struct SimpleEvent
{
	int value;
};
} // namespace Scope2

TEST_CASE("eventbus/EventBus different events",
		  "Events with the same name but different scope should be different")
{
	int isCalled = 0;
	Dexode::EventBus bus;

	bus.listen<Scope1::SimpleEvent>([&](const Scope1::SimpleEvent& event) {
		++isCalled;
		REQUIRE(event.value == 1);
	});
	bus.listen<Scope2::SimpleEvent>([&](const Scope2::SimpleEvent& event) {
		++isCalled;
		REQUIRE(event.value == 2);
	});
	REQUIRE(isCalled == 0);

	bus.notify(Scope1::SimpleEvent {1});
	REQUIRE(isCalled == 1);

	bus.notify(Scope2::SimpleEvent {2});
	REQUIRE(isCalled == 2);
}

TEST_CASE("eventbus/EventBus modification during notify",
		  "Remove listener during notification should not malform EventBus")
{
	Dexode::EventBus bus;
	struct TestEvent
	{};

	int token1 = 1;
	int token2 = 2;

	int calls = 0;

	bus.listen<TestEvent>(token1, [&](const TestEvent& event) {
		++calls;
		bus.unlistenAll(token1);
		bus.unlistenAll(token2);
	});
	bus.listen<TestEvent>(token2, [&](const TestEvent& event) {
		++calls;
		bus.unlistenAll(token1);
		bus.unlistenAll(token2);
	});

	REQUIRE_NOTHROW(bus.notify(TestEvent {}));
	REQUIRE(calls == 2);

	REQUIRE_NOTHROW(bus.notify(TestEvent {}));
	REQUIRE(calls == 2);
}

TEST_CASE("eventbus/EventBus modification during notify2",
		  "Remove listener during notification should not malform EventBus")
{
	Dexode::EventBus bus;
	struct TestEvent
	{};

	int token1 = 1;
	int token2 = 2;
	int token3 = 3;

	int calls = 0;

	bus.listen<TestEvent>(token1, [&](const TestEvent& event) {
		++calls;
		bus.unlistenAll(token1);
		bus.unlistenAll(token2);
		bus.unlistenAll(token3);
	});
	bus.listen<TestEvent>(token2, [&](const TestEvent& event) {
		++calls;
		bus.unlistenAll(token1);
		bus.unlistenAll(token2);
		bus.unlistenAll(token3);
	});
	bus.listen<TestEvent>(token3, [&](const TestEvent& event) {
		++calls;
		bus.unlistenAll(token1);
		bus.unlistenAll(token2);
		bus.unlistenAll(token3);
	});

	REQUIRE_NOTHROW(bus.notify(TestEvent {}));
	REQUIRE(calls == 3);

	REQUIRE_NOTHROW(bus.notify(TestEvent {}));
	REQUIRE(calls == 3);
}

TEST_CASE("eventbus/EventBus modification during notify3",
		  "Remove listener during notification should not malform EventBus")
{
	Dexode::EventBus bus;
	struct TestEvent
	{};

	int token1 = 1;
	int token2 = 2;
	int token3 = 3;

	int calls = 0;

	bus.listen<TestEvent>(token1, [&](const TestEvent& event) {
		++calls;
		bus.unlistenAll(token1);
	});
	bus.listen<TestEvent>(token2, [&](const TestEvent& event) {
		++calls;
		bus.unlistenAll(token1);
		bus.unlistenAll(token3);
		bus.unlistenAll(token2);
	});
	bus.listen<TestEvent>(token3, [&](const TestEvent& event) {
		++calls;
		bus.unlistenAll(token1);
		bus.unlistenAll(token2);
		bus.unlistenAll(token3);
	});

	REQUIRE_NOTHROW(bus.notify(TestEvent {}));
	REQUIRE(calls == 3);
}

TEST_CASE("eventbus/EventBus modification during notify4",
		  "Remove listener during notification should not malform EventBus")
{
	Dexode::EventBus bus;
	struct TestEvent
	{};

	int token1 = 1;
	int token2 = 2;
	int token3 = 3;

	int calls = 0;

	bus.listen<TestEvent>(token1, [&](const TestEvent& event) {
		++calls;
		bus.unlistenAll(token1);

		bus.listen<TestEvent>(token2, [&](const TestEvent& event) {
			++calls;
			bus.unlistenAll(token1);
			bus.unlistenAll(token3);
			bus.unlistenAll(token2);
		});
	});
	bus.listen<TestEvent>(token3, [&](const TestEvent& event) {
		++calls;
		bus.unlistenAll(token1);
		bus.unlistenAll(token2);
		bus.unlistenAll(token3);
	});

	REQUIRE_NOTHROW(bus.notify(TestEvent {}));
	REQUIRE(calls == 2);
	REQUIRE_NOTHROW(bus.notify(TestEvent {}));
	REQUIRE(calls == 2);
}

TEST_CASE("eventbus/EventBus modification during notify5",
		  "Remove listener during notification should not malform EventBus")
{
	Dexode::EventBus bus;
	struct TestEvent
	{};

	int token1 = 1;
	int token2 = 2;
	int token3 = 3;

	int calls = 0;

	bus.listen<TestEvent>(token1, [&](const TestEvent& event) {
		++calls;
		bus.unlistenAll(token1);
	});
	bus.listen<TestEvent>(token2, [&](const TestEvent& event) {
		++calls;
		bus.unlistenAll(token1);
		bus.unlistenAll(token2);

		bus.listen<TestEvent>(token3, [&](const TestEvent& event) {
			++calls;
			bus.unlistenAll(token1);
			bus.unlistenAll(token2);
			bus.unlistenAll(token3);
		});
	});

	REQUIRE_NOTHROW(bus.notify(TestEvent {}));
	REQUIRE(calls == 2);

	REQUIRE_NOTHROW(bus.notify(TestEvent {}));
	REQUIRE(calls == 3);
}

TEST_CASE("eventbus/EventBus modification during nested notify",
		  "Remove listener during notification should not malform EventBus")
{
	Dexode::EventBus bus;
	struct TestEvent
	{};
	struct TestEvent2
	{};

	int token1 = 1;
	int token2 = 2;
	int token3 = 3;

	int calls = 0;

	bus.listen<TestEvent>(token1, [&](const TestEvent& event) {
		bus.notify(TestEvent2 {});

		++calls;
		bus.unlistenAll(token1);

		bus.listen<TestEvent>(token2, [&](const TestEvent& event) {
			++calls;
			bus.unlistenAll(token1);
			bus.unlistenAll(token3);
			bus.unlistenAll(token2);
		});
	});
	bus.listen<TestEvent>(token3, [&](const TestEvent& event) {
		++calls;
		bus.unlistenAll(token1);
		bus.unlistenAll(token2);
		bus.unlistenAll(token3);
	});

	REQUIRE_NOTHROW(bus.notify(TestEvent {}));
	REQUIRE(calls == 2);
	REQUIRE_NOTHROW(bus.notify(TestEvent {}));
	REQUIRE(calls == 2);
}
