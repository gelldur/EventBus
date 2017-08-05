# http://www.levelofindirection.com/journal/2010/12/28/unit-testing-in-c-and-objective-c-just-got-easier.html
# Thanks for CATCH!

ADD_EXECUTABLE(EventBusTest
		eventbus/EventCollectorTest.cpp
		eventbus/NotifierTest.cpp
		)

TARGET_INCLUDE_DIRECTORIES(EventBusTest PRIVATE Catch/single_include/)
TARGET_LINK_LIBRARIES(EventBusTest PUBLIC Dexode::EventBus)
