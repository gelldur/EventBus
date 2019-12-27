#
add_executable(UseCase_Basic
	src/main.cpp
	)

target_link_libraries(UseCase_Basic PRIVATE Dexode::EventBus)
