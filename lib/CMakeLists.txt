cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# Use ';' to specify multiple e.g. ZIP;TGZ;DEB
set(CPACK_GENERATOR "" CACHE STRING "Set packages CPack should build e.g. ZIP;TGZ;DEB")

# BUILD_SHARED_LIBS can controll build type!
project(EventBus
		VERSION 2.6.0
		LANGUAGES CXX
		)

set(CMAKE_CXX_STANDARD 14)

# Dependencies
# No dependencies for EventBus yay!

# Introduce variables:
# * CMAKE_INSTALL_LIBDIR
# * CMAKE_INSTALL_BINDIR
# * CMAKE_INSTALL_INCLUDEDIR
include(GNUInstallDirs)

# Layout. This works for all platforms:
#   * <prefix>/lib*/cmake/<PROJECT-NAME>
#   * <prefix>/lib*/
#   * <prefix>/include/
set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

# Library definition
add_library(EventBus
	include/eventbus/EventBus.h
	include/eventbus/EventCollector.h
	include/eventbus/internal/AsyncCallbackVector.h
	include/eventbus/internal/CallbackVector.h
	include/eventbus/internal/TransactionCallbackVector.h
	include/eventbus/TokenHolder.h
	src/eventbus/AsyncEventBus.cpp include/eventbus/AsyncEventBus.h
	)
add_library(Dexode::EventBus ALIAS EventBus)

target_compile_features(EventBus PUBLIC cxx_std_14)

target_include_directories(EventBus PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
		$<INSTALL_INTERFACE:include/>
		PRIVATE lib/src/
		)

# Add definitions for targets
# Values:
#   * Debug:   -DEVENTBUS_DEBUG=1
#   * Release: -DEVENTBUS_DEBUG=0
#   * other:   -DEVENTBUS_DEBUG=0
target_compile_definitions(EventBus PUBLIC "EVENTBUS_DEBUG=$<CONFIG:Debug>")


# Configuration
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(namespace "Dexode::")

# Targets:
#   * <prefix>/lib/libEventBus.a
#   * header location after install: <prefix>/include/eventbus/EventBus.h
#   * headers can be included by C++ code `#include <eventbus/EventBus.h>`
install(TARGETS EventBus
		EXPORT "${TARGETS_EXPORT_NAME}"
		ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
		INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
		)

# Export headers
install(DIRECTORY include/eventbus
		DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
		)

# Include module with fuction 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Use:
#   * PROJECT_VERSION
write_basic_package_version_file(
		"${version_config}" COMPATIBILITY SameMajorVersion
)

# Configure '<PROJECT-NAME>Config.cmake'
# Use variables:
#   * TARGETS_EXPORT_NAME
#   * PROJECT_NAME
configure_package_config_file(
		"cmake/Config.cmake.in"
		"${project_config}"
		INSTALL_DESTINATION "${config_install_dir}"
)

# Config
#   * <prefix>/lib/cmake/EventBusventBusConfig.cmake
#   * <prefix>/lib/cmake/EventBus/EventBusConfigVersion.cmake
install(
		FILES "${project_config}" "${version_config}"
		DESTINATION "${config_install_dir}"
)

# Config
#   * <prefix>/lib/cmake/EventBus/EventBusTargets.cmake
install(EXPORT "${TARGETS_EXPORT_NAME}"
		DESTINATION "${config_install_dir}"
		NAMESPACE "${namespace}"
		)

# Cpack configuration
if(NOT CPACK_GENERATOR STREQUAL "")
	include("cmake/EventBus_CPack.cmake")
	enable_cpack(${CPACK_GENERATOR})
endif()
