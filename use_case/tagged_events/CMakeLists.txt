#
add_executable(UseCase_TaggedEvents
	src/Character.cpp src/Character.hpp
	src/event.hpp
	src/EventBus.hpp
	src/main.cpp
	src/Team.cpp src/Team.hpp
	src/Gui.cpp src/Gui.hpp)

target_include_directories(UseCase_TaggedEvents PUBLIC src/)
target_link_libraries(UseCase_TaggedEvents PRIVATE Dexode::EventBus)
