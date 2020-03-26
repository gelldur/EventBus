#include <vector>

#include <catch2/catch.hpp>

#include "dexode/EventBus.hpp"
#include "dexode/eventbus/test/event.hpp"

namespace dexode::eventbus::test
{

using Listener = dexode::EventBus::Listener;

TEST_CASE("Should remove all listeners When use unlistenAll", "[EventBus][Listener]")
{
	EventBus bus;
	auto listener = Listener::createNotOwning(bus);

	int callCount = 0;
	listener.listen([&](const event::Value& event) {
		REQUIRE(event.value == 3);
		++callCount;
	});
	listener.listen([&](const event::T1& event) { ++callCount; });

	bus.postpone(event::Value{3});
	bus.postpone(event::T1{});
	REQUIRE(bus.process() == 2);
	REQUIRE(callCount == 2);

	listener.unlistenAll();

	bus.postpone(event::Value{2});
	bus.postpone(event::T1{});
	REQUIRE(bus.process() == 2);
	REQUIRE(callCount == 2); // unchanged
}

TEST_CASE("Should unlisten all events When listener instance is overriden", "[EventBus][Listener]")
{
	EventBus bus;
	int callCount = 0;
	auto listener = Listener::createNotOwning(bus);
	listener.listen<event::Value>([&](const event::Value& event) {
		REQUIRE(event.value == 3);
		++callCount;
	});
	bus.postpone(event::Value{3});
	REQUIRE(bus.process() == 1);
	REQUIRE(callCount == 1);

	listener.transfer(Listener{});

	bus.postpone(event::Value{2});
	REQUIRE(bus.process() == 1);
	REQUIRE(callCount == 1);
}

TEST_CASE("Should unlisten all events When listener instance is destroyed", "[EventBus][Listener]")
{
	EventBus bus;
	int callCount = 0;

	{
		auto listener = Listener::createNotOwning(bus);
		listener.listen<event::Value>([&](const event::Value& event) {
			REQUIRE(event.value == 3);
			++callCount;
		});
		bus.postpone(event::Value{3});
		REQUIRE(bus.process() == 1);
		REQUIRE(callCount == 1);
	}

	bus.postpone(event::Value{2});
	REQUIRE(bus.process() == 1);
	REQUIRE(callCount == 1);
}

TEST_CASE("Should keep listeners When listener is moved", "[EventBus][Listener]")
{
	auto bus = std::make_shared<EventBus>();
	int callCount = 0;

	Listener transferOne;
	{
		Listener listener{bus};
		listener.listen<event::Value>([&](const event::Value& event) {
			REQUIRE(event.value == 3);
			++callCount;
		});
		bus->postpone(event::Value{3});
		REQUIRE(bus->process() == 1);
		REQUIRE(callCount == 1);

		transferOne.transfer(std::move(listener));
	}

	bus->postpone(event::Value{3});
	REQUIRE(bus->process() == 1);
	REQUIRE(callCount == 2);
}

TEST_CASE("Should receive event When listener added AFTER event emit but BEFORE event porcess",
		  "[EventBus][Listener]")
{
	auto bus = std::make_shared<EventBus>();
	int callCount = 0;
	bus->postpone(event::Value{22});

	Listener listener{bus};
	listener.listen<event::Value>([&](const event::Value& event) {
		REQUIRE(event.value == 22);
		++callCount;
	});

	REQUIRE(callCount == 0);
	bus->process();
	REQUIRE(callCount == 1);
}

TEST_CASE("Should bind listen to class method and unbind When clazz dtor is called",
		  "[EventBus][Listener]")
{
	auto bus = std::make_shared<EventBus>();
	struct TestBind
	{
		int callCount = 0;
		Listener listener;

		TestBind(const std::shared_ptr<EventBus>& bus)
			: listener{bus}
		{
			listener.listen<event::T1>(std::bind(&TestBind::onEvent, this, std::placeholders::_1));
		}

		void onEvent(const event::T1& event)
		{
			++callCount;
		}
	};

	bus->postpone(event::T1{});
	{
		TestBind bindClazz{bus};
		REQUIRE(bindClazz.callCount == 0);
		CHECK(bus->process() == 1);
		REQUIRE(bindClazz.callCount == 1);
	}
	bus->postpone(event::T1{});
	CHECK(bus->process() == 1);
}

static int globalCallCount{0}; // bad practice but want to just test
void freeFunction(const event::T1& event)
{
	++globalCallCount;
}

TEST_CASE("Should compile When listen in different forms", "[EventBus][Listener]")
{
	EventBus bus;

	int callCount = 0;

	// Listen with lambda
	auto listener = Listener::createNotOwning(bus);
	listener.listen([&](const event::T1& event) { ++callCount; });

	// Listen with std::function
	auto listener2 = Listener::createNotOwning(bus);
	std::function<void(const event::T1&)> callback = [&](const event::T1&) { ++callCount; };
	listener2.listen(callback);

	// Listen with std::bind
	auto listener3 = Listener::createNotOwning(bus);
	struct TestClazz
	{
		int clazzCallCount{0};
		void onEvent(const event::T1& event)
		{
			++clazzCallCount;
		}
	};
	TestClazz clazz;
	listener3.listen<event::T1>(std::bind(&TestClazz::onEvent, &clazz, std::placeholders::_1));

	// Listen with free function
	auto listener4 = Listener::createNotOwning(bus);
	listener4.listen(freeFunction);

	bus.postpone(event::T1{});
	bus.process();

	REQUIRE(globalCallCount == 1);
	REQUIRE(clazz.clazzCallCount == 1);
	REQUIRE(callCount == 2);
}

TEST_CASE("Should NOT be able to add multiple same event callbacks When using same listener",
		  "[EventBus][Listener]")
{
	/// User should use separate Listener instance as it would be unabigious what should happen when
	/// call unlisten<Event>
	EventBus bus;
	auto listener = Listener::createNotOwning(bus);

	listener.listen([](const event::Value& event) {});
	REQUIRE_THROWS(listener.listen([](const event::Value& event) {}));
}

TEST_CASE("Should compile", "[EventBus][Listener]")
{
	// Test case to check for compilation
	EventBus bus;
	{
		auto listener = Listener::createNotOwning(bus);
		const auto callback = [](const event::Value& event) {};
		listener.listen(callback);
	}
	{
		auto listener = Listener::createNotOwning(bus);
		auto callback = [](const event::Value& event) {};
		listener.listen(callback);
	}
	{
		auto listener = Listener::createNotOwning(bus);
		auto callback = [](const event::Value& event) {};
		listener.listen(std::move(callback));
	}
	{
		auto listener = Listener::createNotOwning(bus);
		listener.listen([](const event::Value& event) {});
	}
}

class TestClazz
{
public:
	static int counter;

