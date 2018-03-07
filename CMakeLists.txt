CMAKE_MINIMUM_REQUIRED(VERSION 3.2 FATAL_ERROR)

OPTION(PERFORMANCE "Enable/Disable performance subdirectory" OFF)

# BUILD_SHARED_LIBS can controll build type!
PROJECT(EventBus
		VERSION 2.2.0
		LANGUAGES CXX
		)

ADD_LIBRARY(EventBus
		src/eventbus/EventCollector.cpp include/eventbus/EventCollector.h
		include/eventbus/EventBus.h
		)
ADD_LIBRARY(Dexode::EventBus ALIAS EventBus)

TARGET_INCLUDE_DIRECTORIES(EventBus PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
		$<INSTALL_INTERFACE:include/>
		PRIVATE src/
		)

# TARGET_COMPILE_OPTIONS(EventBus PRIVATE
# 		-Wall -pedantic
# 		-Wnon-virtual-dtor
# 		-Werror
# 		-Wno-error=deprecated-declarations
# 		)

IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	TARGET_COMPILE_OPTIONS(EventBus PRIVATE
		-Wno-error=unknown-argument
		-Wno-error=unused-command-line-argument
		)
ENDIF()

SET_TARGET_PROPERTIES(EventBus PROPERTIES
		CXX_STANDARD 14
		)

# IF(NOT CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
# 	TARGET_COMPILE_FEATURES(EventBus
# 			PUBLIC cxx_auto_type
# 			)
# ENDIF()

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

IF(BUILD_TESTING)
	ADD_SUBDIRECTORY(test/)
ENDIF()

IF(PERFORMANCE)
	ADD_SUBDIRECTORY(performance/)
ENDIF()
