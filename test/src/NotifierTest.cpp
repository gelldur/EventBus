//
// Created by Dawid Drozd aka Gelldur on 05.08.17.
//

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <dexode/EventBus.hpp>
#include <dexode/eventbus/strategy/Transaction.hpp>

using namespace dexode;
using TransactionEventBus = EventBus<eventbus::strategy::Transaction>;
using Listener = TransactionEventBus::Listener;

TEST_CASE("eventbus/Simple test", "Simple test")
{
	TransactionEventBus bus;
	struct SimpleEvent
	{
		int value;
	};

	auto listener = Listener::createNotOwning(bus);
	listener.listen<SimpleEvent>([](const SimpleEvent& event) { REQUIRE(event.value == 3); });

	bus.post(SimpleEvent{3});
	listener.unlistenAll();
	bus.post(SimpleEvent{2});

	listener.listen<SimpleEvent>([](const SimpleEvent& event) { REQUIRE(event.value == 1); });
	bus.post(SimpleEvent{1});
}

TEST_CASE("eventbus/Simple test2", "Simple test")
{
	TransactionEventBus bus;
	auto listener = Listener::createNotOwning(bus);

	struct SimpleEvent
	{
		int value;
	};

	listener.listen<SimpleEvent>([](const SimpleEvent& event) { REQUIRE(event.value == 3); });

	bus.post<SimpleEvent>({3});
	listener.unlistenAll();
	bus.post(SimpleEvent{2});

	listener.listen<SimpleEvent>([](const SimpleEvent& event) { REQUIRE(event.value == 1); });
	bus.post(SimpleEvent{1});
}

TEST_CASE("eventbus/EventBus listen & post",
		  "Listen & post without notification object. Using only string")
{
	int isCalled = 0;

	TransactionEventBus bus;
	auto listener = Listener::createNotOwning(bus);

	struct SimpleEvent
	{
		int value;
	};

	listener.listen<SimpleEvent>([&](const SimpleEvent& event) {
		++isCalled;
		REQUIRE(event.value == 3);
	});
	REQUIRE(isCalled == 0);
	bus.post(SimpleEvent{3});
	REQUIRE(isCalled == 1);
	listener.unlistenAll();
	bus.post(SimpleEvent{2});
	REQUIRE(isCalled == 1);

	listener.listen<SimpleEvent>([&](const SimpleEvent& event) {
		++isCalled;
		REQUIRE(event.value == 1);
	});
	bus.post(SimpleEvent{1});
	REQUIRE(isCalled == 2);
}

TEST_CASE("eventbus/Different notification", "Valid check notification")
{
	TransactionEventBus bus;
	auto listener = Listener::createNotOwning(bus);

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

	listener.listen<SimpleEvent1>([&called1](const SimpleEvent1& event) {
		called1 = true;
		REQUIRE(event.value == 1);
	});

	listener.listen<SimpleEvent2>([&called2](const SimpleEvent2& event) {
		called2 = true;
		REQUIRE(event.value == 2);
	});

	REQUIRE(called1 == false);

	bus.post(SimpleEvent1{1});

	REQUIRE(called1 == true);
	REQUIRE(called2 == false);
	called1 = false;

	bus.post(SimpleEvent2{2});

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
	TransactionEventBus bus;
	auto listener = Listener::createNotOwning(bus);

	listener.listen<Scope1::SimpleEvent>([&](const Scope1::SimpleEvent& event) {
		++isCalled;
		REQUIRE(event.value == 1);
	});
	listener.listen<Scope2::SimpleEvent>([&](const Scope2::SimpleEvent& event) {
		++isCalled;
		REQUIRE(event.value == 2);
	});
	REQUIRE(isCalled == 0);

	bus.post(Scope1::SimpleEvent{1});
	REQUIRE(isCalled == 1);

	bus.post(Scope2::SimpleEvent{2});
	REQUIRE(isCalled == 2);
}

TEST_CASE("eventbus/EventBus modification during post",
		  "Remove listener during notification should not malform EventBus")
{
	TransactionEventBus bus;
	auto listener = Listener::createNotOwning(bus);

	struct TestEvent
	{};

	//	int token1 = 1;
	//	int token2 = 2;

	int calls = 0;

	//	listener.listen<TestEvent>(token1, [&](const TestEvent& event) {
	//		++calls;
	//		bus.unlistenAll(token1);
	//		bus.unlistenAll(token2);
	//	});
	//	listener.listen<TestEvent>(token2, [&](const TestEvent& event) {
	//		++calls;
	//		bus.unlistenAll(token1);
	//		bus.unlistenAll(token2);
	//	});

	REQUIRE_NOTHROW(bus.post(TestEvent{}));
	REQUIRE(calls == 2);

	REQUIRE_NOTHROW(bus.post(TestEvent{}));
	REQUIRE(calls == 2);
}

TEST_CASE("eventbus/EventBus modification during post2",
		  "Remove listener during notification should not malform EventBus")
{
	TransactionEventBus bus;
	auto listener = Listener::createNotOwning(bus);

	struct TestEvent
	{};

	//	int token1 = 1;
	//	int token2 = 2;
	//	int token3 = 3;

	int calls = 0;

	//	bus.listen<TestEvent>(token1, [&](const TestEvent& event) {
	//		++calls;
	//		bus.unlistenAll(token1);
	//		bus.unlistenAll(token2);
	//		bus.unlistenAll(token3);
	//	});
	//	bus.listen<TestEvent>(token2, [&](const TestEvent& event) {
	//		++calls;
	//		bus.unlistenAll(token1);
	//		bus.unlistenAll(token2);
	//		bus.unlistenAll(token3);
	//	});
	//	bus.listen<TestEvent>(token3, [&](const TestEvent& event) {
	//		++calls;
	//		bus.unlistenAll(token1);
	//		bus.unlistenAll(token2);
	//		bus.unlistenAll(token3);
	//	});

	REQUIRE_NOTHROW(bus.post(TestEvent{}));
	REQUIRE(calls == 3);

	REQUIRE_NOTHROW(bus.post(TestEvent{}));
	REQUIRE(calls == 3);
}

