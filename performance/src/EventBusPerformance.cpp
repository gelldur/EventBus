//
// Created by Dawid Drozd aka Gelldur on 05.08.17.
//
#include <random>

#include <benchmark/benchmark.h>

#include "dexode/EventBus.hpp"
#include "dexode/eventbus/strategy/Protected.hpp"
#include "dexode/eventbus/strategy/Transaction.hpp"

namespace
{

template <class Bus>
void checkNListeners(benchmark::State& state, const int listenersCount)
{
	Bus bus;

	struct SimpleEvent
	{
		int value;
	};

	std::vector<typename Bus::Listener> listeners;
	listeners.reserve(listenersCount);

	std::uint64_t sum = 0;
	for(int i = 0; i < listenersCount; ++i)
	{
		listeners.emplace_back(bus.createListener());
		listeners.back().template listen<SimpleEvent>(
			[&sum](const auto& event) { benchmark::DoNotOptimize(sum += event.value); });
	}

	while(state.KeepRunning()) // Performance area!
	{
		const auto event = SimpleEvent{1};
		bus.post(event);
	}
	state.counters["sum"] = sum;
}

template <class Strategy>
void checkSimpleNotification(benchmark::State& state)
{
	checkNListeners<dexode::EventBus<Strategy>>(state, 1);
}

template <class Strategy>
void check10Listeners(benchmark::State& state)
{
	checkNListeners<dexode::EventBus<Strategy>>(state, 10);
}

template <class Strategy>
void check100Listeners(benchmark::State& state)
{
	checkNListeners<dexode::EventBus<Strategy>>(state, 100);
}

template <class Strategy>
void check1kListeners(benchmark::State& state)
{
	checkNListeners<dexode::EventBus<Strategy>>(state, 1000);
}

void call1kLambdas_compare(benchmark::State& state)
{
	std::vector<std::function<void(int)>> callbacks;
	constexpr int listenersCount = 1000;
	callbacks.reserve(listenersCount);
	std::uint64_t sum = 0;
	for(int i = 0; i < listenersCount; ++i)
	{
		callbacks.emplace_back([&sum](int value) { benchmark::DoNotOptimize(sum += value); });
	}

	while(state.KeepRunning()) // Performance area!
	{
		for(int i = 0; i < listenersCount; ++i)
		{
			callbacks[i](1);
		}
	}
	state.counters["sum"] = sum;
}

template <int N>
struct UniqEvent
{
	int value;
};

template <class Bus>
void checkNNotificationsForNListeners(benchmark::State& state,
									  const int notificationsCount,
									  const int listenersCount)
{
	std::mt19937 generator(311281);
	std::uniform_int_distribution<int> uniformDistribution(0, 9);

	Bus bus;
	std::uint64_t sum = 0;

	std::vector<typename Bus::Listener> listeners;
	listeners.reserve(listenersCount);

	// We register M listeners for N notifications using uniform distribution
	for(int i = 0; i < listenersCount; ++i)
	{
		listeners.emplace_back(bus.createListener());
		const auto which = uniformDistribution(generator);

		// We generate here N different notifications
		switch(which)
		{
		case 0:
			listeners.back().template listen<UniqEvent<0>>(
				[&sum](const auto& event) { benchmark::DoNotOptimize(sum += event.value); });
			break;
		case 1:
			listeners.back().template listen<UniqEvent<1>>(
				[&sum](const auto& event) { benchmark::DoNotOptimize(sum += event.value); });
			break;
		case 2:
			listeners.back().template listen<UniqEvent<2>>(
				[&sum](const auto& event) { benchmark::DoNotOptimize(sum += event.value); });
			break;
		case 3:
			listeners.back().template listen<UniqEvent<3>>(
				[&sum](const auto& event) { benchmark::DoNotOptimize(sum += event.value); });
			break;
		case 4:
			listeners.back().template listen<UniqEvent<4>>(
				[&sum](const auto& event) { benchmark::DoNotOptimize(sum += event.value); });
			break;
		case 5:
			listeners.back().template listen<UniqEvent<5>>(
				[&sum](const auto& event) { benchmark::DoNotOptimize(sum += event.value); });
			break;
		case 6:
			listeners.back().template listen<UniqEvent<6>>(
				[&sum](const auto& event) { benchmark::DoNotOptimize(sum += event.value); });
			break;
		case 7:
			listeners.back().template listen<UniqEvent<7>>(
				[&sum](const auto& event) { benchmark::DoNotOptimize(sum += event.value); });
			break;
		case 8:
			listeners.back().template listen<UniqEvent<8>>(
				[&sum](const auto& event) { benchmark::DoNotOptimize(sum += event.value); });
			break;
		case 9:
			listeners.back().template listen<UniqEvent<9>>(
				[&sum](const auto& event) { benchmark::DoNotOptimize(sum += event.value); });
			break;
		default:
			throw std::runtime_error{"Nope"};
		}
	}

	while(state.KeepRunning()) // Performance area!
	{
		// Pick random notification
		const auto which = uniformDistribution(generator);
		// We generate here N different notifications
		switch(which)
		{
		case 0:
			bus.post(UniqEvent<0>{1});
			break;
		case 1:
			bus.post(UniqEvent<1>{2});
			break;
		case 2:
			bus.post(UniqEvent<2>{3});
			break;
		case 3:
			bus.post(UniqEvent<3>{4});
			break;
		case 4:
			bus.post(UniqEvent<4>{5});
			break;
		case 5:
			bus.post(UniqEvent<5>{6});
			break;
		case 6:
			bus.post(UniqEvent<6>{7});
			break;
		case 7:
			bus.post(UniqEvent<7>{8});
			break;
		case 8:
			bus.post(UniqEvent<8>{9});
			break;
		case 9:
			bus.post(UniqEvent<9>{10});
			break;
		default:
			throw std::runtime_error{"Nope"};
		}
	}
	state.counters["sum"] = sum;
}

template <class Strategy>
void check10NotificationsFor1kListeners(benchmark::State& state)
{
	checkNNotificationsForNListeners<dexode::EventBus<Strategy>>(state, 10, 1000);
}

template <class Strategy>
void check100NotificationsFor1kListeners(benchmark::State& state)
{
	checkNNotificationsForNListeners<dexode::EventBus<Strategy>>(state, 100, 1000);
}

template <class Strategy>
void check1kNotificationsFor1kListeners(benchmark::State& state)
{
	checkNNotificationsForNListeners<dexode::EventBus<Strategy>>(state, 1000, 1000);
}

template <class Strategy>
void check100NotificationsFor10kListeners(benchmark::State& state)
{
	checkNNotificationsForNListeners<dexode::EventBus<Strategy>>(state, 100, 10000);
}

template <class Strategy>
void checkNotifyFor10kListenersWhenNoOneListens(benchmark::State& state)
{
	using Listener = typename dexode::EventBus<Strategy>::Listener;
	dexode::EventBus<Strategy> bus;

	constexpr int listenersCount = 10000;

	std::uint64_t sum = 0;
	struct SimpleEvent
	{
		int value;
	};
	struct UnknownEvent
	{
		int value;
	};

	std::vector<Listener> listeners;
	listeners.reserve(listenersCount);
	for(int i = 0; i < listenersCount; ++i)
	{
		listeners.emplace_back(bus.createListener());
		listeners.back().template listen<SimpleEvent>(
			[&sum](const auto& event) { benchmark::DoNotOptimize(sum += event.value); });
	}

	while(state.KeepRunning()) // Performance area!
	{
		const auto unknownEvent = UnknownEvent{2};
		bus.post(unknownEvent); // this event doesn't have any listener
	}
	state.counters["sum"] = sum;
}
} // namespace

