cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

# Layout of project is inspired by: https://youtu.be/6sWec7b0JIc?t=20m50s
# This top level CMakeLists should be used for development

project(EventBusDev)

add_subdirectory(lib/)

enable_testing()
add_subdirectory(test/)
add_subdirectory(sample/)
add_subdirectory(performance/)

target_compile_options(EventBus PUBLIC
		-Wall -pedantic
		-Wnon-virtual-dtor
		-Werror
		-Wno-error=deprecated-declarations
		)
