cmake_minimum_required(VERSION 3.11 FATAL_ERROR)

# Layout of project is inspired by: https://youtu.be/6sWec7b0JIc?t=20m50s
# This top level CMakeLists should be used for development

project(EventBusDev LANGUAGES CXX)

option(ENABLE_TEST "Enable test" ON)
option(ENABLE_PERFORMANCE "Enable performance subproject" OFF)
option(ENABLE_LIBCXX "Enable build with libc++" OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(ENABLE_LIBCXX)
	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
	else()
		message(FATAL_ERROR "C++ compiler should be set to clang")
	endif()
endif()

# Build Types
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE}
	CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel tsan asan lsan msan ubsan"
	FORCE)

# ThreadSanitizer
set(CMAKE_C_FLAGS_TSAN
	"-fsanitize=thread -g -O1"
	CACHE STRING "Flags used by the C compiler during ThreadSanitizer builds."
	FORCE)
set(CMAKE_CXX_FLAGS_TSAN
	"-fsanitize=thread -g -O1"
	CACHE STRING "Flags used by the C++ compiler during ThreadSanitizer builds."
	FORCE)

# AddressSanitize
set(CMAKE_C_FLAGS_ASAN
	"-fsanitize=address -fno-optimize-sibling-calls -fsanitize-address-use-after-scope -fno-omit-frame-pointer -g -O1"
	CACHE STRING "Flags used by the C compiler during AddressSanitizer builds."
	FORCE)
set(CMAKE_CXX_FLAGS_ASAN
	"-fsanitize=address -fno-optimize-sibling-calls -fsanitize-address-use-after-scope -fno-omit-frame-pointer -g -O1"
	CACHE STRING "Flags used by the C++ compiler during AddressSanitizer builds."
	FORCE)

# LeakSanitizer
set(CMAKE_C_FLAGS_LSAN
	"-fsanitize=leak -fno-omit-frame-pointer -g -O1"
	CACHE STRING "Flags used by the C compiler during LeakSanitizer builds."
	FORCE)
set(CMAKE_CXX_FLAGS_LSAN
	"-fsanitize=leak -fno-omit-frame-pointer -g -O1"
	CACHE STRING "Flags used by the C++ compiler during LeakSanitizer builds."
	FORCE)

# Remember to switch lib: https://stackoverflow.com/questions/20617788/using-memory-sanitizer-with-libstdc/20784130#20784130
# use ENABLE_LIBCXX
# MemorySanitizer
set(CMAKE_C_FLAGS_MSAN
	"-fsanitize=memory -fno-optimize-sibling-calls -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer -g -O2"
	CACHE STRING "Flags used by the C compiler during MemorySanitizer builds."
	FORCE)
set(CMAKE_CXX_FLAGS_MSAN
	"-fsanitize=memory -fno-optimize-sibling-calls -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer -g -O2"
	CACHE STRING "Flags used by the C++ compiler during MemorySanitizer builds."
	FORCE)

# UndefinedBehaviour
set(CMAKE_C_FLAGS_UBSAN
	"-fsanitize=undefined"
	CACHE STRING "Flags used by the C compiler during UndefinedBehaviourSanitizer builds."
	FORCE)
set(CMAKE_CXX_FLAGS_UBSAN
	"-fsanitize=undefined"
	CACHE STRING "Flags used by the C++ compiler during UndefinedBehaviourSanitizer builds."
	FORCE)

add_subdirectory(lib)
add_subdirectory(use_case)

if(ENABLE_TEST)
	enable_testing()
	add_subdirectory(test/)
endif()

if(ENABLE_PERFORMANCE)
	add_subdirectory(performance/)
endif()

if(NOT MSVC)
	target_compile_options(EventBus PRIVATE
	-Wall -pedantic
	-Wnon-virtual-dtor
	-Werror
	-Wno-error=deprecated-declarations
	)
endif()