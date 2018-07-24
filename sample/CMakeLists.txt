cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

project(Sample LANGUAGES CXX)

add_executable(Sample
		src/main.cpp
		)

if(NOT TARGET Dexode::EventBus)
	find_package(EventBus CONFIG REQUIRED)
endif()

target_link_libraries(Sample PRIVATE Dexode::EventBus)
