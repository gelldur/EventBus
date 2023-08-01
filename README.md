# EventBus [![GitHub version](https://badge.fury.io/gh/gelldur%2FEventBus.svg)](https://badge.fury.io/gh/gelldur%2FEventBus)
&nbsp;&nbsp;[![Build status for Travis](https://travis-ci.org/gelldur/EventBus.svg?branch=master)](https://travis-ci.org/gelldur/EventBus)
&nbsp;&nbsp;[![Build status for Appveyor](https://ci.appveyor.com/api/projects/status/github/gelldur/EventBus)](https://ci.appveyor.com/project/gelldur/EventBus)

Simple and very fast event bus.
The EventBus library is a convenient realization of the observer pattern.
It works perfectly to supplement the implementation of MVC logic (model-view-controller) in event-driven UIs

General concept
![EventBus Diagram](docs/res/EventBus-concept.png)


EventBus was created because I want something easy to use and faster than [CCNotificationCenter](https://github.com/cocos2d/cocos2d-x/blob/v2/cocos2dx/support/CCNotificationCenter.h)
from [cocos2d-x](https://github.com/cocos2d/cocos2d-x) library. Of course C++11 support was mandatory at that moment.


EventBus main goals:
- Fast
- Easy to use
- Strongly typed
- Free
- tiny (~37 KB)
- Decouples notification senders and receivers
- on every platform you need (cross-platform)

# Brief @ presentation
Presentation [google docs](https://docs.google.com/presentation/d/1apAlKcVWo9FcqkPqL8108a1Fy9LGmhgLT56hSVpoI3w/edit?usp=sharing)

# Sample / use cases
You can checkout [use_case/](use_case/)  
If you want to play with sample online checkout this link: [wandbox.org](https://wandbox.org/permlink/VWo2acOX6hxUfV1Q)

# Usage
0. Store bus

```cpp
// store it in controller / singleton / std::shared_ptr whatever you want
auto bus = std::make_shared<EventBus>();
```

1. Define events

```cpp
namespace event // optional namespace
{
	struct Gold
	{
		int goldReceived = 0;
	};

	struct OK {}; // Simple event when user press "OK" button
}
```

2. Subscribe

```cpp
// ...
dexode::EventBus::Listener listener{bus};
listener.listen([](const event::Gold& event) // listen with lambda
                {
                     std::cout << "I received gold: " << event.goldReceived << " 💰" << std::endl;
                });

HudLayer* hudLayer;
// Hud layer will receive info about gold
hudLayer->listener.listen<event::Gold>(std::bind(&HudLayer::onGoldReceived, hudLayer, std::placeholders::_1));
```

3. Spread the news

```cpp
//Inform listeners about event
bus->postpone(event::Gold{12}); // 1 way
bus->postpone<event::Gold>({12}); // 2 way

event::Gold myGold{12};
bus->postpone(myGold); // 3 way
```

4. Process the events

```cpp
bus->process();
```

Checkout [tests](test/) or [use cases](use_case/) for more examples. Or create issue what isn't clear :)

# Add to your project
EventBus can be added as `ADD_SUBDIRECTORY` to your cmake file.
Then simply link it via `TARGET_LINK_LIBRARIES`
Example:
```
# No tests/benchmarks target won't be added. Root CMakeLists is for development.
ADD_SUBDIRECTORY(path/to/EventBus/lib)
ADD_EXECUTABLE(MyExecutable
		main.cpp
		)

TARGET_LINK_LIBRARIES(MyExecutable PUBLIC Dexode::EventBus)
```

Also if you want you can install library and add it at any way you want.
Eg.
```commandline
mkdir -p lib/build/
cd lib/build
cmake -DCMAKE_BUILD_TYPE=Relase -DCMAKE_INSTALL_PREFIX=~/.local/ ..

cmake --build . --target install
# OR
make && make install
```

Now in `Release/install` library is placed.

Or, you can install the library through your package manager (dpkg, rpm, etc). 
E.g. 
```commandline
mkdir -p lib/build/
cd lib/build

# For most RH-based Distributions
cmake -DCMAKE_BUILD_TYPE=Relase -DCPACK_GENERATOR="RPM" ..

# For most Debian-based Distributions
cmake -DCMAKE_BUILD_TYPE=Relase -DCPACK_GENERATOR="DEB" ..

# Or for both of them
cmake -DCMAKE_BUILD_TYPE=Relase -DCPACK_GENERATOR="RPM;DEB" ..

cmake --build . --target package
# Or
make package

# For most Debian-based systems
sudo dpkg -i EventBus*.deb

# For most RH-based systems
sudo rpm -i EventBus*.rpm
# OR
sudo yum install EventBus*.rpm
```

# Performance (could be outdated)
I have prepared some performance results. You can read about them [here](performance/README.md)
Small example:

```
check10NotificationsFor1kListeners                                     263 ns        263 ns    2668786 sum=-1.76281G
check10NotificationsFor1kListeners_CCNotificationCenter              11172 ns      11171 ns      62865 sum=54.023M

checkNotifyFor10kListenersWhenNoOneListens                              18 ns         18 ns   38976599 sum=0
checkNotifyFor10kListenersWhenNoOneListens_CCNotificationCenter     127388 ns     127378 ns       5460 sum=0
```

# Issues ? [![GitHub issues](https://img.shields.io/github/issues/gelldur/EventBus.svg)](https://github.com/gelldur/EventBus/issues)
Please report here issue / question / whatever, there is chance 99% I will answer ;)

If you have any questions or want to chat use gitter.

[![Join the chat at https://gitter.im/EventBusCpp/Lobby](https://badges.gitter.im/EventBusCpp/Lobby.svg)](https://gitter.im/EventBusCpp/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

# Tesing and metrics
// TODO


[![LoC](https://tokei.rs/b1/github/gelldur/EventBus)](https://github.com/gelldur/EventBus)
[![Coverage Status](https://coveralls.io/repos/github/gelldur/EventBus/badge.svg?branch=master)](https://coveralls.io/github/gelldur/EventBus?branch=master)

# Thanks to [![GitHub contributors](https://img.shields.io/github/contributors/gelldur/EventBus.svg)](https://github.com/gelldur/EventBus/graphs/contributors) [![Open Source Helpers](https://www.codetriage.com/gelldur/eventbus/badges/users.svg)](https://www.codetriage.com/gelldur/eventbus)
- [staakk](https://github.com/stanislawkabacinski) for fixing windows ;) [53d5026](https://github.com/gelldur/EventBus/commit/53d5026cad24810e82cd8d4a43d58cbfe329c502)
- [kuhar](https://github.com/kuhar) for his advice and suggestions for EventBus
- [swietlana](https://github.com/swietlana) for English correction and support ;)
- [ruslo](https://github.com/ruslo) for this great example: https://github.com/forexample/package-example

## For modern cmake refer
 - https://github.com/forexample/package-example
 - https://www.youtube.com/watch?v=6sWec7b0JIc

# License
EventBus source code can be used according to the **Apache License, Version 2.0**.
For more information see [LICENSE](LICENSE) file

If you don't like to read to much here is sumup about license [Apache 2.0](https://tldrlegal.com/license/apache-license-2.0-(apache-2.0)#summary)
