cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

# http://www.levelofindirection.com/journal/2010/12/28/unit-testing-in-c-and-objective-c-just-got-easier.html
# Thanks for CATCH!

project(EventBusTest)

# Dependencies
enable_testing()
if(NOT TARGET Dexode::EventBus)
	find_package(EventBus CONFIG REQUIRED)
endif()

find_package(Catch2 2.10 REQUIRED)
find_package(Threads REQUIRED)

add_subdirectory(unit)
add_subdirectory(integration)
