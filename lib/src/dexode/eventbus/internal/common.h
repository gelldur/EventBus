#pragma once

#include <type_traits>

namespace dexode::eventbus::internal
{

using event_id_t = std::size_t;

template <typename T>
constexpr event_id_t event_id() // Helper for getting "type id"
{
	return typeid(T).hash_code();
}

template <class Event>
constexpr bool validateEvent()
{
	static_assert(std::is_const<Event>::value == false, "Struct must be without const");
	static_assert(std::is_volatile<Event>::value == false, "Struct must be without volatile");
	static_assert(std::is_reference<Event>::value == false, "Struct must be without reference");
	static_assert(std::is_pointer<Event>::value == false, "Struct must be without pointer");
	return true;
}

} // namespace dexode::eventbus::internal
