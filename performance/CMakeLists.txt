# http://www.levelofindirection.com/journal/2010/12/28/unit-testing-in-c-and-objective-c-just-got-easier.html
# Thanks for CATCH!

ADD_SUBDIRECTORY(benchmark/)

# If you want to compare with CCNotificationCenter read about it in README and uncomment line below
#INCLUDE(cocos2d-x-compare/Cocos2dxCompare.cmake)

ADD_EXECUTABLE(EventBusPerformance
		eventbus/EventBusPerformance.cpp
		${CCNOTIFICATION_CENTER_SRC}
		)

TARGET_COMPILE_OPTIONS(EventBusPerformance
		PRIVATE -Wall -pedantic -Wno-unused-private-field -Wnon-virtual-dtor
		-Werror
		)

SET_TARGET_PROPERTIES(EventBusPerformance PROPERTIES
		CXX_STANDARD 14
		CXX_STANDARD_REQUIRED YES
		)

TARGET_INCLUDE_DIRECTORIES(EventBusPerformance PRIVATE
		./
		${CCNOTIFICATION_CENTER_INCLUDE}
		)

TARGET_LINK_LIBRARIES(EventBusPerformance PUBLIC Dexode::EventBus benchmark)
