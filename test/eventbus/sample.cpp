//
// Created by Dawid Drozd aka Gelldur on 06.08.17.
//

#include <eventbus/EventBus.h>
#include <eventbus/EventCollector.h>

void sampleUsages()
{
	{
		//Notify by Event object
		Dexode::EventBus bus;
		Dexode::Event<int> simpleEvent{"simple"};
		//...
		bus.notify(simpleEvent, 2);//Everyone who listens will receive this notification.
	}
	{
		//Notify without Event object
		Dexode::EventBus bus;
		//...
		bus.notify<int>("simple", 2);//Everyone who listens will receive this notification.
	}
	{
		//Lambda listener
		Dexode::EventBus bus;
		//...
		int token = bus.listen<int>("simple", [](int value) // register listener
		{
		});
		//If we want unlisten exact listener we can use token for it
		bus.unlistenAll(token);
	}
	{
		//Unlisten everyone who is waiting for event X //TODO
		Dexode::EventBus bus;
		Dexode::Event<int> event{"simple"};
		//...
		int token = bus.listen(event, [](int value) // register listener
		{
		});
		//If we want unlisten exact listener we can use token for it
	}
	{
		//Listen on some token
		Dexode::EventBus bus;
		Dexode::Event<int> event{"simple"};
		//...
		int token = bus.listen(event, [](int value) // register listener
		{
		});

		bus.listen(token, event, [](int value) // another listener
		{
		});

		bus.unlistenAll(token);//Now those two lambdas will be removed from listeners
	}
	{
		//EventCollector sample
		Dexode::EventBus bus;
		Dexode::Event<int> event{"simple"};
		Dexode::EventCollector collector{&bus};
		//...
		collector.listen(event, [](int value) // register listener
		{
		});

		collector.listen(event, [](int value) // another listener
		{
		});

		collector.unlistenAll();//Now those two lambdas will be removed from listeners
	}
	{
		class Example
		{
		public:
			Example(Dexode::EventBus& bus)
					: _collector{&bus}
			{
				_collector.listen<int>("event1", std::bind(&Example::onEvent1, this, std::placeholders::_1));
				_collector.listen<std::string>("event2", std::bind(&Example::onEvent2, this, std::placeholders::_1));
			}

			void onEvent1(int value)
			{
			}

			void onEvent2(std::string value)
			{
			}

		private:
			Dexode::EventCollector _collector;// use RAII
		};

		//EventCollector sample
		Dexode::EventBus bus;
		Example ex{bus};
		//...
		bus.notify<int>("event1", 2);
	}
}