BENCHMARK(call1kLambdas_compare);

namespace strategy::Transaction
{

using Transaction = dexode::eventbus::strategy::Transaction;

BENCHMARK_TEMPLATE(checkSimpleNotification, Transaction);
BENCHMARK_TEMPLATE(check10Listeners, Transaction);
BENCHMARK_TEMPLATE(check100Listeners, Transaction);
BENCHMARK_TEMPLATE(check1kListeners, Transaction);

BENCHMARK_TEMPLATE(check10NotificationsFor1kListeners, Transaction);
BENCHMARK_TEMPLATE(check100NotificationsFor1kListeners, Transaction);
BENCHMARK_TEMPLATE(check1kNotificationsFor1kListeners, Transaction);
BENCHMARK_TEMPLATE(check100NotificationsFor10kListeners, Transaction);

BENCHMARK_TEMPLATE(checkNotifyFor10kListenersWhenNoOneListens, Transaction);

} // namespace strategy::Transaction

namespace strategy::Protected
{

using Protected = dexode::eventbus::strategy::Protected;

BENCHMARK_TEMPLATE(checkSimpleNotification, Protected);
BENCHMARK_TEMPLATE(check10Listeners, Protected);
BENCHMARK_TEMPLATE(check100Listeners, Protected);
BENCHMARK_TEMPLATE(check1kListeners, Protected);

BENCHMARK_TEMPLATE(check10NotificationsFor1kListeners, Protected);
BENCHMARK_TEMPLATE(check100NotificationsFor1kListeners, Protected);
BENCHMARK_TEMPLATE(check1kNotificationsFor1kListeners, Protected);
BENCHMARK_TEMPLATE(check100NotificationsFor10kListeners, Protected);

BENCHMARK_TEMPLATE(checkNotifyFor10kListenersWhenNoOneListens, Protected);

} // namespace strategy::Protected
