CMAKE_MINIMUM_REQUIRED(VERSION 3.2 FATAL_ERROR)

PROJECT(Sample LANGUAGES CXX)

ADD_EXECUTABLE(Sample
		src/main.cpp
		)

# TARGET_COMPILE_OPTIONS(Sample PUBLIC
# 		-Wall -pedantic
# 		-Wno-unused-private-field
# 		-Wnon-virtual-dtor
# 		-Wno-gnu
# 		-Werror
# 		)

SET_TARGET_PROPERTIES(Sample PROPERTIES
		CXX_STANDARD 14
		)

ADD_SUBDIRECTORY(../ EventBus/)

TARGET_LINK_LIBRARIES(Sample PRIVATE Dexode::EventBus)
