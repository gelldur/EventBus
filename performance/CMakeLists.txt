# http://www.levelofindirection.com/journal/2010/12/28/unit-testing-in-c-and-objective-c-just-got-easier.html
# Thanks for CATCH!

ADD_SUBDIRECTORY(benchmark/)

# If you want to compare with CCNotificationCenter read about it in README and uncomment line below
#INCLUDE(cocos2d-x-compare/Cocos2dxCompare.cmake)

ADD_EXECUTABLE(EventBusPerformance
		eventbus/EventBusPerformance.cpp
		${CCNOTIFICATION_CENTER_SRC}
		)

# TARGET_COMPILE_OPTIONS(EventBusPerformance PUBLIC
# 		-Wall -pedantic
# 		-Wno-unused-private-field
# 		-Wnon-virtual-dtor
# 		-Wno-gnu
# 		-Werror
# 		)

SET(EVENTBUS_DEBUG_FLAGS
		-O0 -fno-inline
		-DDEBUG
		-D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
		)

SET(EVENTBUS_RELEASE_FLAGS
		-DNDEBUG
		)

TARGET_COMPILE_OPTIONS(EventBusPerformance PUBLIC "$<$<CONFIG:DEBUG>:${EVENTBUS_DEBUG_FLAGS}>")
TARGET_COMPILE_OPTIONS(EventBusPerformance PUBLIC "$<$<CONFIG:RELEASE>:${EVENTBUS_RELEASE_FLAGS}>")

SET_TARGET_PROPERTIES(EventBusPerformance PROPERTIES
		CXX_STANDARD 14
		CXX_STANDARD_REQUIRED YES
		)

TARGET_INCLUDE_DIRECTORIES(EventBusPerformance PUBLIC
		./
		${CCNOTIFICATION_CENTER_INCLUDE}
		)

TARGET_LINK_LIBRARIES(EventBusPerformance PUBLIC Dexode::EventBus benchmark)
