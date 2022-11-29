cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

# Use ';' to specify multiple e.g. ZIP;TGZ;DEB
set(CPACK_GENERATOR "" CACHE STRING "Set packages CPack should build e.g. ZIP;TGZ;DEB")

# BUILD_SHARED_LIBS can controll build type!
project(EventBus
	VERSION 3.0.3
	LANGUAGES CXX
	)

# Dependencies
find_package(Threads)

# Introduce variables:
# * CMAKE_INSTALL_LIBDIR
# * CMAKE_INSTALL_BINDIR
# * CMAKE_INSTALL_INCLUDEDIR
include(GNUInstallDirs)
include(cmake/InstallHelp.cmake)

# Layout. This works for all platforms:
#   * <prefix>/lib*/cmake/<PROJECT-NAME>
#   * <prefix>/lib*/
#   * <prefix>/include/
set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

set(EventBus_PUBLIC_HEADERS
	src/dexode/EventBus.hpp
	src/dexode/eventbus/Bus.hpp
	src/dexode/eventbus/internal/event_id.hpp
	src/dexode/eventbus/internal/listener_traits.hpp
	src/dexode/eventbus/internal/ListenerAttorney.hpp
	src/dexode/eventbus/Listener.hpp
	src/dexode/eventbus/perk/PassPerk.hpp
	src/dexode/eventbus/perk/Perk.hpp
	src/dexode/eventbus/perk/PerkEventBus.hpp
	src/dexode/eventbus/perk/TagPerk.hpp
	src/dexode/eventbus/perk/WaitPerk.hpp
	src/dexode/eventbus/permission/PostponeBus.hpp
	src/dexode/eventbus/stream/EventStream.hpp
	src/dexode/eventbus/stream/ProtectedEventStream.hpp
	)

# Library definition
add_library(EventBus
	${EventBus_PUBLIC_HEADERS}
	src/dexode/EventBus.cpp
	src/dexode/eventbus/perk/PassPerk.cpp
	src/dexode/eventbus/perk/Perk.cpp
	src/dexode/eventbus/perk/PerkEventBus.cpp
	src/dexode/eventbus/perk/TagPerk.cpp
	src/dexode/eventbus/perk/WaitPerk.cpp
	)
add_library(Dexode::EventBus ALIAS EventBus)

# Why C++ 17 needed:
# - std::shared_mutex used
# - nested namespaces e.g. my::name::space

set_target_properties(EventBus PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED YES
	CXX_EXTENSIONS NO # -std=c++17 rather than -std=gnu++17
	)

target_compile_features(EventBus PUBLIC cxx_std_17)

target_include_directories(EventBus PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>
	$<INSTALL_INTERFACE:include/>
	)
target_link_libraries(EventBus ${CMAKE_THREAD_LIBS_INIT})

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
	# We don't want someone by accident modify his installed files
	PERMISSIONS OWNER_EXECUTE OWNER_READ
)

# Config
#   * <prefix>/lib/cmake/EventBus/EventBusTargets.cmake
install(EXPORT "${TARGETS_EXPORT_NAME}"
	DESTINATION "${config_install_dir}"
	NAMESPACE "${namespace}"
	# We don't want someone by accident modify his installed files
	PERMISSIONS OWNER_EXECUTE OWNER_READ
	)

# Export headers (Install public headers)
install_public_headers_with_directory(EventBus_PUBLIC_HEADERS "src/")

# Cpack configuration
if(NOT CPACK_GENERATOR STREQUAL "")
	include(cmake/EventBus_CPack.cmake)
	enable_cpack(${CPACK_GENERATOR})
endif()
