#
# Target definition
add_executable(EventBus-IntegrationTest
	src/dexode/eventbus/test/SuiteWait.cpp
	src/main.cpp
	)

target_include_directories(EventBus-IntegrationTest PRIVATE src/)

target_compile_options(EventBus-IntegrationTest PUBLIC
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

target_compile_options(EventBus-IntegrationTest PUBLIC "$<$<CONFIG:DEBUG>:${EVENTBUS_DEBUG_FLAGS}>")

target_link_libraries(EventBus-IntegrationTest PUBLIC Catch2::Catch2 Dexode::EventBus Threads::Threads)


add_test(NAME EventBus.UnitTests COMMAND EventBus-IntegrationTest)