	TestClazz(int id, const std::shared_ptr<EventBus>& bus)
		: _id{id}
		, _listener{bus}
	{
		registerListener();
	}

	TestClazz(TestClazz&& other)
		: _id{other._id}
		, _listener{other._listener.getBus()}
	{
		// We need to register again
		registerListener();
	}

	~TestClazz()
	{
		_id = 0;
	}

private:
	int _id = 0;
	EventBus::Listener _listener;

	void registerListener()
	{
		_listener.listen([this](const event::Value& event) {
			if(_id == 1)
			{
				++counter;
			}
		});
	}
};

int TestClazz::counter = 0;

TEST_CASE("Should not allow for mistake with move ctor", "[EventBus][Listener]")
{
	/**
	 * Test case TAG: FORBID_MOVE_LISTENER
	 *
	 * This case is little bit complicated.
	 * We can't move EventBus::Listener as it capture 'this' in ctor so whenever we would use it it
	 * would lead to UB.
	 */
	std::shared_ptr<EventBus> bus = std::make_shared<EventBus>();

	std::vector<TestClazz> vector;
	vector.emplace_back(1, bus);
	vector.emplace_back(2, bus);
	vector.emplace_back(3, bus);

	bus->postpone(event::Value{100});
	bus->process();

	REQUIRE(TestClazz::counter == 1);
}

} // namespace dexode::eventbus::test
