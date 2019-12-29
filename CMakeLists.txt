cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

# Layout of project is inspired by: https://youtu.be/6sWec7b0JIc?t=20m50s
# This top level CMakeLists should be used for development

project(EventBusDev LANGUAGES CXX)

option(ENABLE_TEST "Enable test" ON)
option(ENABLE_PERFORMANCE "Enable performance subproject" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(lib/)
add_subdirectory(use_case/)

if(ENABLE_TEST)
	enable_testing()
	add_subdirectory(test/)
endif()

if(ENABLE_PERFORMANCE)
	add_subdirectory(performance/)
endif()

target_compile_options(EventBus PUBLIC
	-Wall -pedantic
	-Wnon-virtual-dtor
	-Werror
	-Wno-error=deprecated-declarations
	)
