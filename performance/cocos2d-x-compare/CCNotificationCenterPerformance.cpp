//
// Created by Dawid Drozd aka Gelldur on 05.08.17.
//
#include <random>
#include <functional>

#include <benchmark/benchmark.h>
#include <support/CCNotificationCenter.h>
#include <cocoa/CCInteger.h>
#include <cocos2d-x/cocos2dx/cocoa/CCAutoreleasePool.h>

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

void CCNotificationCenter_checkNListeners(benchmark::State& state, const int listenersCount)
{
	using namespace cocos2d;
	cocos2d::CCNotificationCenter bus;
	int sum = 0;
	for (int i = 0; i < listenersCount; ++i)
	{
		auto observer = new SampleObserver{};
		observer->autorelease();
		observer->callback = [&](int value)
		{
			benchmark::DoNotOptimize(sum += value * 2);
		};
		bus.addObserver(observer, callfuncO_selector(SampleObserver::onCall), "sample", nullptr);
	}
	auto number = CCInteger::create(2);
	while (state.KeepRunning())//Performance area!
	{
		bus.postNotification("sample", number);
	}
	state.counters["sum"] = sum;
	CCPoolManager::sharedPoolManager()->purgePoolManager();
}

void CCNotificationCenter_checkSimpleNotification(benchmark::State& state)
{
	CCNotificationCenter_checkNListeners(state, 1);
}

void CCNotificationCenter_check10Listeners(benchmark::State& state)
{
	CCNotificationCenter_checkNListeners(state, 10);
}

void CCNotificationCenter_check100Listeners(benchmark::State& state)
{
	CCNotificationCenter_checkNListeners(state, 100);
}

void CCNotificationCenter_check1kListeners(benchmark::State& state)
{
	CCNotificationCenter_checkNListeners(state, 1000);
}

void CCNotificationCenter_checkNNotificationsForNListeners(benchmark::State& state
														   , const int notificationsCount
														   , const int listenersCount)
{
	std::mt19937 generator(311281);
	std::uniform_int_distribution<int> uniformDistribution(0, notificationsCount - 1);


	using namespace cocos2d;
	cocos2d::CCNotificationCenter bus;

	//We generate here N different notifications
	std::vector<std::string> notifications;
	notifications.reserve(notificationsCount);
	for (int i = 0; i < notificationsCount; ++i)
	{
		notifications.emplace_back(std::string{"notify_"} + std::to_string(i));
	}

	int sum = 0;
	for (int i = 0; i < listenersCount; ++i)//We register M listeners for N notifications using uniform distribution
	{
		auto observer = new SampleObserver{};
		observer->autorelease();
		observer->callback = [&](int value)
		{
			benchmark::DoNotOptimize(sum += value * 2);
		};

		const auto& notification = notifications.at(uniformDistribution(generator));
		bus.addObserver(observer, callfuncO_selector(SampleObserver::onCall), notification.c_str(), nullptr);
	}
	auto number = CCInteger::create(2);

	while (state.KeepRunning())//Performance area!
	{
		//Pick random notification
		const auto& notification = notifications.at(uniformDistribution(generator));

		number->m_nValue = uniformDistribution(generator);
		bus.postNotification(notification.c_str(), number);
	}
	state.counters["sum"] = sum;
	CCPoolManager::sharedPoolManager()->purgePoolManager();
}

void CCNotificationCenter_check10NotificationsFor1kListeners(benchmark::State& state)
{
	CCNotificationCenter_checkNNotificationsForNListeners(state, 10, 1000);
}

void CCNotificationCenter_check100NotificationsFor1kListeners(benchmark::State& state)
{
	CCNotificationCenter_checkNNotificationsForNListeners(state, 100, 1000);
}

void CCNotificationCenter_check1kNotificationsFor1kListeners(benchmark::State& state)
{
	CCNotificationCenter_checkNNotificationsForNListeners(state, 1000, 1000);
}

void CCNotificationCenter_check100NotificationsFor10kListeners(benchmark::State& state)
{
	CCNotificationCenter_checkNNotificationsForNListeners(state, 100, 10000);
}

BENCHMARK(CCNotificationCenter_checkSimpleNotification);
BENCHMARK(CCNotificationCenter_check10Listeners);
BENCHMARK(CCNotificationCenter_check100Listeners);
BENCHMARK(CCNotificationCenter_check1kListeners);
BENCHMARK(CCNotificationCenter_check10NotificationsFor1kListeners);
BENCHMARK(CCNotificationCenter_check100NotificationsFor1kListeners);
BENCHMARK(CCNotificationCenter_check1kNotificationsFor1kListeners);
BENCHMARK(CCNotificationCenter_check100NotificationsFor10kListeners);
}
