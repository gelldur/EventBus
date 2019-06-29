//
// Created by Dawid Drozd aka Gelldur on 05.08.17.
//
#include <functional>
#include <random>

#include <benchmark/benchmark.h>
#include <cocoa/CCInteger.h>
#include <cocos2d-x/cocos2dx/cocoa/CCAutoreleasePool.h>
#include <support/CCNotificationCenter.h>

namespace cocos2dx
{

struct SampleObserver : public cocos2d::CCObject
{
	std::function<void(int)> callback;

	void onCall(cocos2d::CCObject* object)
	{
		callback(static_cast<cocos2d::CCInteger*>(object)->getValue());
	}
};

void checkNListeners(benchmark::State& state, const int listenersCount)
{
	using namespace cocos2d;
	cocos2d::CCNotificationCenter bus;
	int sum = 0;
	for(int i = 0; i < listenersCount; ++i)
	{
		auto observer = new SampleObserver{};
		observer->autorelease();
		observer->callback = [&](int value) { benchmark::DoNotOptimize(sum += value * 2); };
		bus.addObserver(observer, callfuncO_selector(SampleObserver::onCall), "sample", nullptr);
	}
	auto number = CCInteger::create(2);
	while(state.KeepRunning()) // Performance area!
	{
		bus.postNotification("sample", number);
	}
	state.counters["sum"] = sum;
	CCPoolManager::sharedPoolManager()->purgePoolManager();
}

void checkSimpleNotification_CCNotificationCenter(benchmark::State& state)
{
	checkNListeners(state, 1);
}

void check10Listeners_CCNotificationCenter(benchmark::State& state)
{
	checkNListeners(state, 10);
}

void check100Listeners_CCNotificationCenter(benchmark::State& state)
{
	checkNListeners(state, 100);
}

void check1kListeners_CCNotificationCenter(benchmark::State& state)
{
	checkNListeners(state, 1000);
}

void checkNNotificationsForNListeners(benchmark::State& state,
									  const int notificationsCount,
									  const int listenersCount)
{
	std::mt19937 generator(311281);
	std::uniform_int_distribution<int> uniformDistribution(0, notificationsCount - 1);

	using namespace cocos2d;
	cocos2d::CCNotificationCenter bus;

	// We generate here N different notifications
	std::vector<std::string> notifications;
	notifications.reserve(notificationsCount);
	for(int i = 0; i < notificationsCount; ++i)
	{
		notifications.emplace_back(std::string{"notify_"} + std::to_string(i));
	}

	int sum = 0;
	for(int i = 0; i < listenersCount;
		++i) // We register M listeners for N notifications using uniform distribution
	{
		auto observer = new SampleObserver{};
		observer->autorelease();
		observer->callback = [&](int value) { benchmark::DoNotOptimize(sum += value * 2); };

		const auto& notification = notifications.at(uniformDistribution(generator));
		bus.addObserver(
			observer, callfuncO_selector(SampleObserver::onCall), notification.c_str(), nullptr);
	}
	auto number = CCInteger::create(2);

	while(state.KeepRunning()) // Performance area!
	{
		// Pick random notification
		const auto& notification = notifications.at(uniformDistribution(generator));

		number->m_nValue = uniformDistribution(generator);
		bus.postNotification(notification.c_str(), number);
	}
	state.counters["sum"] = sum;
	CCPoolManager::sharedPoolManager()->purgePoolManager();
}

void check10NotificationsFor1kListeners_CCNotificationCenter(benchmark::State& state)
{
	checkNNotificationsForNListeners(state, 10, 1000);
}

void check100NotificationsFor1kListeners_CCNotificationCenter(benchmark::State& state)
{
	checkNNotificationsForNListeners(state, 100, 1000);
}

void check1kNotificationsFor1kListeners_CCNotificationCenter(benchmark::State& state)
{
	checkNNotificationsForNListeners(state, 1000, 1000);
}

void check100NotificationsFor10kListeners_CCNotificationCenter(benchmark::State& state)
{
	checkNNotificationsForNListeners(state, 100, 10000);
}

void checkNotifyFor10kListenersWhenNoOneListens_CCNotificationCenter(benchmark::State& state)
{
	using namespace cocos2d;
	cocos2d::CCNotificationCenter bus;
	int sum = 0;
	for(int i = 0; i < 10000; ++i)
	{
		auto observer = new SampleObserver{};
		observer->autorelease();
		observer->callback = [&](int value) { benchmark::DoNotOptimize(sum += value * 2); };
		bus.addObserver(observer, callfuncO_selector(SampleObserver::onCall), "sample", nullptr);
	}
	auto number = CCInteger::create(2);
	while(state.KeepRunning()) // Performance area!
	{
		bus.postNotification("unknown", number);
	}
	state.counters["sum"] = sum;
	CCPoolManager::sharedPoolManager()->purgePoolManager();
}

BENCHMARK(checkSimpleNotification_CCNotificationCenter);
BENCHMARK(check10Listeners_CCNotificationCenter);
BENCHMARK(check100Listeners_CCNotificationCenter);
BENCHMARK(check1kListeners_CCNotificationCenter);
BENCHMARK(check10NotificationsFor1kListeners_CCNotificationCenter);
BENCHMARK(check100NotificationsFor1kListeners_CCNotificationCenter);
BENCHMARK(check1kNotificationsFor1kListeners_CCNotificationCenter);
BENCHMARK(check100NotificationsFor10kListeners_CCNotificationCenter);
BENCHMARK(checkNotifyFor10kListenersWhenNoOneListens_CCNotificationCenter);
} // namespace cocos2dx
