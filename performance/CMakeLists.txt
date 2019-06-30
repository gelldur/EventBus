cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

# http://www.levelofindirection.com/journal/2010/12/28/unit-testing-in-c-and-objective-c-just-got-easier.html
# Thanks for CATCH!

find_package(Poco COMPONENTS Foundation Util)

set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
add_subdirectory(benchmark/)

if(NOT TARGET Dexode::EventBus)
	find_package(EventBus CONFIG REQUIRED)
endif()

# If you want to compare with CCNotificationCenter read about it in README and uncomment line below
#INCLUDE(cocos2d-x-compare/Cocos2dxCompare.cmake)

add_executable(EventBusPerformance
	src/EventBusPerformance.cpp
	${CCNOTIFICATION_CENTER_SRC}
	$<$<BOOL:${Poco_FOUND}>:src/PocoNotificationCenterPerformance.cpp>
	)

target_compile_options(EventBusPerformance PUBLIC
	-Wall -pedantic
	-Wno-unused-private-field
	-Wnon-virtual-dtor
	-Wno-gnu
	-Werror
	)

set(EVENTBUS_DEBUG_FLAGS
	-O0 -fno-inline
	-DDEBUG
	)


target_compile_options(EventBusPerformance PUBLIC "$<$<CONFIG:DEBUG>:${EVENTBUS_DEBUG_FLAGS}>")

target_include_directories(EventBusPerformance PUBLIC
	src/
	${CCNOTIFICATION_CENTER_INCLUDE}
	)

target_link_libraries(EventBusPerformance PUBLIC
	Dexode::EventBus
	benchmark benchmark_main
	$<$<BOOL:${Poco_FOUND}>:Poco::Foundation>
	$<$<BOOL:${Poco_FOUND}>:Poco::Util>
	)
