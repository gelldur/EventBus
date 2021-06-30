#include <variant>

#include <catch2/catch.hpp>

#include "dexode/EventBus.hpp"
#include "dexode/eventbus/test/event.hpp"

namespace dexode::eventbus::test
{

// class TestProducer
//{
// public:
//	using Events = std::variant<event::T1, event::T2>;
//
//	void onUpdate(TransactionEventBus& bus)
//	{
//		bus.postpone(event::T1{});
//	}
//
// private:
//};

// TEST_CASE("Should process events independently When postponed multiple event types",
// "[EventBus]")
//{
//	EventBus bus;
//
//	int event1CallCount = 0;
//	int event2CallCount = 0;
//
//	auto listener = Listener::createNotOwning(bus);
//	listener.listen<event::T1>([&](const event::T1& event) { ++event1CallCount; });
//	listener.listen2([&](const event::T2& event) { ++event2CallCount; });
//
//	bus.postpone(event::T1{});
//	{
//		event::T2 localVariable;
//		bus.postpone(localVariable);
//	}
//
//	REQUIRE(event1CallCount == 0);
//	REQUIRE(event2CallCount == 0);
//	REQUIRE(bus.process<event::T1>() == 1);
//	REQUIRE(event1CallCount == 1);
//	REQUIRE(event2CallCount == 0);
//
//	REQUIRE(bus.process<event::T2>() == 1);
//	REQUIRE(event1CallCount == 1);
//	REQUIRE(event2CallCount == 1);
//}

TEST_CASE("Should deliver event with desired value When postpone event", "[EventBus]")
{
	EventBus bus;
	auto listener = EventBus::Listener::createNotOwning(bus);

	int callCount = 0;

	listener.listen([&](const event::Value& event) {
		REQUIRE(event.value == 3);
		++callCount;
	});

	REQUIRE(callCount == 0);
	bus.postpone(event::Value{3});
	REQUIRE(callCount == 0);
	REQUIRE(bus.process() == 1);
	REQUIRE(callCount == 1);
}

TEST_CASE("Should be able to replace listener When we do listen -> unlisten -> listen",
		  "[EventBus]")
{
	EventBus bus;
	auto listener = EventBus::Listener::createNotOwning(bus);

	int callCount = 0;

	{
		// TODO validate other signatures is it possible to make mistake? (maybe fail to build tests
		// ?)
		listener.listen([&](const event::Value& event) {
			REQUIRE(event.value == 3);
			++callCount;
		});
		bus.postpone(event::Value{3});
		REQUIRE(bus.process() == 1);
		REQUIRE(callCount == 1);
	}

	{
		listener.unlisten<event::Value>();
		bus.postpone(event::Value{2});
		REQUIRE(bus.process() == 1);
		REQUIRE(callCount == 1);
	}

	{
		listener.listen([&](const event::Value& event) {
			REQUIRE(event.value == 1);
			++callCount;
		});

		bus.postpone(event::Value{1});
		REQUIRE(bus.process() == 1);
		REQUIRE(callCount == 2);
	}
}

TEST_CASE("Should be able to replace listener When we do listen -> unlistenAll -> listen",
		  "[EventBus]")
{
	EventBus bus;
	auto listener = EventBus::Listener::createNotOwning(bus);

	int callCount = 0;
	{
		// TODO validate other signatures is it possible to make mistake? (maybe fail to build tests
		// ?)
		listener.listen([&](const event::Value& event) {
			REQUIRE(event.value == 3);
			++callCount;
		});
		listener.listen([&](const event::T1& event) { ++callCount; });
		bus.postpone(event::Value{3});
		bus.postpone(event::T1{});
		REQUIRE(bus.process() == 2);
		REQUIRE(callCount == 2);
	}

	{
		listener.unlistenAll();
		bus.postpone(event::Value{3});
		bus.postpone(event::T1{});
		REQUIRE(bus.process() == 2);
		REQUIRE(callCount == 2); // no new calls
	}

	{
		listener.listen([&](const event::Value& event) {
			REQUIRE(event.value == 1);
			++callCount;
		});

		bus.postpone(event::Value{1});
		bus.postpone(event::T1{});
		REQUIRE(bus.process() == 2);
		REQUIRE(callCount == 3);
	}
}

TEST_CASE("Should flush events When no one listens", "[EventBus]")
{
	EventBus bus;
	auto listener = EventBus::Listener::createNotOwning(bus);

	bus.postpone(event::Value{3});
	bus.postpone(event::Value{3});
	bus.postpone(event::Value{3});
	REQUIRE(bus.process() == 3); // it shouldn't accumulate events
}

TEST_CASE("Should be able to unlisten When processing event", "[EventBus]")
{
	EventBus bus;
	auto listener = EventBus::Listener::createNotOwning(bus);

	int callCount = 0;
	listener.listen([&](const event::Value& event) {
		++callCount;
		listener.unlisten<event::Value>();
	});

	bus.postpone(event::Value{3});
	bus.postpone(event::Value{3});
	bus.postpone(event::Value{3});
	REQUIRE(bus.process() == 3); // it shouldn't accumulate events
	REQUIRE(callCount == 1);
}

TEST_CASE("Should not fail When unlisten multiple times during processing event", "[EventBus]")
{
	EventBus bus;
	auto listener = EventBus::Listener::createNotOwning(bus);

	int callCount = 0;
	listener.listen([&](const event::Value& event) {
		REQUIRE_NOTHROW(listener.unlisten<event::Value>());
		REQUIRE_NOTHROW(listener.unlisten<event::Value>());
		REQUIRE_NOTHROW(listener.unlisten<event::Value>());
		++callCount;
	});

	bus.postpone(event::Value{3});
	bus.postpone(event::Value{3});
	bus.postpone(event::Value{3});
	REQUIRE(bus.process() == 3); // it shouldn't accumulate events
	REQUIRE(callCount == 1);
}

TEST_CASE("Should fail When try to add not unique listener", "[EventBus]")
{
	EventBus bus;
	auto listener = EventBus::Listener::createNotOwning(bus);

	REQUIRE_NOTHROW(listener.listen([](const event::T1&) {}));
	REQUIRE_THROWS(listener.listen([](const event::T1&) {})); // We already added listener
}

TEST_CASE("Should be able to add listener When processing event", "[EventBus]")
{
	EventBus bus;
	auto listener = EventBus::Listener::createNotOwning(bus);
	auto listenerOther = EventBus::Listener::createNotOwning(bus);

	int callCount = 0;
	int callCountOther = 0;
	listener.listen([&](const event::Value& event) {
		++callCount;
		if(callCount == 1) // remember that we can only add it once!
		{
			listenerOther.listen(
				[&callCountOther](const event::Value& event) { ++callCountOther; });
		}
	});

	{
		bus.postpone(event::Value{3});
		REQUIRE(bus.process() == 1);
		REQUIRE(callCount == 1);
		REQUIRE(callCountOther == 0);

		bus.postpone(event::Value{3});
		REQUIRE(bus.process() == 1);
		REQUIRE(callCount == 2);
		REQUIRE(callCountOther == 1);
	}
	{
		listenerOther.unlistenAll();
		callCount = 0;
		callCountOther = 0;
		bus.postpone(event::Value{3});
		bus.postpone(event::Value{3});
		bus.postpone(event::Value{3});
		REQUIRE(bus.process() == 3); // it shouldn't accumulate events
		REQUIRE(callCount == 3);
		REQUIRE(callCountOther == 2);
	}
}

TEST_CASE("Should be able to add listener and unlisten When processing event", "[EventBus]")
{
	EventBus bus;
	auto listener = EventBus::Listener::createNotOwning(bus);
	auto listenerOther = EventBus::Listener::createNotOwning(bus);

	int callCount = 0;
	int callCountOther = 0;
	listener.listen([&](const event::Value& event) {
		++callCount;
		if(callCount == 1) // remember that we can only add it once!
		{
			listenerOther.listen([&](const event::Value& event) { ++callCountOther; });
		}
		listener.unlistenAll();
	});

	bus.postpone(event::Value{3});
	bus.postpone(event::Value{3});
	bus.postpone(event::Value{3});
	REQUIRE(bus.process() == 3);
	REQUIRE(callCount == 1);
	REQUIRE(callCountOther == 2);
}

TEST_CASE(
	"Should be able to add listener and remove listener in Matryoshka style When processing event",
	"[EventBus]")
{
	EventBus bus;
	auto listener1 = EventBus::Listener::createNotOwning(bus);
	auto listener2 = EventBus::Listener::createNotOwning(bus);
	auto listener3 = EventBus::Listener::createNotOwning(bus);

	int callCount = 0;

	listener1.listen([&](const event::Value& event) {
		listener1.unlistenAll(); // Level 1

		listener2.listen([&](const event::Value& event) {
			listener2.unlistenAll(); // Level 2

			listener3.listen([&](const event::Value& event) {
				listener3.unlistenAll(); // Level 3 (final)
				++callCount;
			});
			++callCount;
		});
		++callCount;
	});

	bus.postpone(event::Value{3});
	bus.postpone(event::Value{3});
	bus.postpone(event::Value{3});
	bus.postpone(event::Value{3});
	REQUIRE(bus.process() == 4);
	REQUIRE(callCount == 3);
}

TEST_CASE("Should be chain listen and unlisten When processing event", "[EventBus]")
{
	EventBus bus;
	auto listener = EventBus::Listener::createNotOwning(bus);
	auto listenerOther = EventBus::Listener::createNotOwning(bus);

	int callCount = 0;
	int callOtherOption = 0;
	listener.listen([&](const event::Value& event) {
		++callCount;
		// remember that we can only add it once!
		listenerOther.listen([&](const event::Value& event) { callOtherOption = 1; });
		listenerOther.unlisten<event::Value>();
		listenerOther.listen([&](const event::Value& event) { callOtherOption = 2; });
		listenerOther.unlisten<event::Value>();
		listenerOther.listen([&](const event::Value& event) { callOtherOption = 3; });
		listenerOther.unlisten<event::Value>();

		listenerOther.listen([&](const event::Value& event) { callOtherOption = 4; });

		listener.unlistenAll();
	});

	bus.postpone(event::Value{3});
	bus.postpone(event::Value{3});
	bus.postpone(event::Value{3});
	REQUIRE(bus.process() == 3);
	REQUIRE(callCount == 1);
	REQUIRE(callOtherOption == 4);
}

TEST_CASE("Should not process events When no more events", "[EventBus]")
{
	EventBus bus;
	auto listener = EventBus::Listener::createNotOwning(bus);

	bus.postpone(event::Value{3});
	bus.postpone(event::Value{3});
	bus.postpone(event::Value{3});
	REQUIRE(bus.process() == 3); // it shouldn't accumulate events
	REQUIRE(bus.process() == 0);
}

TEST_CASE("Should process event When listener transit", "[EventBus]")
{
	/**
	 * This case may be usefull when we use EventBus for some kind of state machine and we are
	 * during transit from one state to other.
	 */
	EventBus bus;
	auto listenerA = EventBus::Listener::createNotOwning(bus);
	auto listenerB = EventBus::Listener::createNotOwning(bus);

	int listenerAReceiveEvent = 0;
	int listenerBReceiveEvent = 0;

	listenerA.listen([&](const event::Value& event) { ++listenerAReceiveEvent; });

	REQUIRE(bus.process() == 0);

	// All cases should be same because of deterministic way of processing
	SECTION("Post event before transit")
	{
		bus.postpone(event::Value{3}); // <-- before

		listenerA.unlistenAll();
		listenerB.listen([&](const event::Value& event) { ++listenerBReceiveEvent; });
	}
	SECTION("Post event in transit")
	{
		listenerA.unlistenAll();
		bus.postpone(event::Value{3}); // <-- in
		listenerB.listen([&](const event::Value& event) { ++listenerBReceiveEvent; });
	}
	SECTION("Post event after transit")
	{
		listenerA.unlistenAll();
		listenerB.listen([&](const event::Value& event) { ++listenerBReceiveEvent; });

		bus.postpone(event::Value{3}); // <-- after
	}

	REQUIRE(bus.process() == 1);
	CHECK(listenerAReceiveEvent == 0);
	CHECK(listenerBReceiveEvent == 1);
}

TEST_CASE("Should NOT process event When listener unlisten before process", "[EventBus]")
{
	EventBus bus;
	auto listener = EventBus::Listener::createNotOwning(bus);

	int listenerReceiveEvent = 0;

	listener.listen([&](const event::Value& event) { ++listenerReceiveEvent; });

	REQUIRE(bus.process() == 0);
	bus.postpone(event::Value{3});
	REQUIRE(bus.process() == 1);
	CHECK(listenerReceiveEvent == 1);

	// All cases should be same because of deterministic way of processing
	SECTION("Post event before unlisten")
	{
		bus.postpone(event::Value{3}); // <-- before
		listener.unlistenAll();
	}
	SECTION("Post event after transit")
	{
		listener.unlistenAll();
		bus.postpone(event::Value{3}); // <-- after
	}

	REQUIRE(bus.process() == 1);
	CHECK(listenerReceiveEvent == 1);
}

TEST_CASE("Should distinguish event producer When", "[EventBus]")
{
	//	EventBus bus;
	//
	//	//	dexode::eventbus::WrapTag<event::T1, Tag, Tag::gui> e;
	//
	//	int counterGui = 0;
	//	int counterBackend = 0;
	//	auto listener = EventBus::Listener::createNotOwning(bus);
	//
	//	listener.listen([&](const event::Tag<event::T1>& event) {
	//		if(event.tag == "gui")
	//		{
	//			++counterGui;
	//		}
	//		else if(event.tag == "backend")
	//		{
	//			++counterBackend;
	//		}
	//		else
	//		{
	//			FAIL();
	//		}
	//	});
	//
	//	auto producerGui = [tag = Tag::gui](EventBus& bus) {
	//		const event::T1 event;
	//		bus.postpone(event);
	//	};
	//
	//	auto producerBackend = [](EventBus& bus) {
	//		const event::T1 event;
	//		bus.postpone(Tag::backend, event);
	//	};
	//
	//	auto producerNoTag = [](EventBus& bus) {
	//		const event::T1 event;
	//		bus.postpone(event);
	//	};
	//
	//	auto producerGeneric = [](EventBus& bus, Tag tag) {
	//		const event::T1 event;
	//		if(tag == Tag::none)
	//		{
	//			bus.postpone(event);
	//		}
	//		else
	//		{
	//			bus.postpone(tag, event);
	//		}
	//	};
	//
	//	producerGui(bus);
	//	producerGui(bus);
	//	producerBackend(bus);
	//	producerGui(bus);
	//	producerNoTag(bus);
	//	producerGeneric(bus, Tag::none);
	//	producerGeneric(bus, Tag::backend);
	//
	//	CHECK(bus.process() == 7);
	//
	//	REQUIRE(counterGui == 3);
	//	REQUIRE(counterBackend == 2);
}

} // namespace dexode::eventbus::test

// TODO should listen work with bind'ing e.g.
//_listener.listen<dashboard::back::events::LoadOrders>(
//	std::bind(&BackDashboard::onLoadOrders, this, std::placeholders::_1));
