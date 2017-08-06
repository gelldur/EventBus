# http://www.levelofindirection.com/journal/2010/12/28/unit-testing-in-c-and-objective-c-just-got-easier.html
# Thanks for CATCH!

ADD_EXECUTABLE(EventBusTest
		eventbus/EventCollectorTest.cpp
		eventbus/NotifierTest.cpp
		eventbus/NotificationTest.cpp
		)

SET_TARGET_PROPERTIES(EventBusTest PROPERTIES
		CXX_STANDARD 14
		CXX_STANDARD_REQUIRED YES
		)

TARGET_COMPILE_OPTIONS(EventBusTest
		PRIVATE -Wall -pedantic -Wno-unused-private-field -Wnon-virtual-dtor -Wno-gnu
		-Werror
		)

TARGET_INCLUDE_DIRECTORIES(EventBusTest PRIVATE Catch/single_include/)
TARGET_LINK_LIBRARIES(EventBusTest PUBLIC Dexode::EventBus)
