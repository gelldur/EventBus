//
// Created by Dawid Drozd aka Gelldur on 24.07.18.
//
#include <functional>
#include <random>

#include <Poco/NObserver.h>
#include <Poco/Notification.h>
#include <Poco/NotificationCenter.h>
#include <benchmark/benchmark.h>

namespace
{

template <class T>
struct Wrapper : public Poco::Notification
{
	T data;

	Wrapper(T data)
		: data(std::move(data))
	{}
};

template <class Egg>
class Target
{
public:
	Target(std::function<void(const Egg&)> callback)
		: _callback(std::move(callback))
	{}

	void handle(const Poco::AutoPtr<Wrapper<Egg>>& event)
	{
		_callback((*event.get()).data);
	}

private:
	std::function<void(const Egg&)> _callback;
};

void checkNListeners(benchmark::State& state, const int listenersCount)
{
	Poco::NotificationCenter bus;
	int sum = 0;

	struct SimpleEvent
	{
		int value;
	};

	using MyEvent = Wrapper<SimpleEvent>;
	using Listener = Target<SimpleEvent>;

	std::vector<Listener> targets;
	targets.reserve(listenersCount + 1);

	for(int i = 0; i < listenersCount; ++i)
	{
		targets.emplace_back(
			[&](const SimpleEvent& event) { benchmark::DoNotOptimize(sum += event.value * 2); });

		bus.addObserver(Poco::NObserver<Listener, MyEvent>(targets.back(), &Listener::handle));
	}

	while(state.KeepRunning()) // Performance area!
	{
		const Poco::AutoPtr<MyEvent> event = new Wrapper<SimpleEvent>{SimpleEvent{2}};
		bus.postNotification(event);
	}
	state.counters["sum"] = sum;
}

void Poco_checkSimpleNotification(benchmark::State& state)
{
	checkNListeners(state, 1);
}

void Poco_check10Listeners(benchmark::State& state)
{
	checkNListeners(state, 10);
}

void Poco_check100Listeners(benchmark::State& state)
{
	checkNListeners(state, 100);
}

void Poco_check1kListeners(benchmark::State& state)
{
	checkNListeners(state, 1000);
}

BENCHMARK(Poco_checkSimpleNotification);
BENCHMARK(Poco_check10Listeners);
BENCHMARK(Poco_check100Listeners);
BENCHMARK(Poco_check1kListeners);
}
