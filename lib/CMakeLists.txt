cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# BUILD_SHARED_LIBS can controll build type!
project(EventBus
		VERSION 2.2.0
		LANGUAGES CXX
		)

# Dependencies
# No dependencies for EventBus yay!

# Library definition
add_library(EventBus
		src/EventCollector.cpp include/eventbus/EventCollector.h
		include/eventbus/EventBus.h
		)
add_library(Dexode::EventBus ALIAS EventBus)

target_compile_features(EventBus PUBLIC cxx_std_14)

target_include_directories(EventBus PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
		$<INSTALL_INTERFACE:include/>
		PRIVATE lib/src/
		)

install(TARGETS EventBus EXPORT EventBusTargets
		ARCHIVE DESTINATION lib/
		LIBRARY DESTINATION lib/
		RUNTIME DESTINATION bin/
		INCLUDES DESTINATION include/
		)

install(EXPORT EventBusTargets
		DESTINATION lib/cmake/EventBus
		FILE EventBusTargets.cmake
		NAMESPACE Dexode::
		)

# Copy public headers
install(DIRECTORY include/eventbus
		DESTINATION include/
		)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(EventBusConfigVersion.cmake COMPATIBILITY SameMajorVersion)

install(FILES EventBusConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/EventBusConfigVersion.cmake
		DESTINATION lib/cmake/EventBus)