TEST_CASE("eventbus/EventBus modification during post3",
		  "Remove listener during notification should not malform EventBus")
{
	TransactionEventBus bus;
	auto listener = Listener::createNotOwning(bus);

	struct TestEvent
	{};

	//	int token1 = 1;
	//	int token2 = 2;
	//	int token3 = 3;

	int calls = 0;

	//	bus.listen<TestEvent>(token1, [&](const TestEvent& event) {
	//		++calls;
	//		bus.unlistenAll(token1);
	//	});
	//	bus.listen<TestEvent>(token2, [&](const TestEvent& event) {
	//		++calls;
	//		bus.unlistenAll(token1);
	//		bus.unlistenAll(token3);
	//		bus.unlistenAll(token2);
	//	});
	//	bus.listen<TestEvent>(token3, [&](const TestEvent& event) {
	//		++calls;
	//		bus.unlistenAll(token1);
	//		bus.unlistenAll(token2);
	//		bus.unlistenAll(token3);
	//	});

	REQUIRE_NOTHROW(bus.post(TestEvent{}));
	REQUIRE(calls == 3);
}

TEST_CASE("eventbus/EventBus modification during post4",
		  "Remove listener during notification should not malform EventBus")
{
	TransactionEventBus bus;
	auto listener = Listener::createNotOwning(bus);

	struct TestEvent
	{};

	//	int token1 = 1;
	//	int token2 = 2;
	//	int token3 = 3;

	int calls = 0;

	//	bus.listen<TestEvent>(token1, [&](const TestEvent& event) {
	//		++calls;
	//		bus.unlistenAll(token1);
	//
	//		bus.listen<TestEvent>(token2, [&](const TestEvent& event) {
	//			++calls;
	//			bus.unlistenAll(token1);
	//			bus.unlistenAll(token3);
	//			bus.unlistenAll(token2);
	//		});
	//	});
	//	bus.listen<TestEvent>(token3, [&](const TestEvent& event) {
	//		++calls;
	//		bus.unlistenAll(token1);
	//		bus.unlistenAll(token2);
	//		bus.unlistenAll(token3);
	//	});

	REQUIRE_NOTHROW(bus.post(TestEvent{}));
	REQUIRE(calls == 2);
	REQUIRE_NOTHROW(bus.post(TestEvent{}));
	REQUIRE(calls == 2);
}

TEST_CASE("eventbus/EventBus modification during post5",
		  "Remove listener during notification should not malform EventBus")
{
	TransactionEventBus bus;
	auto listener = Listener::createNotOwning(bus);

	struct TestEvent
	{};

	//	int token1 = 1;
	//	int token2 = 2;
	//	int token3 = 3;

	int calls = 0;

	//	bus.listen<TestEvent>(token1, [&](const TestEvent& event) {
	//		++calls;
	//		bus.unlistenAll(token1);
	//	});
	//	bus.listen<TestEvent>(token2, [&](const TestEvent& event) {
	//		++calls;
	//		bus.unlistenAll(token1);
	//		bus.unlistenAll(token2);
	//
	//		bus.listen<TestEvent>(token3, [&](const TestEvent& event) {
	//			++calls;
	//			bus.unlistenAll(token1);
	//			bus.unlistenAll(token2);
	//			bus.unlistenAll(token3);
	//		});
	//	});

	REQUIRE_NOTHROW(bus.post(TestEvent{}));
	REQUIRE(calls == 2);

	REQUIRE_NOTHROW(bus.post(TestEvent{}));
	REQUIRE(calls == 3);
}

TEST_CASE("eventbus/EventBus modification during nested post",
		  "Remove listener during notification should not malform EventBus")
{
	TransactionEventBus bus;
	auto listener = Listener::createNotOwning(bus);

	struct TestEvent
	{};
	struct TestEvent2
	{};

	//	int token1 = 1;
	//	int token2 = 2;
	//	int token3 = 3;

	int calls = 0;

	//	bus.listen<TestEvent>(token1, [&](const TestEvent& event) {
	//		bus.post(TestEvent2{});
	//
	//		++calls;
	//		bus.unlistenAll(token1);
	//
	//		bus.listen<TestEvent>(token2, [&](const TestEvent& event) {
	//			++calls;
	//			bus.unlistenAll(token1);
	//			bus.unlistenAll(token3);
	//			bus.unlistenAll(token2);
	//		});
	//	});
	//	bus.listen<TestEvent>(token3, [&](const TestEvent& event) {
	//		++calls;
	//		bus.unlistenAll(token1);
	//		bus.unlistenAll(token2);
	//		bus.unlistenAll(token3);
	//	});

	REQUIRE_NOTHROW(bus.post(TestEvent{}));
	REQUIRE(calls == 2);
	REQUIRE_NOTHROW(bus.post(TestEvent{}));
	REQUIRE(calls == 2);
}
