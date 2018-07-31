cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# http://www.levelofindirection.com/journal/2010/12/28/unit-testing-in-c-and-objective-c-just-got-easier.html
# Thanks for CATCH!

project(EventBusTest)

# Dependencies
enable_testing()
if (NOT TARGET Dexode::EventBus)
	find_package(EventBus CONFIG REQUIRED)
endif ()

find_package(Catch2 REQUIRED)

# Target definition
add_executable(EventBusTest
		src/EventCollectorTest.cpp
		src/NotifierTest.cpp
		)

target_compile_options(EventBusTest PUBLIC
		-Wall -pedantic
		-Wno-unused-private-field
		-Wnon-virtual-dtor
		-Wno-gnu
		-Werror
		)

# Don't do such thing:
# if(CMAKE_BUILD_TYPE STREQUAL DEBUG)
#		....
# else()
#		...
# endif()
#
# Instead do this way: (It will work for Visual Studio)
# target_compile_definitions(foo PRIVATE "VERBOSITY=$<IF:$<BOOL:${VERBOSE}>,30,10>")


set(EVENTBUS_DEBUG_FLAGS
		-O0 -fno-inline
		-DDEBUG
		#-D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
		)

target_compile_options(EventBusTest PUBLIC "$<$<CONFIG:DEBUG>:${EVENTBUS_DEBUG_FLAGS}>")

if (TARGET Catch2::Catch2) # Fix for naming library
	target_link_libraries(EventBusTest PUBLIC Dexode::EventBus Catch2::Catch2)
elseif (TARGET Catch2::Catch)
	target_link_libraries(EventBusTest PUBLIC Dexode::EventBus Catch2::Catch)
endif ()


add_test(NAME EventBus.UnitTests COMMAND EventBusTest)
