//
// Created by Dawid Drozd aka Gelldur on 05.08.17.
//
#include <random>

#include <benchmark/benchmark.h>
#include <eventbus/EventBus.h>

#include <iostream>

namespace
{

void checkNListeners(benchmark::State& state, const int listenersCount)
{
	Dexode::EventBus bus;
	int sum = 0;

	Dexode::Event<int> simpleNotification("simple");
	for (int i = 0; i < listenersCount; ++i)
	{
		bus.listen(simpleNotification, [&](int value)
		{
			benchmark::DoNotOptimize(sum += value * 2);
		});
	}

	while (state.KeepRunning())//Performance area!
	{
		bus.notify(simpleNotification, 2);
	}
	state.counters["sum"] = sum;
}

void checkSimpleNotification_EventBus2(benchmark::State& state)
{
	checkNListeners(state, 1);
}

void check10Listeners_EventBus2(benchmark::State& state)
{
	checkNListeners(state, 10);
}

void check100Listeners_EventBus2(benchmark::State& state)
{
	checkNListeners(state, 100);
}

void check1kListeners_EventBus2(benchmark::State& state)
{
	checkNListeners(state, 1000);
}

void call1kLambdas_compare_EventBus2(benchmark::State& state)
{
	int sum = 0;
	std::vector<std::function<void(int)>> callbacks;
	callbacks.reserve(1000);
	for (int i = 0; i < 1000; ++i)
	{
		callbacks.emplace_back([&](int value)
							   {
								   benchmark::DoNotOptimize(sum += value * 2);
							   });
	}

	while (state.KeepRunning())//Performance area!
	{
		for (int i = 0; i < 1000; ++i)
			//		for (auto& callback :callbacks)
		{
			callbacks[i](2);
		}
	}
	state.counters["sum"] = sum;
}

void checkNNotificationsForNListeners(benchmark::State& state, const int notificationsCount, const int listenersCount)
{
	std::mt19937 generator(311281);
	std::uniform_int_distribution<int> uniformDistribution(0, notificationsCount - 1);

	//We generate here N different notifications
	std::vector<Dexode::Event<int>> notifications;
	notifications.reserve(notificationsCount);
	for (int i = 0; i < notificationsCount; ++i)
	{
		notifications.emplace_back(std::string{"notify_"} + std::to_string(i));
	}

	Dexode::EventBus bus;
	int sum = 0;
	for (int i = 0; i < listenersCount; ++i)//We register M listeners for N notifications using uniform distribution
	{
		const auto& notification = notifications.at(uniformDistribution(generator));
		bus.listen(notification, [&](int value)
		{
			benchmark::DoNotOptimize(sum += value * 2);//we use it to prevent some? optimizations
		});
	}

	while (state.KeepRunning())//Performance area!
	{
		//Pick random notification
		const auto& notification = notifications.at(uniformDistribution(generator));
		bus.notify(notification, uniformDistribution(generator));
	}
	state.counters["sum"] = sum;
}

void check10NotificationsFor1kListeners_EventBus2(benchmark::State& state)
{
	checkNNotificationsForNListeners(state, 10, 1000);
}

void check100NotificationsFor1kListeners_EventBus2(benchmark::State& state)
{
	checkNNotificationsForNListeners(state, 100, 1000);
}

void check1kNotificationsFor1kListeners_EventBus2(benchmark::State& state)
{
	checkNNotificationsForNListeners(state, 1000, 1000);
}

void check100NotificationsFor10kListeners_EventBus2(benchmark::State& state)
{
	checkNNotificationsForNListeners(state, 100, 10000);
}

void checkNotifyFor10kListenersWhenNoOneListens_EventBus2(benchmark::State& state)
{
	Dexode::EventBus bus;
	Dexode::Event<int> simpleNotification("simple");
	Dexode::Event<int> unknownNotification("unknown");
	int sum = 0;
	for (int i = 0; i < 10000; ++i)
	{
		bus.listen(simpleNotification, [&](int value)
		{
			benchmark::DoNotOptimize(sum += value * 2);
		});
	}

	while (state.KeepRunning())//Performance area!
	{
		bus.notify(unknownNotification, 2);
	}
	state.counters["sum"] = sum;
}

}

BENCHMARK(call1kLambdas_compare_EventBus2);

BENCHMARK(checkSimpleNotification_EventBus2);
BENCHMARK(check10Listeners_EventBus2);
BENCHMARK(check100Listeners_EventBus2);
BENCHMARK(check1kListeners_EventBus2);
BENCHMARK(check10NotificationsFor1kListeners_EventBus2);
BENCHMARK(check100NotificationsFor1kListeners_EventBus2);
BENCHMARK(check1kNotificationsFor1kListeners_EventBus2);
BENCHMARK(check100NotificationsFor10kListeners_EventBus2);
BENCHMARK(checkNotifyFor10kListenersWhenNoOneListens_EventBus2);
