//
// Created by Dawid Drozd aka Gelldur on 05.08.17.
//
#include <random>

#include <benchmark/benchmark.h>
#include <eventbus/EventBus.h>

namespace
{

void checkNListeners(benchmark::State& state, const int listenersCount)
{
	Dexode::EventBus bus;
	int sum = 0;

	struct SimpleEvent
	{
		int value;
	};

	for(int i = 0; i < listenersCount; ++i)
	{
		bus.listen<SimpleEvent>(
		    [&](const SimpleEvent& event) { benchmark::DoNotOptimize(sum += event.value * 2); });
	}

	while(state.KeepRunning()) // Performance area!
	{
		const auto event = SimpleEvent{2};
		bus.notify(event);
	}
	state.counters["sum"] = sum;
}

void checkSimpleNotification(benchmark::State& state)
{
	checkNListeners(state, 1);
}

void check10Listeners(benchmark::State& state)
{
	checkNListeners(state, 10);
}

void check100Listeners(benchmark::State& state)
{
	checkNListeners(state, 100);
}

void check1kListeners(benchmark::State& state)
{
	checkNListeners(state, 1000);
}

void call1kLambdas_compare(benchmark::State& state)
{
	int sum = 0;
	std::vector<std::function<void(int)>> callbacks;
	callbacks.reserve(1000);
	for(int i = 0; i < 1000; ++i)
	{
		callbacks.emplace_back([&](int value) { benchmark::DoNotOptimize(sum += value * 2); });
	}

	while(state.KeepRunning()) // Performance area!
	{
		for(int i = 0; i < 1000; ++i)
		//		for (auto& callback :callbacks)
		{
			callbacks[i](2);
		}
	}
	state.counters["sum"] = sum;
}

template <int N>
struct SimpleEvent
{
	int value = N;
};

void checkNNotificationsForNListeners(benchmark::State& state,
                                      const int notificationsCount,
                                      const int listenersCount)
{
	std::mt19937 generator(311281);
	std::uniform_int_distribution<int> uniformDistribution(0, notificationsCount - 1);

	Dexode::EventBus bus;
	int sum = 0;
	for(int i = 0; i < listenersCount;
	    ++i) // We register M listeners for N notifications using uniform distribution
	{
		const auto which = uniformDistribution(generator);
		// We generate here N different notifications
		switch(which)
		{
		case 0:
			bus.listen<SimpleEvent<0>>([&](const auto& event) {
				benchmark::DoNotOptimize(sum += event.value *
				                                2); // we use it to prevent some? optimizations
			});
			break;
		case 1:
			bus.listen<SimpleEvent<1>>([&](const auto& event) {
				benchmark::DoNotOptimize(sum += event.value *
				                                2); // we use it to prevent some? optimizations
			});
			break;
		case 2:
			bus.listen<SimpleEvent<2>>([&](const auto& event) {
				benchmark::DoNotOptimize(sum += event.value *
				                                2); // we use it to prevent some? optimizations
			});
			break;
		case 3:
			bus.listen<SimpleEvent<3>>([&](const auto& event) {
				benchmark::DoNotOptimize(sum += event.value *
				                                2); // we use it to prevent some? optimizations
			});
			break;
		case 4:
			bus.listen<SimpleEvent<4>>([&](const auto& event) {
				benchmark::DoNotOptimize(sum += event.value *
				                                2); // we use it to prevent some? optimizations
			});
			break;
		case 5:
			bus.listen<SimpleEvent<5>>([&](const auto& event) {
				benchmark::DoNotOptimize(sum += event.value *
				                                2); // we use it to prevent some? optimizations
			});
			break;
		case 6:
			bus.listen<SimpleEvent<6>>([&](const auto& event) {
				benchmark::DoNotOptimize(sum += event.value *
				                                2); // we use it to prevent some? optimizations
			});
			break;
		case 7:
			bus.listen<SimpleEvent<7>>([&](const auto& event) {
				benchmark::DoNotOptimize(sum += event.value *
				                                2); // we use it to prevent some? optimizations
			});
			break;
		case 8:
			bus.listen<SimpleEvent<8>>([&](const auto& event) {
				benchmark::DoNotOptimize(sum += event.value *
				                                2); // we use it to prevent some? optimizations
			});
			break;
		case 9:
			bus.listen<SimpleEvent<9>>([&](const auto& event) {
				benchmark::DoNotOptimize(sum += event.value *
				                                2); // we use it to prevent some? optimizations
			});
			break;
		default:
			assert(false);
		}
	}

	while(state.KeepRunning()) // Performance area!
	{
		// Pick random notification
		const auto which = uniformDistribution(generator);
		const auto number = uniformDistribution(generator);
		// We generate here N different notifications
		switch(which)
		{
		case 0:
			bus.notify(SimpleEvent<0>{number});
			break;
		case 1:
			bus.notify(SimpleEvent<1>{number});
			break;
		case 2:
			bus.notify(SimpleEvent<2>{number});
			break;
		case 3:
			bus.notify(SimpleEvent<3>{number});
			break;
		case 4:
			bus.notify(SimpleEvent<4>{number});
			break;
		case 5:
			bus.notify(SimpleEvent<5>{number});
			break;
		case 6:
			bus.notify(SimpleEvent<6>{number});
			break;
		case 7:
			bus.notify(SimpleEvent<7>{number});
			break;
		case 8:
			bus.notify(SimpleEvent<8>{number});
			break;
		case 9:
			bus.notify(SimpleEvent<9>{number});
			break;
		default:
			assert(false);
		}
	}
	state.counters["sum"] = sum;
}

void check10NotificationsFor1kListeners(benchmark::State& state)
{
	checkNNotificationsForNListeners(state, 10, 1000);
}

// Sorry not available
// void check100NotificationsFor1kListeners(benchmark::State& state)
//{
//	checkNNotificationsForNListeners(state, 100, 1000);
//}
//
// void check1kNotificationsFor1kListeners(benchmark::State& state)
//{
//	checkNNotificationsForNListeners(state, 1000, 1000);
//}
//
// void check100NotificationsFor10kListeners(benchmark::State& state)
//{
//	checkNNotificationsForNListeners(state, 100, 10000);
//}

void checkNotifyFor10kListenersWhenNoOneListens(benchmark::State& state)
{
	Dexode::EventBus bus;
	int sum = 0;
	struct SimpleEvent
	{
		int value;
	};
	struct UnknownEvent
	{
		int value;
	};
	for(int i = 0; i < 10000; ++i)
	{
		bus.listen<SimpleEvent>(
		    [&](const SimpleEvent& event) { benchmark::DoNotOptimize(sum += event.value * 2); });
	}

	const auto unknownEvent = UnknownEvent{2};
	while(state.KeepRunning()) // Performance area!
	{
		bus.notify(unknownEvent);
	}
	state.counters["sum"] = sum;
}
}

BENCHMARK(call1kLambdas_compare);

BENCHMARK(checkSimpleNotification);
BENCHMARK(check10Listeners);
BENCHMARK(check100Listeners);
BENCHMARK(check1kListeners);
BENCHMARK(check10NotificationsFor1kListeners);
// BENCHMARK(check100NotificationsFor1kListeners); //Not available
// BENCHMARK(check1kNotificationsFor1kListeners); //Not available
// BENCHMARK(check100NotificationsFor10kListeners); //Not available
BENCHMARK(checkNotifyFor10kListenersWhenNoOneListens);
