cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# Layout of project is inspired by: https://youtu.be/6sWec7b0JIc?t=20m50s
# This top level CMakeLists should be used for development

project(EventBusDev)

option(ENABLE_TEST "Enable test" ON)
option(ENABLE_PERFORMANCE "Enable performance subproject" OFF)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(lib/)

if(ENABLE_TEST)
	enable_testing()
	add_subdirectory(test/)
endif()

add_subdirectory(sample/)

if(ENABLE_PERFORMANCE)
	add_subdirectory(performance/)
endif()

target_compile_options(EventBus PUBLIC
	-Wall -pedantic
	-Wnon-virtual-dtor
	-Werror
	-Wno-error=deprecated-declarations
	)
