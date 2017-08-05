//
// Created by Dawid Drozd aka Gelldur on 05.08.17.
//
#include <random>

#include <benchmark/benchmark.h>
#include <eventbus/Notifier.h>

namespace
{

MAKE_NOTIFICATION(SimpleNotification, int);

void checkSimpleNotification(benchmark::State& state)
{
	Dexode::Notifier bus;
	int sum = 0;
	bus.listen(getNotificationSimpleNotification(), [&](int value)
	{
		sum += value * 2;
	});

	while (state.KeepRunning())
	{
		bus.notify(getNotificationSimpleNotification(), 2);
	}
}

void check100Listeners(benchmark::State& state)
{
	Dexode::Notifier bus;
	int sum = 0;
	for (int i = 0; i < 100; ++i)
	{
		bus.listen(getNotificationSimpleNotification(), [&](int value)
		{
			sum += value * 2;
		});
	}

	while (state.KeepRunning())
	{
		bus.notify(getNotificationSimpleNotification(), 2);
	}
}

void check1kListeners(benchmark::State& state)
{
	Dexode::Notifier bus;
	int sum = 0;
	for (int i = 0; i < 1000; ++i)
	{
		bus.listen(getNotificationSimpleNotification(), [&](int value)
		{
			sum += value * 2;
		});
	}

	while (state.KeepRunning())
	{
		bus.notify(getNotificationSimpleNotification(), 2);
	}
}

void checkNNotificationsForNListeners(benchmark::State& state, const int notificationsCount, const int listenersCount)
{
	std::mt19937 generator(311281);
	std::uniform_int_distribution<int> uniformDistribution(0, notificationsCount - 1);

	//We generate here N different notifications
	std::vector<Dexode::Notification<int>> notifications;
	notifications.reserve(notificationsCount);
	for (int i = 0; i < notificationsCount; ++i)
	{
		notifications.emplace_back(691283);
	}

	Dexode::Notifier bus;
	int sum = 0;
	//We register 1k listeners for N notifications using uniform distribution
	for (int i = 0; i < listenersCount; ++i)
	{
		const auto& notification = notifications.at(uniformDistribution(generator));
		bus.listen(notification, [&](int value)
		{
			benchmark::DoNotOptimize(sum += value * 2);//we use it to prevent some? optimizations
		});
	}

	//Performance area!
	while (state.KeepRunning())
	{
		//Pick random notification
		const auto& notification = notifications.at(uniformDistribution(generator));
		bus.notify(notification, uniformDistribution(generator));
	}
}

void check10NotificationsFor1kListeners(benchmark::State& state)
{
	checkNNotificationsForNListeners(state, 10, 1000);
}

void check100NotificationsFor1kListeners(benchmark::State& state)
{
	checkNNotificationsForNListeners(state, 100, 1000);
}

void check1kNotificationsFor1kListeners(benchmark::State& state)
{
	checkNNotificationsForNListeners(state, 1000, 1000);
}

void check100NotificationsFor10kListeners(benchmark::State& state)
{
	checkNNotificationsForNListeners(state, 100, 10000);
}

}

BENCHMARK(checkSimpleNotification);
BENCHMARK(check100Listeners);
BENCHMARK(check1kListeners);
BENCHMARK(check10NotificationsFor1kListeners);
BENCHMARK(check100NotificationsFor1kListeners);
BENCHMARK(check1kNotificationsFor1kListeners);
BENCHMARK(check100NotificationsFor10kListeners);

BENCHMARK_MAIN();
