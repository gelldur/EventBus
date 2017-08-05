# http://www.levelofindirection.com/journal/2010/12/28/unit-testing-in-c-and-objective-c-just-got-easier.html
# Thanks for CATCH!

ADD_SUBDIRECTORY(benchmark/)

ADD_EXECUTABLE(EventBusPerformance
		eventbus/EventBusPerformance.cpp
		)

TARGET_LINK_LIBRARIES(EventBusPerformance PUBLIC Dexode::EventBus benchmark)
