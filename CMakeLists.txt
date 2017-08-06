CMAKE_MINIMUM_REQUIRED(VERSION 3.6 FATAL_ERROR)

# BUILD_SHARED_LIBS can controll build type!
PROJECT(EventBus
		VERSION 2.0.0
		LANGUAGES CXX
		)

ADD_LIBRARY(EventBus
		src/eventbus/EventCollector.cpp include/eventbus/EventCollector.h
		include/eventbus/Event.h
		include/eventbus/EventBus.h
		)
ADD_LIBRARY(Dexode::EventBus ALIAS EventBus)

TARGET_INCLUDE_DIRECTORIES(EventBus PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
		$<INSTALL_INTERFACE:include/>
		PRIVATE src/
		)

TARGET_COMPILE_OPTIONS(EventBus
		PRIVATE -Wall -pedantic -Wno-unused-private-field -Wnon-virtual-dtor #-Werror
		)

SET_TARGET_PROPERTIES(EventBus PROPERTIES
		CXX_STANDARD 14
		CXX_STANDARD_REQUIRED YES
		)

TARGET_COMPILE_FEATURES(EventBus
		PUBLIC cxx_auto_type cxx_variadic_templates
		)

INSTALL(TARGETS EventBus EXPORT EventBusConfig
		ARCHIVE DESTINATION lib/
		LIBRARY DESTINATION lib/
		RUNTIME DESTINATION bin/
		INCLUDES DESTINATION include/
		)
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION include/ FILES_MATCHING PATTERN "*.h*")
INSTALL(EXPORT EventBusConfig
		DESTINATION cmake/
		NAMESPACE Dexode::
		)
EXPORT(TARGETS EventBus FILE EventBusConfig.cmake)


ENABLE_TESTING()
ADD_SUBDIRECTORY(test/)
ADD_SUBDIRECTORY(performance/)
