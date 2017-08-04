CMAKE_MINIMUM_REQUIRED(VERSION 3.6 FATAL_ERROR)

# BUILD_SHARED_LIBS can controll build type!
PROJECT(EventBus
		VERSION 1.0.12
		LANGUAGES CXX
		)

ADD_LIBRARY(EventBus
		src/eventbus/EventCollector.cpp src/eventbus/EventCollector.h
		src/eventbus/Notification.cpp src/eventbus/Notification.h
		src/eventbus/Notifier.h
		)
ADD_LIBRARY(Dexode::EventBus ALIAS EventBus)

TARGET_INCLUDE_DIRECTORIES(EventBus PUBLIC src/)

TARGET_COMPILE_OPTIONS(EventBus
		PUBLIC $<$<COMPILE_LANGUAGE:CXX>:-std=c++14>
		PRIVATE -Wall -pedantic -Wno-unused-private-field -Wnon-virtual-dtor -Werror
		)

TARGET_COMPILE_FEATURES(EventBus
		PUBLIC cxx_auto_type cxx_variadic_templates
		)

