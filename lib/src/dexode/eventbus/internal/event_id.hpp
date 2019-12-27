#pragma once

#include <type_traits>

namespace dexode::eventbus::internal
{

template <typename T>
struct type_id_ptr
{
	static const T* const id;
};

template <typename T>
const T* const type_id_ptr<T>::id = nullptr;

using event_id_t = const void*;

template <typename T>
constexpr event_id_t event_id() // Helper for getting "type id"
{
	return &type_id_ptr<T>::id;
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
